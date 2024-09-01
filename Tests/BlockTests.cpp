#pragma once

#include "gtest/gtest.h"
#include "../KrigingEngine/Blocks.hpp"
#include "../KrigingEngine/Blocks.cpp"
#include "../KrigingEngine/KrigingParameters.hpp"

/**
 * @brief Unit tests for blocks class
 */
namespace BlockTests
{
   TEST(TestCreateBlocks, CreatesCorrectNumberOfComposites)
   {  
      CoordinateExtents modelExtents;
      modelExtents.MinX = 20;
      modelExtents.MinY = 20;
      modelExtents.MinZ = 15;
      modelExtents.MaxX = 100;
      modelExtents.MaxY = 100;
      modelExtents.MaxZ = 50;

      BlockModelInfo modelInfo;
      modelInfo.BlockCoordExtents = modelExtents;
      modelInfo.BlockCountI = 16;
      modelInfo.BlockCountJ = 16;
      modelInfo.BlockCountK = 14;

      auto expectedBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

      Blocks blocks(modelInfo);

      // Check that length of vectors is correct
      EXPECT_EQ(expectedBlocks, blocks.X.size());
      EXPECT_EQ(expectedBlocks, blocks.Y.size());
      EXPECT_EQ(expectedBlocks, blocks.Z.size());

      // Spot check first centroid
      double maxError = 0.0001;
      EXPECT_NEAR(22.5, blocks.X[0], maxError);
      EXPECT_NEAR(22.5, blocks.Y[0], maxError);
      EXPECT_NEAR(16.25, blocks.Z[0], maxError);
   }
}

