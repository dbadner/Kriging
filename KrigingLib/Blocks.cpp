#include "Blocks.hpp"

Blocks::Blocks(const BlockModelInfo& modelInfo)
{
	std::cout << "Generating blocks..." << std::endl;

	auto extents = modelInfo.BlockCoordExtents;

	double deltaX = (extents.MaxX - extents.MinX) / modelInfo.BlockCountI;
	double deltaY = (extents.MaxY - extents.MinY) / modelInfo.BlockCountJ;
	double deltaZ = (extents.MaxZ - extents.MinZ) / modelInfo.BlockCountK;

	int numBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

	X.reserve(numBlocks);
	Y.reserve(numBlocks);
	Z.reserve(numBlocks);
	Grade.resize(numBlocks, std::nullopt); // Initialize grade with null values

	for (int k = 0; k < modelInfo.BlockCountK; ++k)
	{
		double z = extents.MinZ + k * deltaZ + deltaZ / 2.0;

		for (int j = 0; j < modelInfo.BlockCountJ; ++j)
		{
			double y = extents.MinY + j * deltaY + deltaY / 2.0;

			for (int i = 0; i < modelInfo.BlockCountI; ++i)
			{
				double x = extents.MinX + i * deltaX + deltaX / 2.0;

				X.emplace_back(x);
				Y.emplace_back(y);
				Z.emplace_back(z);
			}
		}
	}
	std::cout << "Number of blocks created: " << X.size() << std::endl;
}

void Blocks::WriteToCSV(const std::string& filePath) const
{
	std::cout << "Writing results to file..." << std::endl;
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		LogAndThrow<std::runtime_error>("Cannot write to file: " + filePath);
	}

	file << "X,Y,Z,Grade\n";

	size_t numRows = GetSize();
	std::string grade;
	for (size_t i = 0; i < numRows; ++i)
	{
		if (Grade[i].has_value())
		{
			grade = std::to_string(Grade[i].value());
		}
		else
		{
			grade = "NULL";
		}
		file << X[i] << "," << Y[i] << "," << Z[i] << "," << grade << "\n";
	}

	file.close();
	std::cout << "Finished writing. Results are in file: " << filePath << std::endl;
}