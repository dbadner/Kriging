#pragma once

#include "Blocks.hpp"
#include "Composites.hpp"
#include "KrigingParameters.hpp"
#include "KrigingEngine.hpp"

/**
 * @brief Entry point for the console application. 
 */
int main(int argc, char* argv[])
{
   // Read in kriging parameters from file
   // TODO: file input TBC; dummy placeholder
   KrigingParameters parameters;

   // Validate input parameters
   // TODO: Add validation

   // Create blocks based on input range
   Blocks blocks(parameters.BlockModelInfo);

   // Read in composites
   // TODO; TBC; dummy placeholder
   std::string csvFilePath = {};
   Composites composites(csvFilePath, parameters.BlockModelInfo, parameters.MaxRadius);

   // Perform kriging
   KrigingEngine::RunKriging(blocks, parameters, composites);

   // Output summary statistics
   // TODO: calculate and output summary statistics

   // Write blocks to file
   // TODO: TBC

   return 0;
}
