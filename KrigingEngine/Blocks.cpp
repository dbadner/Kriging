#include "Blocks.hpp"

Blocks::Blocks(const BlockModelInfo& modelInfo)
{
	double deltaX = (modelInfo.MaxX - modelInfo.MinX) / modelInfo.BlockCountI;
	double deltaY = (modelInfo.MaxY - modelInfo.MinY) / modelInfo.BlockCountJ;
	double deltaZ = (modelInfo.MaxZ - modelInfo.MinZ) / modelInfo.BlockCountK;

	int numBlocks = modelInfo.BlockCountI * modelInfo.BlockCountJ * modelInfo.BlockCountK;

	I.reserve(numBlocks);
	J.reserve(numBlocks);
	K.reserve(numBlocks);
	X.reserve(numBlocks);
	Y.reserve(numBlocks);
	Z.reserve(numBlocks);
	Grade.reserve(numBlocks);

	for (int k = 0; k < modelInfo.BlockCountK; ++k)
	{
		double z = modelInfo.MinZ + k * deltaZ + deltaZ / 2.0;

		for (int j = 0; j < modelInfo.BlockCountJ; ++j)
		{
			double y = modelInfo.MinY + j * deltaY + deltaY / 2.0;

			for (int i = 0; i < modelInfo.BlockCountI; ++i)
			{
				double x = modelInfo.MinX + i * deltaX + deltaX / 2.0;

				this->I.push_back(i);
				this->J.push_back(j);
				this->K.push_back(k);
				this->X.push_back(x);
				this->Y.push_back(y);
				this->Z.push_back(z);
				this->Grade.push_back(NullValue); // Initialize Grades as null
			}
		}
	}
}