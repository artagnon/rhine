#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Overshadow, Local) {
  auto SourcePrg =
      R"rh(
        def main do
          Handle = 0;
          Handle = 2;
        end
      )rh";
  auto ExpectedErr = "string stream:4:11: error: symbol Handle attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Overshadow, Function) {
  auto SourcePrg =
      R"rh(
        def main do
          ret 3
        end
        def main do
          ret 2
        end
      )rh";
  auto ExpectedErr = "string stream:5:13: error: function main attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Overshadow, Argument) {
  auto SourcePrg =
      R"rh(
        def main(main Int) do
          ret $ main + 2;
        end
      )rh";
  auto ExpectedErr = "string stream:2:18: error: argument main attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
