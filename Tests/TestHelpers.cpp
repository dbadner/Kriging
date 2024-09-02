#include "TestHelpers.hpp"

/**
* @brief Helper function to get the absolute file path to the test data folder
*/
std::string TestHelpers::GetTestDataFilePath(const std::string fileName)
{
	std::filesystem::path path = std::filesystem::current_path();

	path = path.parent_path().parent_path();

	path /= "Data";

	return (path / fileName).string();
}

/**
* @brief Helper function to dump composites to CSV file
*/
void TestHelpers::SaveCompositesToCSV(Composites& composites, std::string fileName)
{
	std::ofstream file(fileName);

	// Write the header
	file << "X,Y,Z,Grade\n";

	// Write the data
	for (size_t i = 0; i < composites.GetSize(); ++i) {
		file << std::fixed << std::setprecision(3)
			<< composites.GetX(i) << ','
			<< composites.GetY(i) << ','
			<< composites.GetZ(i) << ','
			<< composites.GetGrade(i) << '\n';
	}

	file.close();
}