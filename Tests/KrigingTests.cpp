#include "pch.h"
#include "TestMain.cpp"

namespace KrigingEngineTests
{
   class KrigingTests : public ::testing::Test {
   protected:
      VariogramParameters parameters;
      const double maxError = 1e-5;

      void SetUp() override {
         // Initialize VariogramParameters
         parameters.Range = 200.0;
         parameters.Sill = 1.0;
         parameters.Nugget = 0.1;
         parameters.Structure = VariogramParameters::StructureType::Spherical;
      }

      void TearDown() override {

      }
   };

   TEST_F(KrigingTests, LagGTRangeReturnsSillValue) {

      // Define the point to be estimated
      double x0 = 2.5;
      double y0 = 2.5;
      double z0 = 2.5;

      // Define the coordinates and values of the composites
      std::vector<double> xs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> ys = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> zs = { 0.0, 1.0, 2.0, 3.0, 4.0 };
      std::vector<double> values = { 0.1, 0.1, 0.1, 0.1, 0.1 };

      double okResult = KrigingEngine::OrdinaryKrigingPoint(x0, y0, z0, xs, ys, zs, values, parameters);

      ASSERT_NEAR(2.5, okResult, maxError);
   }
}