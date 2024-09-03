# Kriging Solution Overview
Proof of concept kriging engine for geostatistical analysis.
Solution is a work in progress and has several TODOs where implementation / refactoring is needed.

## Build Information
Solution was built on Windows in Visual Studio 2022 using the ISO C++20 Language Standard. 

## Solution Structure
The Kriging solution contains three projects:
1. KrigingApp: Console application for running the kriging program.
2. KrigingLib: Static library project containing kriging engines and supporting libraries. 
3. Tests: Unit tests for KrigingLib.

## To Run
The solution can be run from command line. The solution requires the following two arguments:
 1. KrigingParametersFile: Path to the JSON file containing kriging parameters.
 2. CompositesFile: Path to the CSV file containing composites data.
 
 Formats and examples for these input files are provided in the 'ExampleInputData' folder. 

 Example command to run: KrigingApp.exe ExKrigingParams.json ExComposites10k.csv

 Kriging can also be run via unit tests:
* KrigingEngineTests.cpp -> FullBlockModelKrigingTest test method can be used/modified to run the kriging engine on a full block model
* KrigingEngineTests.cpp -> OrdinaryKrigingOneBlock... test methods can be used/updated to run one block (point)

## Next steps
There are many simplifications in the current solution, as indicated by the many TODOs throughout. Some key next steps are as follows:
* Add proper kriged value verifications
* Support anisotropy, directional variogams with rotations, nested structures, etc.
* Add different types of kriging (currently only Ordinary Kriging is supported)
* Add block discretization (currently simplifies as point kriging at block centers)
* Further code optimization - KDTree improvements, data structures, build optimization, etc.
