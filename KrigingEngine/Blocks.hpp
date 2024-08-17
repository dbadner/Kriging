#pragma once
#include <vector>
#include "KrigingParameters.hpp"
#include "Globals.hpp"

// Class to store list of block locations and attributes as vectors
class Blocks
{
public:
   std::vector<double> I, J, K; // Block indices
   std::vector<double> X, Y, Z; // Block centroids
   std::vector<double> Grade;

   Blocks(const BlockModelInfo& modelInfo);
};

//TODO: Add domain and read in blocks from file for geology matching

//TODO: Consider performance impact of changing to full OOP and defining a 'Block' class (and 'Composite' class)