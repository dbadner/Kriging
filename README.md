# Kriging Solution Overview
Proof of concept kriging engine for geostatistical analysis.
Solution is a work in progress and has several TODOs where implementation / refactoring is needed.

## Build Information
Solution was built on Windows in Visual Studio 2022 as a C++ console application. 

## Solution Structure
Program.cpp contains the main method and entry point to the solution. 
Currently, this main method is just a skeleton with some placeholders, and thus kriging can not be run usefully from the command line.
The core kriging engines are in KrigingEngine.cpp. 

## To Run
Currently to test the kriging engines, the Test project can be used. 
* KrigingEngineTests.cpp -> FullBlockModelKrigingTest method can be used/modified to run the engine on a full block model
* KrigingEngineTests.cpp -> OrdinaryKrigingOneBlock test methods can be used/updated to run one block (point)

## Next steps
There are many simplifications in the current POC solution, as indicated by the many TODOs throughout the solution. Some key next steps are as follows:
* Support file input and output
* Add input data validation with unit testing
* Add proper CLI support with input args and summary output
* Add proper kriged value verifications
* Support anisotropy, directional variogams with rotations, nested structures, etc.
* Add different types of kriging (currently only Ordinary Kriging is supported)
* Add block discretization (currently simplifies as point kriging at block centers)
* Parallelization - confirm thread safety
* Further code optimization - KDTree improvements, data structures, build optimization, etc.
