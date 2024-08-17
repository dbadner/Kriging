#include "pch.h"
#include "../KrigingEngine/Blocks.hpp"
#include "../KrigingEngine/Blocks.cpp"
#include "../KrigingEngine/KrigingParameters.hpp"

namespace BlockTests
{
   TEST(TestCreateBlocks, CreatesCorrectNumberOfComposites)
   {
      BlockModelInfo modelInfo;
      modelInfo.MinX = 20;
      modelInfo.MinY = 20;
      modelInfo.MinZ = 15;
      modelInfo.MaxX = 100;
      modelInfo.MaxY = 100;
      modelInfo.MaxZ = 50;
      modelInfo.BlockCountI = 16;
      modelInfo.BlockCountJ = 16;
      modelInfo.BlockCountK = 14;

      auto expectedBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

      Blocks blocks(modelInfo);

      // Check that length of vectors is correct
      EXPECT_EQ(expectedBlocks, blocks.I.size());
      EXPECT_EQ(expectedBlocks, blocks.J.size());
      EXPECT_EQ(expectedBlocks, blocks.K.size());
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

