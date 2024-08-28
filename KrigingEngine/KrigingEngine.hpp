#pragma once

#include <vector>
#include <future>
#include <cmath>
#include <iostream>

#include "include/Eigen/Dense"
#include "Blocks.hpp"
#include "Composites.hpp"
#include "Globals.hpp"
#include "KrigingParameters.hpp"

/**
* @brief Class containing variogram and kriging calculation methods.
*
* Reference for equations: Journel & Huijbregts 1978. Mining Geostatistics. 
*/
class KrigingEngine
{
public:

   /**
    * @brief Calculates the variogram value for a given pair of points.
    *
    * @param h Euclidean distance between pair of points in 3D space.
    * @param parameters Variogram parameters.
    * @return Variogram value.
    */
   static double Variogram(const double h, const VariogramParameters& parameters);

   /**
    * @brief Calculates the covariance for a given pair of points.
    *
    * @param h Euclidean distance between pair of points in 3D space.
    * @param parameters Variogram parameters.
    * @return Covariance value.
    */
   static double Covariance(const double h, const VariogramParameters& parameters);

   /**
    * @brief Performs ordinary kriging for a point p0 given nearest samples.
    *
    * @param x0,y0,z0 X,Y,Z value of unknown point p0 to be krigged.
    * @param xs,ys,zs X,Y,Z values of known sample points.
    * @param values Grade values of known sample points.
    * @param parameters Variogram parameters.
    * @return Krigged value at point p0.
    */
   static double OrdinaryKrigingPoint(const double x0, const double y0, const double z0,
      const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<double>& zs,
      const std::vector<double>& values, const VariogramParameters& parameters);

   /**
    * @brief Retrieves composites for the current block in preparation for kriging. 
    *
    * @param blockX,blockY,blockZ X,Y,Z centroid of block.
    * @param parameters Kriging parameters.
    * @param composites Composites.
    * @return Block krigged value.
    */
   static double KrigeOneBlock(const double blockX, const double blockY, const double blockZ,
      const KrigingParameters& parameters, const Composites& composites);

   /**
    * @brief Runs kriging for all provided blocks using parallelization.
    *
    * @param blocks Ref class containing list of block information.
    * @param parameters Ref class containing parameters for kriging.
    * @param composites Ref class containing composite information.
    * @return Block krigged value.
    */
   static void RunKriging(Blocks& blocks, const KrigingParameters& parameters, const Composites& composites);

private:

   /**
    * @brief Calculates the Euclidean distance between points p1 and p2.
    */
   static double EuclideanDistance(const double x1, const double y1, const double z1, 
      const double x2, const double y2, const double z2);

   /**
    * @brief Determines the number of threads to use based on the system processor information and number of blocks.
    *
    * Default 8 threads if system information is unknown. 
    */
   static size_t GetThreadBatchSize(const size_t numBlocks);
};