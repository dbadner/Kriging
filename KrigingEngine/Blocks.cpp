#include "Blocks.hpp"

Blocks::Blocks(const BlockModelInfo& modelInfo)
{
	double deltaX = (modelInfo.MaxX - modelInfo.MinX) / modelInfo.BlockCountI;
	double deltaY = (modelInfo.MaxY - modelInfo.MinY) / modelInfo.BlockCountJ;
	double deltaZ = (modelInfo.MaxZ - modelInfo.MinZ) / modelInfo.BlockCountK;

	int numBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

	X.reserve(numBlocks);
	Y.reserve(numBlocks);
	Z.reserve(numBlocks);
	Grade.resize(numBlocks, NullValue); // Initialize grades as null

	for (int k = 0; k < modelInfo.BlockCountK; ++k)
	{
		double z = modelInfo.MinZ + k * deltaZ + deltaZ / 2.0;

		for (int j = 0; j < modelInfo.BlockCountJ; ++j)
		{
			double y = modelInfo.MinY + j * deltaY + deltaY / 2.0;

			for (int i = 0; i < modelInfo.BlockCountI; ++i)
			{
				double x = modelInfo.MinX + i * deltaX + deltaX / 2.0;

				X.emplace_back(x);
				Y.emplace_back(y);
				Z.emplace_back(z);
			}
		}
	}
}