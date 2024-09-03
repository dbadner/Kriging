#pragma once

#include "../KrigingLib/Blocks.hpp"
#include "../KrigingLib/Composites.hpp"
#include "../KrigingLib/KrigingParameters.hpp"
#include "../KrigingLib/KrigingEngine.hpp"

/**
 * @brief Entry point for the console application.
 */
int main(int argc, char* argv[])
{
	// Read in kriging parameters from file and validate
	KrigingParameters parameters;
	parameters.SerializeParameters(argv[0]);

	// Create blocks based on input parameters
	Blocks blocks(parameters.BlockParameters);

	// Read in composites filtered to interpolation area and validate
	Composites composites(argv[1], parameters.BlockParameters.BlockCoordExtents, parameters.MaxRadius);

	// Perform kriging
	KrigingEngine::RunKriging(blocks, parameters, composites);

	// Write blocks to file
	// TODO: TBC

	return 0;
}
