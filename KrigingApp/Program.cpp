#pragma once

#include <filesystem>

#include "../KrigingLib/Blocks.hpp"
#include "../KrigingLib/Composites.hpp"
#include "../KrigingLib/KrigingParameters.hpp"
#include "../KrigingLib/KrigingEngine.hpp"

/**
 * @brief Entry point for the console application.
 * 
 * Two arguments are required:
 * 1. KrigingParametersFile: Path to the JSON file containing kriging parameters.
 * 2. CompositesFile: Path to the CSV file containing composites data.
 */
void main(int argc, char* argv[])
{
	// Confirm two arguments were provided in additional to the exe
	if (argc < 3)
	{
		LogAndThrow<std::invalid_argument>("Expected two parameters, but got " + std::to_string(argc - 1));
	}

	// Read in kriging parameters from file and validate
	KrigingParameters parameters;
	parameters.SerializeParameters(argv[1]);

	// Create blocks based on input parameters
	Blocks blocks(parameters.BlockParameters);

	// Read in composites filtered to interpolation area and validate
	Composites composites(argv[2], parameters.BlockParameters.BlockCoordExtents, parameters.MaxRadius);

	// Perform kriging
	KrigingEngine::RunKriging(blocks, parameters, composites);

	// Write blocks to CSV in EXE directory
	std::string outputFileName = "KrigingResults.csv";
	std::string outputFilePath = std::filesystem::current_path().string() + "\\" + outputFileName;
	blocks.WriteToCSV(outputFilePath);
}