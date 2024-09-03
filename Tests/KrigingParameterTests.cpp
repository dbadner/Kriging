#pragma once

#include "gtest/gtest.h"
#include "../KrigingLib/KrigingParameters.hpp"
#include "TestHelpers.hpp"

/**
 * @brief Unit tests for kriging parameter serialization
 */
namespace KrigingParameterTests
{
    TEST(SerializeAllParameters, SerializesCorrectly)
    {
		// Get JSON file path
		std::string filePath = TestHelpers::GetTestDataFilePath("ExKrigingParams.json");

		// Serialize kriging parameters from file
		// Test exception is not thrown with valid parameters
		KrigingParameters parameters;
		EXPECT_NO_THROW(parameters.SerializeParameters(filePath));

		// Spot check imported parameters
		EXPECT_EQ(parameters.Type, KrigingParameters::KrigingType::Ordinary);
		EXPECT_EQ(parameters.MinNumComposites, 5);
		EXPECT_EQ(parameters.VariogramParameters.Structure, VariogramParameters::StructureType::Spherical);
		EXPECT_EQ(parameters.BlockParameters.BlockCountJ, 100);
		EXPECT_DOUBLE_EQ(parameters.VariogramParameters.Range, 100);
		EXPECT_DOUBLE_EQ(parameters.MaxRadius, 150);
		EXPECT_DOUBLE_EQ(parameters.BlockParameters.BlockCoordExtents.MaxZ, 700);
    }

	TEST(SerializeRequiredParameters, MissingOptionalParametersSerializesCorrectly)
	{
		// Get JSON file path
		std::string filePath = TestHelpers::GetTestDataFilePath("ExKrigingParamsMissingOptional.json");

		// Serialize kriging parameters from file
		// Test exception is not thrown with valid parameters
		KrigingParameters parameters;
		EXPECT_NO_THROW(parameters.SerializeParameters(filePath));

		// Test optional properties were populated with defaults
		EXPECT_EQ(parameters.Type, KrigingParameters::KrigingType::Ordinary);
		EXPECT_EQ(parameters.MinNumComposites, 1);
		EXPECT_EQ(parameters.MaxNumComposites, 15);

		// Spot check imported parameters
		EXPECT_DOUBLE_EQ(parameters.MaxRadius, 200);
		EXPECT_DOUBLE_EQ(parameters.BlockParameters.BlockCoordExtents.MaxZ, 700);
	}

	TEST(TrySerializeBadParameters, InvalidVariogramStructureThrowsError)
	{
		// Get JSON file path
		std::string filePath = TestHelpers::GetTestDataFilePath("ExKrigingParamsBadStructureType.json");

		// Serialize kriging parameters from file
		// Test exception is thrown
		KrigingParameters parameters;
		EXPECT_THROW(parameters.SerializeParameters(filePath), std::invalid_argument);
	}

	TEST(TrySerializeBadParameters, InvalidMaxCompositeDistanceThrowsError)
	{
		// Get JSON file path
		std::string filePath = TestHelpers::GetTestDataFilePath("ExKrigingParamsInvalidMaxComp.json");

		// Serialize kriging parameters from file
		// Test exception is thrown
		KrigingParameters parameters;
		EXPECT_THROW(parameters.SerializeParameters(filePath), std::invalid_argument);
	}
}