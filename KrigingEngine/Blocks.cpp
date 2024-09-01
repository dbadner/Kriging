#include "Blocks.hpp"

Blocks::Blocks(const BlockModelInfo& modelInfo)
{
	auto extents = modelInfo.BlockCoordExtents;

	double deltaX = (extents.MaxX - extents.MinX) / modelInfo.BlockCountI;
	double deltaY = (extents.MaxY - extents.MinY) / modelInfo.BlockCountJ;
	double deltaZ = (extents.MaxZ - extents.MinZ) / modelInfo.BlockCountK;

	int numBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

	X.reserve(numBlocks);
	Y.reserve(numBlocks);
	Z.reserve(numBlocks);
	Grade.resize(numBlocks, NullValue); // TODO: replace this with null handling, std::optional

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
}