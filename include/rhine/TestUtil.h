//-*- C++ -*-

#ifndef RHINE_TESTUTIL_H
#define RHINE_TESTUTIL_H

#include "rhine/Toplevel/ParseFacade.h"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

#include <string>

using namespace rhine;

namespace rhine {
void EXPECT_TRANSFORM_PP(std::string &SourcePrg, std::string &ExpectedPP);
void EXPECT_PARSE_PP(std::string SourcePrg, std::string ExpectedPP);
void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut);
}

#endif
