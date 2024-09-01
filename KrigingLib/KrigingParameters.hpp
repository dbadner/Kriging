#pragma once

#include "CoordinateExtents.hpp"

/**
 * @brief Parameters to define the variogram fit
 *
 * Simplifications: Single structure, isotropic, global variogram
 */
struct VariogramParameters
{
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
struct KrigingParameters
{
	enum KrigingType
	{
		Ordinary = 0
		// TODO: Support other types of kriging
	};

	KrigingType Type; // Type of kriging, not used yet
	int Domain; // Domain to interpolate, not used yet
	int MinNumComposites; // Minimum number of composites per block
	int MaxNumComposites; // Maximum number of composites per block
	double MaxRadius; // Maximum isotropic search radius
	VariogramParameters VariogramParameters; // Variogram parameters
	BlockModelInfo BlockModelInfo; // Block model definition
};
