#pragma once

#include <vector>

#include "include/nanoflann.hpp"

using namespace nanoflann;

/**
 * @brief Nearest composite result, comprising vectors of composite indices in order of increasing distance, and corresponding distances.
 */
struct NearestCompositesResult
{
   std::vector<size_t> Indices;
   std::vector<double> Distances;
};

/**
 * @brief Class containing composite / sample information.
 */
class Composites
{
   // Create a KD-tree of composite data
   typedef KDTreeSingleIndexAdaptor<
      L2_Simple_Adaptor<double, Composites>,
      Composites,
      3
   > KDTree;

   KDTree* mKdTree;
   //TODO: convert to use an object rather than pointer for safety

public:
   std::vector<double> X, Y, Z;  // Composite/sample center locations
   std::vector<int> Domain;      // TODO: to be implemented with geology matching
   std::vector<double> Grade;    // Composite grades

   ~Composites();

   /**
    * @brief Method to build the KdTree index.
    *
    * IMPORTANT: Must be called after composite X,Y,Z locations are defined.
    */
   void BuildKDTreeIndex();
   //TODO: Refactor so composites and KDTree are built in a constructor for robustness. Possibly split to separate class.

   /**
    * @brief Finds the nearest n composites to the given coordinates and a maximum spherical search distance.
    *
    * @return Nearest composite result, comprising vectors of composite indices in order of increasing distance, and corresponding distances.
    */
   NearestCompositesResult FindNearestComposites(const double x, const double y, const double z, const int n, const double maxDist) const;

   // Required methods below for nanoflann
   /**
    * @brief Required methods below for nanoflann.
    */
   size_t kdtree_get_point_count() const;

   double kdtree_distance(const double* p1, const size_t idxp2) const;

   double kdtree_get_pt(const size_t idx, int dim) const;

   template <class BBOX>
   bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
};
