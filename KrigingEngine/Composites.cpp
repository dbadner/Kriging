#include "Composites.hpp"

Composites::Composites(const std::string csvFilePath, const CoordinateExtents blockExtents, const double maxSearchRadius)
{
	ReadCompositesFromCSV(csvFilePath, blockExtents, maxSearchRadius);
	BuildKdTree();
}

Composites::Composites(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& grades)
	: X(x), Y(y), Z(z), Grade(grades)
{
	auto n = x.size();
	if (n != y.size() || n != z.size())
	{
		throw std::runtime_error("X,Y,Z vectors must be the same size.");
	}

	BuildKdTree();
}

Composites::~Composites()
{
	delete mKdTree;
}

NearestCompositesResult Composites::FindNearestComposites(const double x, const double y, const double z, const int n, const double maxDist) const
{
	// TODO: Optimize this method and nanoflann parameters for improved performance, move reusable objects to the thread level

	double point[3] = { x, y, z };
	double maxDistSq = maxDist * maxDist;

	// Perform the nearest neighbor search
	std::vector<size_t> indices(n);
	std::vector<double> distancesSq(n);
	nanoflann::KNNResultSet<double> resultSet(n);
	resultSet.init(indices.data(), distancesSq.data());
	mKdTree->findNeighbors(resultSet, &point[0]);

	// Filter out distances greater than maxDist
	NearestCompositesResult result;
	for (size_t i = 0; i < indices.size(); ++i)
	{
		if (distancesSq[i] <= maxDistSq)
		{
			result.Indices.push_back(indices[i]);
			result.Distances.push_back(sqrt(distancesSq[i]));
		}
	}

	return result;
}

// Add the required methods for Nanoflann
inline size_t Composites::kdtree_get_point_count() const
{
	return X.size();
}

inline double Composites::kdtree_distance(const double* p1, const size_t idxp2) const
{
	double d0 = p1[0] - X[idxp2];
	double d1 = p1[1] - Y[idxp2];
	double d2 = p1[2] - Z[idxp2];
	return d0 * d0 + d1 * d1 + d2 * d2;
}

inline double Composites::kdtree_get_pt(const size_t idx, const int dim) const
{
	if (dim == 0) return X[idx];
	if (dim == 1) return Y[idx];
	return Z[idx];
}

void Composites::ReadCompositesFromCSV(const std::string& filePath, const CoordinateExtents& blockExtents, const double maxSearchRadius)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::ostringstream oss;
		oss << "File does not exist or cannot be opened: " << filePath;
		throw std::runtime_error(oss.str());
	}

	std::string line;

	// Read header line
	std::getline(file, line);

	// Parse header
	std::unordered_map<std::string, size_t> columnIndices;
	std::istringstream headerStream(line);
	std::string header;
	size_t columnIndex = 0;
	while (std::getline(headerStream, header, ','))
	{
		// Trim whitespace and convert to lowercase for consistency
		header.erase(header.find_last_not_of(" \n\r\t") + 1);
		std::transform(header.begin(), header.end(), header.begin(), tolower);
		columnIndices[header] = columnIndex++;
	}

	// Check required columns
	for (const auto& col : mRequiredColumns)
	{
		if (columnIndices.find(col) == columnIndices.end())
		{
			file.close();
			std::ostringstream oss;
			oss << "Missing required column: " << col;
			throw std::runtime_error(oss.str());
		}
	}

	// Calculate extents within which to import
	CoordinateExtents extents;
	CoordinateExtents blockCoordExtents;
	extents.MaxX = blockExtents.MaxX + maxSearchRadius;
	extents.MinX = blockExtents.MinX - maxSearchRadius;
	extents.MaxY = blockExtents.MaxY + maxSearchRadius;
	extents.MinY = blockExtents.MinY - maxSearchRadius;
	extents.MaxZ = blockExtents.MaxZ + maxSearchRadius;
	extents.MinZ = blockExtents.MinZ - maxSearchRadius;

	ReadComposites(file, columnIndices, extents);

	file.close();
}

void Composites::ReadComposites(std::ifstream& file, std::unordered_map<std::string, size_t>& columnIndices, const CoordinateExtents& extents)
{
	// Read data
	std::string line;
	size_t invalidRows = 0;
	size_t irrelevantRows = 0;
	double x, y, z, grade;
	while (std::getline(file, line))
	{
		std::istringstream lineStream(line);
		std::string cell;
		std::vector<std::string> cells;

		while (std::getline(lineStream, cell, ','))
		{
			// Trim whitespace
			cell.erase(cell.find_last_not_of(" \n\r\t") + 1);
			cells.push_back(cell);
		}

		if (cells.size() < columnIndices.size())
		{
			++invalidRows;
			continue;
		}

		// Extract values based on column indices
		try
		{
			x = std::stod(cells[columnIndices[mXColName]]);
			y = std::stod(cells[columnIndices[mYColName]]);
			z = std::stod(cells[columnIndices[mZColName]]);
			grade = std::stod(cells[columnIndices[mGradeColName]]);

			// Assume grade < 0 is invalid; could be user input in future
			if (grade < 0)
			{
				++invalidRows;
				continue;
			}
			// Check whether composite is relevant
			if (!IsRelevantComposite(x, y, z, grade, extents))
			{
				++irrelevantRows;
				continue;
			}

			// Valid, relevant composite; add to composite list
			X.emplace_back(x);
			Y.emplace_back(y);
			Z.emplace_back(z);
			Grade.emplace_back(grade);
		}
		catch (const std::invalid_argument& e)
		{
			++invalidRows;
			continue;
		}
		catch (const std::out_of_range& e)
		{
			++invalidRows;
			continue;
		}
	}

	std::cout << "Number of composites imported: " << X.size() << '\n';
	std::cout << "Number of rows skipped due to invalid data: " << invalidRows << '\n';
	std::cout << "Number of rows skipped due to irrelevant data beyond interpolation extents: " << irrelevantRows << '\n';
}

bool Composites::IsRelevantComposite(const double x, const double y, const double z, const double grade, const CoordinateExtents& extents)
{
	// Don't import composite if beyond relevant interpolation limits
	if (x < extents.MinX || x > extents.MaxX)
	{
		return false;
	}
	if (y < extents.MinY || y > extents.MaxY)
	{
		return false;
	}
	if (z < extents.MinZ || z > extents.MaxZ)
	{
		return false;
	}

	return true;
}

void Composites::BuildKdTree()
{
	mKdTree = new KDTree(3, *this);
	mKdTree->buildIndex();
}