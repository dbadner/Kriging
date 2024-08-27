#include "KrigingEngine.hpp"

KrigingEngine::KrigingEngine(const KrigingParameters& parameters, const Composites& composites, Blocks& blocks)
	: mParameters(parameters), mComposites(composites), mBlocks(blocks)
{
	//TODO: Add validation of input parameters
}

// Returns the variogram for a given lag distance h and variogram parameters
double KrigingEngine::Variogram(const double h, const VariogramParameters& parameters)
{
	// lag distance normalized by the range
	double ha = h / parameters.Range;
	double contribution = parameters.Sill - parameters.Nugget;

	switch (parameters.Structure)
	{
	case VariogramParameters::Spherical:
		if (h > parameters.Range)
		{
			return parameters.Sill;
		}
		return parameters.Nugget + contribution * (1.5 * ha - 0.5 * ha * ha * ha);
	case VariogramParameters::Exponential:
		return parameters.Nugget + contribution * (1 - exp(-3 * ha));
	case VariogramParameters::Gaussian:
		return parameters.Nugget + contribution * (1 - exp(-3 * ha * ha));
	default:
		throw std::invalid_argument("Unsupported variogram model");
	}

	//TODO: Add support for anisotropy and rotated variograms with rotation matrix
}

// Returns the covariance for a given lag distance h and variogram parameters
double KrigingEngine::Covariance(const double h, const VariogramParameters& parameters)
{
	double gamma_h = Variogram(h, parameters);
	return parameters.Sill - gamma_h;
}

// Function to perform point kriging
double KrigingEngine::OrdinaryKrigingPoint(double x0, double y0, double z0,
	const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<double>& zs,
	const std::vector<double>& values, const VariogramParameters& parameters)
{
	size_t n = xs.size();

	if (n != ys.size() || n != zs.size() || n != values.size()) 
	{
		throw std::invalid_argument("Input vectors must have the same size");
	}

	Eigen::MatrixXd C(n + 1, n + 1); // LHS Covariance matrix cij between sample locations i,j
	Eigen::VectorXd D(n + 1); // RHS Covariance vector ci0 between sample locations i and estimation point 0

	// Fill the kriging matrix with covariance values
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			C(i, j) = Covariance(EuclideanDistance(xs[i], ys[i], zs[i], xs[j], ys[j], zs[j]), parameters);
		}
		// Lagrange multiplier
		C(i, n) = 1.0;
		C(n, i) = 1.0;
	}
	// Bottom-right corner for Lagrange multiplier
	C(n, n) = 0.0;

	// Fill the right-hand side vector
	for (int i = 0; i < n; ++i) 
	{
		D(i) = Covariance(EuclideanDistance(xs[i], ys[i], zs[i], x0, y0, z0), parameters);
	}
	D(n) = 1.0;

	// Solve for the kriging weights
	Eigen::VectorXd weights = C.colPivHouseholderQr().solve(D);

	// Compute the kriged value
	double krigedValue = 0.0;
	for (int i = 0; i < n; ++i) 
	{
		krigedValue += weights[i] * values[i];
	}

	return krigedValue;
}

double KrigingEngine::KrigeOneBlock(double blockX, double blockY, double blockZ)
{
	int maxNumComp = mParameters.MaxNumComposites;
	int minNumComp = mParameters.MinNumComposites;
	double maxRadius = mParameters.MaxRadius;

	// Find nearest composites
	auto nearestComposites = mComposites.FindNearestComposites(blockX, blockY, blockZ, maxNumComp, maxRadius);

	// Skip block if not enough composites
	if (nearestComposites.Indices.size() < minNumComp)
	{
		return NullValue;
	}

	// Create subset of composites based on indices
	std::vector<double> subsetX;
	std::vector<double> subsetY;
	std::vector<double> subsetZ;
	std::vector<double> subsetGrade;

	for (int index : nearestComposites.Indices)
	{
		subsetX.push_back(mComposites.X[index]);
		subsetY.push_back(mComposites.Y[index]);
		subsetZ.push_back(mComposites.Z[index]);
		subsetGrade.push_back(mComposites.Grade[index]);
	}

	return OrdinaryKrigingPoint(blockX, blockY, blockZ,
		subsetX, subsetY, subsetZ, subsetGrade, mParameters.VariogramParameters);
}

//Kriging for all blocks in parallel
void KrigingEngine::RunKriging()
{
	const int numBlocks = mBlocks.I.size();
	if (numBlocks < 1)
	{
		return;
	}

	// Process blocks in batches
	// TODO: Confirm thread safety. Assess further threading optimization
	int batchSize = GetThreadBatchSize(numBlocks);
	std::vector<std::future<void>> futures;
	for (int i = 0; i < numBlocks; i += batchSize)
	{
		futures.push_back(std::async(std::launch::async, [this, i, batchSize, numBlocks]() {
			int end = std::min(i + batchSize, numBlocks);
			for (int j = i; j < end; ++j)
			{
				mBlocks.Grade[j] = KrigeOneBlock(mBlocks.X[j], mBlocks.Y[j], mBlocks.Z[j]);
			}
			}));
	}

	// Wait for all tasks to complete
	for (auto& fut : futures)
	{
		fut.get();
	}
}

// Define the batch size for parallel processing based on # blocks and hardware specifications
int KrigingEngine::GetThreadBatchSize(int numBlocks)
{
	int numThread;
	try
	{
		numThread = std::thread::hardware_concurrency();
		if (numThread == 0)
		{
			throw std::runtime_error("Unable to determine the number of threads.");
		}
	}
	catch (const std::exception& e)
	{
		int defaultNumThread = 8;
		std::cerr << "Exception caught: " << e.what() << ". Using default number of threads: " << defaultNumThread << std::endl;
		numThread = defaultNumThread;
	}
	numThread = std::min(numThread, numBlocks);

	// Round the batch size up to the nearest whole number
	return (numBlocks + numThread - 1) / numThread;
}

// Returns the Euclidean distance between two points
double KrigingEngine::EuclideanDistance(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}