#include "KrigingParameters.hpp"

VariogramParameters::StructureType VariogramParameters::StringToStructureType(std::string structure)
{
	// Transform to lower case
	std::transform(structure.begin(), structure.end(), structure.begin(), tolower);
	if (structure == "spherical")
	{
		return StructureType::Spherical;
	}
	else if (structure == "exponential")
	{
		return StructureType::Exponential;
	}
	else if (structure == "gaussian")
	{
		return StructureType::Gaussian;
	}
	else
	{
		LogAndThrow<std::invalid_argument>("Invalid structure type: " + structure);
	}
}

void KrigingParameters::SerializeParameters(const std::string& filePath)
{
	std::cout << "Reading kriging parameters from file: " + filePath << std::endl;

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LogAndThrow<std::runtime_error>("File does not exist or cannot be opened: " + filePath);
	}

	nlohmann::json j;
	try
	{
		file >> j;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		LogAndThrow<std::runtime_error>("JSON parsing error. Ensure input file follows the correct JSON format. File: " + filePath);
	}

	try
	{
		// Serialize optional parameters
		if (j.contains("Type"))
		{
			Type = StringToKrigingType(j.at("Type").get<std::string>());
		}
		else
		{
			Type = mDefaultType;
			std::cout << "Warning: Parameter 'Type' not found in JSON. Using Ordinary Kriging." << mDefaultType << std::endl;
		}

		if (j.contains("MinNumComposites"))
		{
			MinNumComposites = j.at("MinNumComposites").get<int>();
		}
		else
		{
			MinNumComposites = mDefaultMinNumComposites;
			std::cout << "Warning: Parameter 'MinNumComposites' not found in JSON. Using default: " << mDefaultMinNumComposites << std::endl;
		}

		if (j.contains("MaxNumComposites"))
		{
			MaxNumComposites = j.at("MaxNumComposites").get<int>();
		}
		else
		{
			MaxNumComposites = mDefaultMaxNumComposites;
			std::cout << "Warning: Parameter 'MaxNumComposites' not found in JSON. Using default: " << mDefaultMaxNumComposites << std::endl;
		}

		// Serialize required parameters
		MaxRadius = j.at("MaxRadius").get<double>();

		auto& varParams = j.at("VariogramParameters");
		VariogramParameters.Nugget = varParams.at("Nugget").get<double>();
		VariogramParameters.Sill = varParams.at("Sill").get<double>();
		VariogramParameters.Range = varParams.at("Range").get<double>();
		VariogramParameters.Structure = VariogramParameters.StringToStructureType(varParams.at("StructureType").get<std::string>());

		auto& blockInfo = j.at("BlockModelInfo");
		auto& coordExtents = blockInfo.at("CoordinateExtents");
		BlockParameters.BlockCoordExtents.MinX = coordExtents.at("MinX").get<double>();
		BlockParameters.BlockCoordExtents.MinY = coordExtents.at("MinY").get<double>();
		BlockParameters.BlockCoordExtents.MinZ = coordExtents.at("MinZ").get<double>();
		BlockParameters.BlockCoordExtents.MaxX = coordExtents.at("MaxX").get<double>();
		BlockParameters.BlockCoordExtents.MaxY = coordExtents.at("MaxY").get<double>();
		BlockParameters.BlockCoordExtents.MaxZ = coordExtents.at("MaxZ").get<double>();
		BlockParameters.BlockCountI = blockInfo.at("BlockCountI").get<int>();
		BlockParameters.BlockCountJ = blockInfo.at("BlockCountJ").get<int>();
		BlockParameters.BlockCountK = blockInfo.at("BlockCountK").get<int>();
	}
	catch (const nlohmann::json::exception& e)
	{
		LogAndThrow<std::runtime_error>("Parameter serialization error: " + std::string(e.what()));
	}

	ValidateParameters();
	std::cout << "Kriging parameters successfully read." << std::endl;
}

void KrigingParameters::ValidateParameters()
{
	ValidateKrigingParameters();
	ValidateVariogramParameters();
	ValidateBlockParameters();
}

void KrigingParameters::ValidateKrigingParameters()
{
	if (MinNumComposites > MaxNumComposites)
	{
		LogAndThrow<std::invalid_argument>("Minimum number of composites cannot be greater than maximum number of composites.");
	}
	if (MaxNumComposites < 1)
	{
		LogAndThrow<std::invalid_argument>("Maximum number of composites must be greater than zero.");
	}
	if (MaxRadius <= 0)
	{
		LogAndThrow<std::invalid_argument>("Maximum radius must be greater than zero.");
	}
}

void KrigingParameters::ValidateVariogramParameters()
{
	if (VariogramParameters.Sill <= 0)
	{
		LogAndThrow<std::invalid_argument>("Variogram sill must be greater than zero.");
	}
	if (VariogramParameters.Nugget < 0 || VariogramParameters.Nugget > VariogramParameters.Sill)
	{
		LogAndThrow<std::invalid_argument>("Variogram nugget must be between zero and the sill.");
	}
	if (VariogramParameters.Range < mDoubleValMin)
	{
		LogAndThrow<std::invalid_argument>("Variogram range must be greater than zero.");
	}
}

void KrigingParameters::ValidateBlockParameters()
{
	auto& coordExtents = BlockParameters.BlockCoordExtents;

	if (BlockParameters.BlockCountI < 1 || BlockParameters.BlockCountJ < 1 || BlockParameters.BlockCountK < 1)
	{
		LogAndThrow<std::invalid_argument>("Block count must be at least one.");
	}
	if ((coordExtents.MaxX - coordExtents.MinX) / BlockParameters.BlockCountI < mDoubleValMin)
	{
		LogAndThrow<std::invalid_argument>("Invalid block X size.");
	}
	if ((coordExtents.MaxY - coordExtents.MinY) / BlockParameters.BlockCountJ < mDoubleValMin)
	{
		LogAndThrow<std::invalid_argument>("Invalid block Y size.");
	}
	if ((coordExtents.MaxZ - coordExtents.MinZ) / BlockParameters.BlockCountK < mDoubleValMin)
	{
		LogAndThrow<std::invalid_argument>("Invalid block Z size.");
	}
}

KrigingParameters::KrigingType KrigingParameters::StringToKrigingType(std::string string)
{
	// Transform to lower case
	std::transform(string.begin(), string.end(), string.begin(), tolower);
	if (string == "ordinary")
	{
		return KrigingType::Ordinary;
	}
	else
	{
		LogAndThrow<std::invalid_argument>("Unknown kriging type: " + string);
	}
	// TODO: Add more kriging types once implemented
}