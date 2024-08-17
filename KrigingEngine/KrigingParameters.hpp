#pragma once
// Simplified isotropic variogram with single structure
struct VariogramParameters
{
   enum StructureType {
      Spherical = 0,
      Exponential = 1,
      Gaussian = 2
   };

   double Nugget;
   double Sill;
   double Range;
   StructureType Structure;
};

// Simplified block model, no sub-blocks, no rotation
struct BlockModelInfo
{
   double MinX, MinY, MinZ;
   double MaxX, MaxY, MaxZ;
   double BlockCountI, BlockCountJ, BlockCountK;
};

// Simplified kriging parameters, no octants, isotropic
struct KrigingParameters
{
   enum KrigingType
   {
      Ordinary = 0
      // TODO: Support other types of kriging
   };

   KrigingType Type; // Type of kriging, not used yet
   int Domain; // Domain to interpolate, not used yet
   int MinNumComposites; // Minimum number of composites per block
   int MaxNumComposites; // Maximum number of composites per block
   double MaxRadius; // Maximum isotropic search radius
   VariogramParameters VariogramParameters;
   BlockModelInfo BlockModelInfo;
};