#include "pch.h"
#include "../KrigingEngine/Composites.hpp"
#include "../KrigingEngine/Composites.cpp"
//TODO: Update solution structure so cpp references are not needed in test project
#include <chrono>
#include <iostream>
#include <vector>

namespace CompositeTests
{
   //TODO: Create a test fixture and initialize shared objects

   static void CreateTestComposites(Composites& composites)
   {
      composites.X = { 1.0, 2.0, 3.0, 4.0, 5.0 };
      composites.Y = { 1.0, 2.0, 3.0, 4.0, 5.0 };
      composites.Z = { 1.0, 2.0, 3.0, 4.0, 5.0 };
      composites.Domain = { 1.0, 2.0, 3.0, 4.0, 5.0 };
      composites.Grade = { 1.0, 2.0, 3.0, 4.0, 5.0 };

      composites.BuildKDTreeIndex();
   }
   TEST(FindNearestCompositesTest, ReturnsCorrectCompositeIndices)
   {
      Composites composites;
      CreateTestComposites(composites);

      double x = 2.432;
      double y = 2.972;
      double z = 3.152;
      int numComposites = 3;
      double maxSearchRadius = 10000;

      NearestCompositesResult result = composites.FindNearestComposites(x, y, z, numComposites, maxSearchRadius);


      // Test size of result is correct
      EXPECT_EQ(3.0, result.Indices.size());
      EXPECT_EQ(3.0, result.Distances.size());

      // Test sequence of nearest neighbors is correct
      EXPECT_EQ(2.0, result.Indices[0]);
      EXPECT_EQ(1.0, result.Indices[1]);
      EXPECT_EQ(3.0, result.Indices[2]);
   }

   TEST(FindNearestCompositesWithMaxDistanceTest, ReturnsCorrectNumberOfComposites)
   {
      Composites composites;
      CreateTestComposites(composites);

      double x = 2.5;
      double y = 2.5;
      double z = 2.5;
      int numComposites = 3;
      double maxSearchRadius = 1.0;

      NearestCompositesResult result = composites.FindNearestComposites(x, y, z, numComposites, maxSearchRadius);

      // Test size of result is correct
      EXPECT_EQ(2.0, result.Indices.size());
      EXPECT_EQ(2.0, result.Distances.size());
   }

   static std::vector<double> FindNearestCompositesNaive(const Composites& composites, double x, double y, double z, int numComposites)
   {
      std::vector<double> nearestComposites;
      for (int j = 0; j < composites.X.size(); j++)
      {
         double distanceSq = pow(x - composites.X[j], 2) + pow(y - composites.Y[j], 2) + pow(z - composites.Z[j], 2);
         nearestComposites.push_back(distanceSq);
      }
      std::sort(nearestComposites.begin(), nearestComposites.end());
      nearestComposites.resize(numComposites);

      return nearestComposites;
   }

   //TODO: Define flag for explicit test cases
   //#ifdef EXPLICIT_TESTS_ENABLED
   TEST(PerformanceTest, KDTreeFasterThanNaive)
   {
      //TODO: Pass multiple pairs of Composites and Blocks into test method for performance testing
      int numComposites = 10000;
      int numBlocks = 100;
      int numClosestComp = 3;
      double maxSearchRadius = 10000;

      // Generate random composite data between 0-100
      Composites composites;
      std::vector<double> randomData;
      for (int i = 0; i < numComposites; i++)
      {
         double randomValue = rand() % 101;
         randomData.push_back(randomValue);
      }
      composites.X = randomData;
      composites.Y = randomData;
      composites.Z = randomData;
      composites.Domain = randomData;
      composites.Grade = randomData;

      // Build the Kd Tree
      composites.BuildKDTreeIndex();

      std::vector<double> inputPointsX(numBlocks);
      std::vector<double> inputPointsY(numBlocks);
      std::vector<double> inputPointsZ(numBlocks);

      // Generate random input points
      for (int i = 0; i < numBlocks; i++)
      {
         double randomX = rand() % 101;
         double randomY = rand() % 101;
         double randomZ = rand() % 101;
         inputPointsX[i] = randomX;
         inputPointsY[i] = randomY;
         inputPointsZ[i] = randomZ;
      }

      // Measure performance of KdTree method
      auto start = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < numBlocks; i++)
      {
         NearestCompositesResult result = composites.FindNearestComposites(
            inputPointsX[i], inputPointsY[i], inputPointsZ[i], numClosestComp, maxSearchRadius);
      }
      auto end = std::chrono::high_resolution_clock::now();
      auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      std::cout << "FindNearestComposites method performance: " << duration1 << " milliseconds" << std::endl;

      // Measure performance of naive solution
      start = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < numBlocks; i++)
      {
         std::vector<double> result = FindNearestCompositesNaive(composites, inputPointsX[i], inputPointsY[i], inputPointsZ[i], numClosestComp);
      }
      end = std::chrono::high_resolution_clock::now();
      auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      std::cout << "FindNearestCompositesNaive method performance: " << duration2 << " milliseconds" << std::endl;

      // Assert that duration1 is less than duration2
      ASSERT_LT(duration1, duration2);
   }
   //#endif
}
