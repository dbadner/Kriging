#pragma once

#include "gtest/gtest.h"
#include "../KrigingLib/Blocks.hpp"
#include "../KrigingLib/KrigingParameters.hpp"

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
      EXPECT_EQ(expectedBlocks, blocks.GetSize());

      // Spot check first centroid
      double maxError = 0.0001;
      size_t index = 0;
      EXPECT_NEAR(22.5, blocks.GetX(index), maxError);
      EXPECT_NEAR(22.5, blocks.GetY(index), maxError);
      EXPECT_NEAR(16.25, blocks.GetZ(index), maxError);
   }
}

