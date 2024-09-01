#pragma once

#include <string>
#include <filesystem>

#include "../KrigingLib/Composites.hpp"

class TestHelpers
{
public:
	static std::string GetTestDataFilePath(const std::string fileName);
	static void SaveCompositesToCSV(Composites& composites, std::string fileName);
};