#include "pch.h"
#include <chrono> 
#include <random>

#include "../KrigingEngine/KrigingEngine.hpp"
#include "../KrigingEngine/KrigingEngine.cpp"
#include "../KrigingEngine/KrigingParameters.hpp"

namespace KrigingEngineTests
{
#pragma region VariogramTests
   class VariogramTests : public ::testing::Test {
   protected:
      VariogramParameters parameters;
      const double maxError = 0.0001;

      void SetUp() override {
         // Initialize VariogramParameters
         parameters.Range = 200.0;
         parameters.Sill = 1.0;
         parameters.Nugget = 0.1;
         parameters.Structure = VariogramParameters::StructureType::Spherical;
      }

      void TearDown() override {

      }
   };
   TEST_F(VariogramTests, SphericalBasicResultValid) {

      double h = 201;
      double var = KrigingEngine::Variogram(h, parameters);
      double c = KrigingEngine::Covariance(h, parameters);

      // Test variogram value
      EXPECT_NEAR(parameters.Sill, var, maxError);
      // Test covariance
      EXPECT_NEAR(0, c, maxError);
   }

   TEST_F(VariogramTests, ExponentialBasicResultValid) {

      double h = 201;

      auto localParameters = parameters;
      localParameters.Structure = VariogramParameters::StructureType::Exponential;

      double var = KrigingEngine::Variogram(h, localParameters);
      double c = KrigingEngine::Covariance(h, localParameters);

      // Test variogram value at range - should be close to the sill
      // TODO: this is an approximate check only; needs robust testing
      EXPECT_NEAR(localParameters.Sill, var, 0.1);
      // Test covariance
      EXPECT_NEAR(0, c, 0.1);
   }

   TEST_F(VariogramTests, GaussianBasicResultValid) {

      double h = 201;

      auto localParameters = parameters;
      localParameters.Structure = VariogramParameters::StructureType::Gaussian;

      double var = KrigingEngine::Variogram(h, localParameters);
      double c = KrigingEngine::Covariance(h, localParameters);

      // Test variogram value at range - should be close to the sill
      // TODO: this is an approximate check only; needs robust testing
      EXPECT_NEAR(localParameters.Sill, var, 0.1);
      // Test covariance
      EXPECT_NEAR(0, c, 0.1);
   }

   TEST_F(VariogramTests, LagZeroReturnsNuggetValue) {

      double h = 0;
      double var = KrigingEngine::Variogram(h, parameters);
      double c = KrigingEngine::Covariance(h, parameters);

      // Test variogram value
      EXPECT_NEAR(parameters.Nugget, var, maxError);
      // Test covariance
      EXPECT_NEAR(parameters.Sill - parameters.Nugget, c, maxError);
   }

   // TODO: Add more unit tests to check calculated values on the actual variogram fit curves with the different structure types

#pragma endregion VariogramTests

#pragma region KrigingTests

   class KrigingTests : public ::testing::Test {
   protected:
      VariogramParameters mParameters;
      const double mMaxError = 1e-5;

      void SetUp() override {
         // Initialize VariogramParameters
         mParameters.Range = 5.0;
         mParameters.Sill = 1.0;
         mParameters.Nugget = 0.1;
         mParameters.Structure = VariogramParameters::StructureType::Spherical;
      }

      void TearDown() override {

      }
   };

   TEST_F(KrigingTests, OrdinaryKrigingConstGradeTest)
   {
      // Define the point to be estimated
      double x0 = 2.5;
      double y0 = 2.5;
      double z0 = 2.5;

      // Define a single grade for all composites
      const double grade = 0.12;

      // Define the coordinates and values of the composites
      std::vector<double> xs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> ys = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> zs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> grades(5, grade);

      // Perform ordinary kriging
      double okResult = KrigingEngine::OrdinaryKrigingPoint(x0, y0, z0, xs, ys, zs, grades, mParameters);

      ASSERT_NEAR(grade, okResult, mMaxError);
   }

   TEST_F(KrigingTests, OrdinaryKrigingOneBlockOneSampleTest)
   {

      // Define the point to be estimated
      double x0 = 2.5;
      double y0 = 2.5;
      double z0 = 2.5;

      // Define the coordinates and values of the composites
      std::vector<double> xs = { 2.0 };
      std::vector<double> ys = { 0.0 };
      std::vector<double> zs = { 1.0 };
      std::vector<double> grades = { 0.12 };

      // Perform ordinary kriging
      double okResult = KrigingEngine::OrdinaryKrigingPoint(x0, y0, z0, xs, ys, zs, grades, mParameters);

      ASSERT_NEAR(grades[0], okResult, mMaxError);
   }

   TEST_F(KrigingTests, OrdinaryKrigingOneBlockResultsAppearReasonable)
   {
      // TODO: Add more robust tests to prove the kriging results are correct; this test is just an approximate check

      // Define the coordinates and values of the composites
      std::vector<double> xs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> ys = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> zs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> grades = { 0.10, 0.12, 0.82, 0.75, 0.21 };

      // Define the point to be estimated
      double x0 = 2.5;
      double y0 = 2.5;
      double z0 = 2.5;

      // Perform ordinary kriging
      double okResult = KrigingEngine::OrdinaryKrigingPoint(x0, y0, z0, xs, ys, zs, grades, mParameters);

      // Result should be near ~0.8 since these samples are closest to the estimation point
      ASSERT_NEAR(0.8, okResult, 0.1);

      // Define second point to be estimated
      // Define the point to be estimated
      double x1 = 1.5;
      double y1 = 1.5;
      double z1 = 1.5;

      // Perform ordinary kriging
      double okResult2 = KrigingEngine::OrdinaryKrigingPoint(x1, y1, z1, xs, ys, zs, grades, mParameters);

      // Result should be somewhere in between the range given position of the estimation point
      ASSERT_NEAR(0.4, okResult2, 0.15);
   }

   TEST_F(KrigingTests, FullBlockModelKrigingTest)
   {
      // Input parameters
      int numComposites = 1000000;

      BlockModelInfo modelInfo;
      modelInfo.MinX = 0;
      modelInfo.MinY = 0;
      modelInfo.MinZ = 0;
      modelInfo.MaxX = 100;
      modelInfo.MaxY = 100;
      modelInfo.MaxZ = 100;
      modelInfo.BlockCountI = 100;
      modelInfo.BlockCountJ = 100;
      modelInfo.BlockCountK = 100;

      VariogramParameters varParameters;
      varParameters.Range = 50.0;
      varParameters.Sill = 1.0;
      varParameters.Nugget = 0.1;
      varParameters.Structure = VariogramParameters::StructureType::Spherical;

      KrigingParameters parameters;
      parameters.Type = KrigingParameters::KrigingType::Ordinary;
      parameters.MinNumComposites = 3;
      parameters.MaxNumComposites = 10;
      parameters.MaxRadius = 70;
      parameters.VariogramParameters = varParameters;
      parameters.BlockModelInfo = modelInfo;

      // Create Composites
      Composites composites;

      // Randomly generate composites
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<double> dist(0.0, 1.0);

      for (int i = 0; i < numComposites; i++)
      {
         composites.X.push_back(dist(gen) * 100.0);
         composites.Y.push_back(dist(gen) * 100.0);
         composites.Z.push_back(dist(gen) * 100.0);
         composites.Grade.push_back(dist(gen));
      }

      // Create KD Tree
      composites.BuildKDTreeIndex();

      // Start measuring time
      auto startTime = std::chrono::high_resolution_clock::now();

      // Create Blocks
      Blocks blocks(modelInfo);

      // Create KrigingEngine instance
      KrigingEngine krigingEngine(parameters, composites, blocks);

      // Perform kriging on the set of blocks
      krigingEngine.RunKriging();

      // Stop measuring time
      auto endTime = std::chrono::high_resolution_clock::now();

      // Calculate the duration in milliseconds
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

      // Print out the time taken
      std::cout << "Number of blocks: " << blocks.X.size() << std::endl;
      std::cout << "Number of composites: " << composites.X.size() << std::endl;
      std::cout << "Time taken: " << duration << " milliseconds" << std::endl;

      auto x = blocks.Grade[0];

      // Print out the first 5 block I, J, K, and grade values
      for (int i = 0; i < 5; i++)
      {
         std::cout << "Block: " << i << ", Grade: " << blocks.Grade[i] << std::endl;
      }
   }

   TEST_F(KrigingTests, PerformanceTest)
   {
      // Define the coordinates and values of the composites
      std::vector<double> xs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> ys = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> zs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> grades = { 0.10, 0.12, 0.82, 0.75, 0.21 };

      // Define the estimation points
      std::vector<double> estimationPointsX(10000, 2.5);
      std::vector<double> estimationPointsY(10000, 2.5);
      std::vector<double> estimationPointsZ(10000, 2.5);

      // Start measuring time
      auto startTime = std::chrono::high_resolution_clock::now();

      // Perform ordinary kriging for each estimation point
      for (int i = 0; i < 10000; i++)
      {
         double result = KrigingEngine::OrdinaryKrigingPoint(
            estimationPointsX[i], estimationPointsY[i], estimationPointsZ[i], xs, ys, zs, grades, mParameters);
      }

      // Stop measuring time
      auto endTime = std::chrono::high_resolution_clock::now();

      // Calculate the duration in milliseconds
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

      // Print out the time taken
      std::cout << "Time taken for 10,000 estimations: " << duration << " milliseconds" << std::endl;
   }
#pragma endregion KrigingTests   

   int main(int argc, char** argv) {
      ::testing::InitGoogleTest(&argc, argv);
      return RUN_ALL_TESTS();
   }
}