#pragma once

#include <chrono>
#include <iostream>
#include <vector>
#include <filesystem>

//TODO: Update solution structure so cpp references are not needed in test project
#include "gtest/gtest.h"
#include "../KrigingEngine/Composites.hpp"
#include "../KrigingEngine/Composites.cpp"

/**
 * @brief Unit tests for composite class
 */
namespace CompositeTests
{
	//TODO: Create a test fixture and initialize shared objects

	TEST(FindNearestCompositesTest, ReturnsCorrectCompositeIndices)
	{
		std::vector<double> xs = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> ys = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> zs = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> grades = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		Composites composites(xs, ys, zs, grades);

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
		std::vector<double> xs = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> ys = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> zs = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		std::vector<double> grades = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		Composites composites(xs, ys, zs, grades);

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

	static std::vector<double> FindNearestCompositesNaive(const Composites& composites, double x, double y, double z,
		int numComposites)
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

	TEST(PerformanceTest, KDTreeFasterThanNaive)
	{
		int numComposites = 10000;
		int numBlocks = 100;
		int numClosestComp = 3;
		double maxSearchRadius = 10000;

		// Generate random composite data between 0-100
		std::vector<double> randomData;
		for (int i = 0; i < numComposites; i++)
		{
			double randomValue = rand() % 101;
			randomData.push_back(randomValue);
		}
		Composites composites(randomData, randomData, randomData, randomData);

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
			std::vector<double> result = FindNearestCompositesNaive(composites, inputPointsX[i], inputPointsY[i],
				inputPointsZ[i], numClosestComp);
		}
		end = std::chrono::high_resolution_clock::now();
		auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "FindNearestCompositesNaive method performance: " << duration2 << " milliseconds" << std::endl;

		// Assert that duration1 is less than duration2
		ASSERT_LT(duration1, duration2);
	}

	static void InitCoordExtents(CoordinateExtents& modelExtents)
	{
		modelExtents.MinX = 20;
		modelExtents.MinY = 20;
		modelExtents.MinZ = 15;
		modelExtents.MaxX = 100;
		modelExtents.MaxY = 100;
		modelExtents.MaxZ = 50;
	}

	TEST(ImportCompositesTest, ImportsCorerctNumberOfComposites)
	{
		// CSV file path
		// TODO: Fix to support non-hardcoded file path
		std::string filePath = "C:\\repos\\Kriging\\Tests\\Data\\ExComposites10.csv";

		// Create block extents for kriging
		CoordinateExtents modelExtents;
		InitCoordExtents(modelExtents);

		// Search radius for kriging
		double searchRadius = 100;

		// Initialize and import composites
		Composites composites(filePath, modelExtents, searchRadius);

		// Test number of imported composites is correct
		EXPECT_EQ(10, composites.X.size());
	}

	TEST(ImportCompositesTest, SkipsInvalidComposites)
	{
		// CSV file path
		// TODO: Fix to support non-hardcoded file path
		std::string filePath = "C:\\repos\\Kriging\\Tests\\Data\\ExComposites10BadData.csv";

		// Create block extents for kriging
		CoordinateExtents modelExtents;
		InitCoordExtents(modelExtents);

		// Search radius for kriging
		double searchRadius = 100;

		// Initialize and import composites
		Composites composites(filePath, modelExtents, searchRadius);

		// Test number of imported composites is correct
		EXPECT_EQ(7, composites.X.size());
	}
}
