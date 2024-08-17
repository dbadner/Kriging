#include "Blocks.hpp"
#include "Composites.hpp"
#include "KrigingParameters.hpp"
#include "KrigingEngine.hpp"

int main() 
{
   // Read in kriging parameters from file
   // TODO: file input TBC; dummy placeholder
   KrigingParameters parameters;

   // Validate input parameters
   // TODO: Add validation

   // Create blocks based on input range
   Blocks blocks(parameters.BlockModelInfo);

   // Read in composites, trimming based on block range and Domain value
   // TODO; TBC; dummy placeholder
   Composites composites;

   // Index composites using a KDTree
   composites.BuildKDTreeIndex();

   // Perform kriging
   KrigingEngine krigingEngine(parameters, composites, blocks);
   krigingEngine.RunKriging();

   // Output summary statistics
   // TODO: calculate and output summary statistics

   // Write blocks to file
   // TODO: TBC

   return 0;
}
