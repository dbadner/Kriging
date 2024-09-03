#pragma once

#include <vector>

#include "KrigingParameters.hpp"

/**
 * @brief Class containing block model information.
 */
class Blocks
{
public:
	std::vector<std::optional<double>> Grade; // Block grades

	/**
	 * @brief Initializes block locations based on input model information.
	 * 
	 * NOTE: Assumes data have been previously validated. 
     * Refer to KrigingParameters class for validation.
	 */
	Blocks(const BlockModelInfo& modelInfo);

	/**
	 * @brief Get X value at composite index i
	 */
	double GetX(size_t i) const { return X[i]; }

	/**
	 * @brief Get Y value at composite index i
	 */
	double GetY(size_t i) const { return Y[i]; }

	/**
	 * @brief Get Z value at composite index i
	 */
	double GetZ(size_t i) const { return Z[i]; }

	/**
	 * @brief Get number of composites
	 */
	size_t GetSize() const { return X.size(); }

	/**
	 * @brief Writes blocks to CSV at the provided filepath
	 */
	void WriteToCSV(const std::string& filePath) const;

private:
	std::vector<double> X, Y, Z; // Block centroids; can only be set in the constructor
};

//TODO: Add domain and read in blocks from file for geology matching

//TODO: Refactor this depending on future block model file format and I/O TBC; for now storing blocks in memory
