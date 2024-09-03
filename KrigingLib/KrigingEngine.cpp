#include "KrigingEngine.hpp"

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
		LogAndThrow<std::invalid_argument>("Unsupported variogram model");
	}

	//TODO: Add support for anisotropy and rotated variograms with rotation matrix
}

double KrigingEngine::Covariance(const double h, const VariogramParameters& parameters)
{
	double gamma_h = Variogram(h, parameters);
	return parameters.Sill - gamma_h;
}

double KrigingEngine::OrdinaryKrigingPoint(const double x0, const double y0, const double z0,
	const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<double>& zs,
	const std::vector<double>& values, const VariogramParameters& parameters)
{
	size_t n = values.size();

	Eigen::MatrixXd C(n + 1, n + 1); // LHS Covariance matrix cij between sample locations i,j
	Eigen::VectorXd D(n + 1); // RHS Covariance vector ci0 between sample locations i and estimation point 0

	// Fill the kriging matrix with covariance values
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
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
	for (size_t i = 0; i < n; ++i)
	{
		D(i) = Covariance(EuclideanDistance(xs[i], ys[i], zs[i], x0, y0, z0), parameters);
	}
	D(n) = 1.0;

	// Solve for the kriging weights
	Eigen::VectorXd weights = C.colPivHouseholderQr().solve(D);

	// Compute the kriged value
	double krigedValue = 0.0;
	for (size_t i = 0; i < n; ++i)
	{
		krigedValue += weights[i] * values[i];
	}

	return krigedValue;
}

double KrigingEngine::KrigeOneBlock(const double blockX, const double blockY, const double blockZ,
	const KrigingParameters& parameters, const Composites& composites)
{
	// Find nearest composites
	auto nearestComposites = composites.FindNearestComposites(blockX, blockY, blockZ, parameters.MaxNumComposites, parameters.MaxRadius);

	// Skip block if not enough composites
	if (nearestComposites.Indices.size() < parameters.MinNumComposites)
	{
		return NullValue;
	}

	// Create subset of composites based on indices
	std::vector<double> subsetX;
	std::vector<double> subsetY;
	std::vector<double> subsetZ;
	std::vector<double> subsetGrade;

	for (size_t index : nearestComposites.Indices)
	{
		subsetX.push_back(composites.GetX(index));
		subsetY.push_back(composites.GetY(index));
		subsetZ.push_back(composites.GetZ(index));
		subsetGrade.push_back(composites.GetGrade(index));
	}

	return OrdinaryKrigingPoint(blockX, blockY, blockZ,
		subsetX, subsetY, subsetZ, subsetGrade, parameters.VariogramParameters);
}

void KrigingEngine::RunKriging(Blocks& blocks, const KrigingParameters& parameters, const Composites& composites)
{
	std::cout << "Running kriging." << '\n';

	const size_t numBlocks = blocks.GetSize();

	// Process blocks in batches
	size_t batchSize = GetThreadBatchSize(numBlocks);
	std::vector<std::future<void>> futures;
	for (size_t i = 0; i < numBlocks; i += batchSize)
	{
		futures.push_back(std::async(std::launch::async, [&blocks, &parameters, &composites, i, batchSize, numBlocks] {
			size_t end = std::min(i + batchSize, numBlocks);
			for (size_t j = i; j < end; ++j)
			{
				blocks.Grade[j] = KrigeOneBlock(blocks.GetX(j), blocks.GetY(j), blocks.GetZ(j), parameters, composites);
			}
			}));
	}

	// Wait for all tasks to complete
	for (auto& fut : futures)
	{
		fut.get();
	}
	std::cout << "Kriging completed." << '\n';
}

size_t KrigingEngine::GetThreadBatchSize(const size_t numBlocks)
{
	size_t numThread;
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

double KrigingEngine::EuclideanDistance(const double x1, const double y1, const double z1, const double x2, const double y2, const double z2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}