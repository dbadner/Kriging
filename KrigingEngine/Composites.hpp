#pragma once
#include <vector>
#include "include/nanoflann.hpp"

using namespace nanoflann;

struct NearestCompositesResult {
   std::vector<size_t> Indices;
   std::vector<double> Distances;
};

class Composites
{
   // Create a KD-tree of composite data
   typedef KDTreeSingleIndexAdaptor<
      L2_Simple_Adaptor<double, Composites>,
      Composites,
      3 /* dim: X,Y,Z*/
   > KDTree;

   KDTree* mKdTree;

public:
   std::vector<double> X, Y, Z;
   std::vector<double> Domain;
   std::vector<double> Grade;

   Composites();

   // Method to build kd tree index
   void BuildKDTreeIndex();

   // Method returns the nearest n composites to the given coordinates and a maximum spherical search distance
   // Return format: vector of <index, distance>
   NearestCompositesResult FindNearestComposites(double x, double y, double z, int n, double maxDist) const;

   // Add the required methods for nanoflann
   size_t kdtree_get_point_count() const;

   double kdtree_distance(const double* p1, const size_t idxp2) const;

   double kdtree_get_pt(const size_t idx, int dim) const;

   template <class BBOX>
   bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
};
