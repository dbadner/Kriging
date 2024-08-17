#include "Composites.hpp"

Composites::Composites()
{
   
}

//TODO: Refactor so KDTree is built in a constructor for robustness. Maybe move to new class
void Composites::BuildKDTreeIndex()
{
    // Create a KD-tree of composite data
    mKdTree = new KDTree(3, *this);
    mKdTree->buildIndex();
}

// TODO: Optimize this method and nanoflann parameters for improved performance
NearestCompositesResult Composites::FindNearestComposites(double x, double y, double z, int n, double maxDist) const
{
   double point[3] = { x, y, z };
   double maxDistSq = maxDist * maxDist;

   // Perform the nearest neighbor search
   std::vector<size_t> indices(n);
   std::vector<double> distancesSq(n);
   KNNResultSet<double> resultSet(n);
   resultSet.init(indices.data(), distancesSq.data());
   mKdTree->findNeighbors(resultSet, &point[0]);

   // Filter out distances greater than maxDist
   NearestCompositesResult result;
   for (size_t i = 0; i < indices.size(); i++)
   {
      if (distancesSq[i] <= maxDistSq)
      {
         result.Indices.push_back(indices[i]);
         result.Distances.push_back(sqrt(distancesSq[i]));
      }
   }

   return result;
}

// Add the required methods for Nanoflann
inline size_t Composites::kdtree_get_point_count() const
{
   return X.size();
}

inline double Composites::kdtree_distance(const double* p1, const size_t idxp2) const
{
   double d0 = p1[0] - X[idxp2];
   double d1 = p1[1] - Y[idxp2];
   double d2 = p1[2] - Z[idxp2];
   return d0 * d0 + d1 * d1 + d2 * d2;
}

inline double Composites::kdtree_get_pt(const size_t idx, int dim) const
{
   if (dim == 0) return X[idx];
   else if (dim == 1) return Y[idx];
   else return Z[idx];
}
