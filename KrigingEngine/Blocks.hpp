#pragma once

#include <vector>

#include "KrigingParameters.hpp"
#include "Globals.hpp"

/**
 * @brief Class containing block model information.
 */
class Blocks
{
public:
   std::vector<double> X, Y, Z;  // Block centroids
   std::vector<double> Grade;    // Block grades

   /**
    * @brief Initializes block locations based on input model information.
    */
   Blocks(const BlockModelInfo& modelInfo);
};

//TODO: Add domain and read in blocks from file for geology matching

//TODO: Refactor this depending on future block model file format and I/O TBC; for now storing blocks inefficiently in memory for concept