#pragma once

#include <vector>

#include "include/nanoflann.hpp"
#include "Blocks.hpp"

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
public:
   const std::vector<double> X, Y, Z; // Composite/sample center locations; cannot be modified once class is initialized
   const std::vector<double> Grade; // Composite grades

   /**
    * @brief Reads in composites from csv file, filtering based on block extents and search radius
    */
   Composites(const std::string csvFilePath, const BlockModelInfo blockExtents, const double maxSearchRadius);

   /**
    * @brief Initializes composites by copying input vectors of x,y,z coordinates, and grades
    *
    * NOTE: Not memory efficient; recommended to use csv based import
    */
   Composites(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& grades);

   /**
    * @brief Dispose of Kd Tree.
    */
   ~Composites();

   /**
    * @brief Finds the nearest n composites to the given coordinates and a maximum spherical search distance.
    *
    * @return Nearest composite result, comprising vectors of composite indices in order of increasing distance, and corresponding distances.
    */
   NearestCompositesResult FindNearestComposites(const double x, const double y, const double z, const int n, const double maxDist) const;

   /**
    * @brief Required methods below for nanoflann.
    */
   size_t kdtree_get_point_count() const;

   double kdtree_distance(const double* p1, const size_t idxp2) const;

   double kdtree_get_pt(const size_t idx, const int dim) const;

   template <class BBOX>
   bool kdtree_get_bbox(BBOX&) const { return false; }

private:
   // Create a KD-tree of composite data
   using KDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, Composites>, Composites, 3>;
   KDTree* mKdTree;

   // Build the KdTree index; should only be called by the constructor
   void BuildKdTree();
};
