#include <string>
#include <stdexcept>
#include <iostream>

template <typename ExceptionType>
static void LogAndThrow(const std::string& errorMessage)
{
    std::cerr << "Error: " << errorMessage << std::endl;

    throw ExceptionType(errorMessage);
}