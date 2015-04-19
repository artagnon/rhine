#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

TEST(Constant, ConstantInt) {
  auto Source = rhine::ConstantInt::get(32);
  auto PP = rhine::irToPP(Source);
  EXPECT_STREQ(PP.c_str(), "32 ~ IntegerType");
}
