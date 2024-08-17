#pragma once

#include <vector>
#include <cmath>
#include "include/Eigen/Dense"
#include "Blocks.hpp"
#include "Composites.hpp"
#include "KrigingParameters.hpp"
#include "Globals.hpp"

class KrigingEngine
{
public:
   KrigingEngine(const KrigingParameters& parameters, const Composites& composites, Blocks& blocks);
   static double Variogram(const double h, const VariogramParameters& parameters);
   static double Covariance(const double h, const VariogramParameters& parameters);
   static double OrdinaryKrigingPoint(double x0, double y0, double z0,
      const std::vector<double>& xs,const std::vector<double>& ys,const std::vector<double>& zs,
      const std::vector<double>& values, const VariogramParameters& parameters);
   double KrigeOneBlock(double blockX, double blockY, double blockZ);
   void RunKriging();

private:
   const KrigingParameters& mParameters;
   const Composites& mComposites;
   Blocks& mBlocks;
   static double EuclideanDistance(double x1, double y1, double z1, double x2, double y2, double z2);
};