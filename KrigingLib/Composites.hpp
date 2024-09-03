#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <iomanip>

#include "include/nanoflann.hpp"
#include "Blocks.hpp"
#include "CoordinateExtents.hpp"

/**
 * @brief Nearest composite result, comprising vectors of composite indices in order of increasing distance, and corresponding distances.
 */
struct NearestCompositesResult
{
	std::vector<size_t> Indices;
	std::vector<double> Distances;
};

/**
 * @brief Class containing composite / sample information.
 */
class Composites
{
public:
	/**
	 * @brief Reads in composites from csv file, filtering based on block extents and search radius.
	 * 
	 * First row in csv must contain column headers.
	 * Required columns: 'X', 'Y', 'Z', 'Grade'.
	 */
	Composites(const std::string csvFilePath, const CoordinateExtents blockExtents, const double maxSearchRadius);

	/**
	 * @brief Initializes composites by copying input vectors of x,y,z coordinates, and grades
	 *
	 * NOTE: Not memory efficient; recommended to use csv based import
	 */
	Composites(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& grades);

	/**
	 * @brief Dispose of Kd Tree.
	 */
	~Composites();

	/**
	 * @brief Get X value at composite index i
	 */
	const double& GetX(size_t& i) const { return X[i]; }

	/**
	 * @brief Get Y value at composite index i
	 */
	const double& GetY(size_t& i) const { return Y[i]; }

	/**
	 * @brief Get Z value at composite index i
	 */
	const double& GetZ(size_t& i) const { return Z[i]; }

	/**
	 * @brief Get grade value at composite index i
	 */
	const double& GetGrade(size_t& i) const { return Grade[i]; }

	/**
	 * @brief Get number of composites
	 */
	const size_t& GetSize() const { return X.size(); }

	/**
	 * @brief Finds the nearest n composites to the given coordinates, constrained by a maximum spherical search distance.
	 *
	 * @param x,y,z Coordinates of point from which to search
	 * @param n Number of composites
	 * @maxDist Maximum search radius from the search point
	 * @return Nearest composite result, comprising vectors of composite indices in order of increasing distance, and corresponding distances.
	 */
	NearestCompositesResult FindNearestComposites(const double x, const double y, const double z, const int n, const double maxDist) const;

	/**
	 * @brief Required methods below for nanoflann.
	 */
	size_t kdtree_get_point_count() const;

	double kdtree_distance(const double* p1, const size_t idxp2) const;

	double kdtree_get_pt(const size_t idx, const int dim) const;

	template <class BBOX>
	bool kdtree_get_bbox(BBOX&) const { return false; }

private:
	std::vector<double> X, Y, Z; // Composite/sample center locations; should not be modified after class initialization
	std::vector<double> Grade; // Composite grades; should not be modified after class initialization

	// Create a KD-tree of composite data
	using KDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, Composites>, Composites, 3>;
	KDTree* mKdTree;

	// List of required columns in the csv; not case sensitive
	const std::string mXColName = "x";
	const std::string mYColName = "y";
	const std::string mZColName = "z";
	const std::string mGradeColName = "grade";
	const std::vector<std::string> mRequiredColumns = {mXColName, mYColName, mZColName, mGradeColName};

	/**
	 * @brief Read in composite header and data from CSV, with validation.
	 * 
	 * Required columns defined in mRequiredCSVColumns.
	 * Function excludes data beyond the block extents plus search radius
	 * 
	 * @param filePath path of the CSV file
	 * @param blockExtents X,Y,Z coordinate extents of the blocks to be krigged
	 * @param maxSearchRadius maximum search radius for the krigging
	 */
	void ReadCompositesFromCSV(const std::string& filePath, const CoordinateExtents& blockExtents, const double maxSearchRadius);

	/**
	 * @brief Read in composite data from CSV.
	 */
	void ReadComposites(std::ifstream& file, std::unordered_map<std::string, size_t>& columnIndices, const CoordinateExtents& extents);

	/**
	 * @brief Checks if composite is relevant based on interpolation extents.
	 */
	static bool IsRelevantComposite(const double x, const double y, const double z, const double grade, const CoordinateExtents& extents);

	/**
	 * @brief Final data checks, then initialize Kd Tree. 
	 * 
	 * Note: Should only be called from the constructor.
	 */
	void FinishInitialization();

	// Build the KdTree index; should only be called by the constructor
	void BuildKdTree();
};
