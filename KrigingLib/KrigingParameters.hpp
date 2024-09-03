#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#include "CoordinateExtents.hpp"
#include "include\json.hpp"
#include "Helpers.hpp"


/**
 * @brief Parameters to define the variogram fit
 *
 * Simplifications: Single structure, isotropic, global variogram
 */
class VariogramParameters
{
public:
	enum StructureType
	{
		Spherical = 0,
		Exponential = 1,
		Gaussian = 2
	};

	double Nugget;
	double Sill;
	double Range;
	StructureType Structure;

	/**
	 * @brief Returns StructureType corresponding to input string
	 */
	static StructureType StringToStructureType(std::string structure);
};

/**
 * @brief Parameters to define a regular block model
 *
 * Simplifications: No sub-blocks, no rotation
 */
struct BlockModelInfo
{
	CoordinateExtents BlockCoordExtents;
	int BlockCountI, BlockCountJ, BlockCountK;
};

/**
 * @brief Parameters required to run the kriging engine
 *
 * Simplifications: No quadrant/octant search, isotropic, one type of kriging, and much more.
 */
class KrigingParameters
{
public:
	enum KrigingType
	{
		Ordinary = 0 // Default
		// TODO: Support other types of kriging
	};

	// Optional properties
	KrigingType Type; // Type of kriging, not used yet, default ordinary kriging
	int MinNumComposites; // Minimum number of composites per block, default 1
	int MaxNumComposites; // Maximum number of composites per block, default 15

	//Required properties
	double MaxRadius; // Maximum isotropic search radius, default unlimited
	VariogramParameters VariogramParameters; // Variogram parameters
	BlockModelInfo BlockParameters; // Block model definition

	/**
	 * @brief Serializes input json parameters to class fields
	 */
	void SerializeParameters(const std::string& filePath);

private:
	// Optional property defaults
	const KrigingType mDefaultType = KrigingType::Ordinary;
	const int mDefaultMinNumComposites = 1;
	const int mDefaultMaxNumComposites = 15;

	//Minimum non-zero double value for validation
	const double mDoubleValMin = 0.00001;

	/**
	 * @brief Validate parameters stored in class fields.
	 */
	void ValidateParameters();

	/**
	 * @brief Validate kriging parameters stored in class fields.
	 */
	void ValidateKrigingParameters();

	/**
	 * @brief Validate variogram parameters stored in class fields.
	 */
	void ValidateVariogramParameters();

	/**
	 * @brief Validate block parameters stored in class fields.
	 */
	void ValidateBlockParameters();

	/**
	 * @brief Returns KrigingType corresponding to input string
	 */
	static KrigingType StringToKrigingType(std::string string);
};
