//-*- C++ -*-

#ifndef RHINE_TESTUTIL_H
#define RHINE_TESTUTIL_H

#include <string>

#define CAT_RH(Tok) #Tok

namespace rhine {
/// Test that SourcePrg parses to the given Rhine IR; all transforms are run,
/// and no LLVM operation happens.
void EXPECT_IR(std::string &SourcePrg, std::string &ExpectedIR);

/// Call EXPECT_IR with "" as the second argument.
void EXPECT_SUCCESSFUL_PARSE(std::string &SourcePrg);

/// Test the output of Module->dump() for an LLVM IR Module.
void EXPECT_LL(std::string &SourcePrg, std::string &ExpectedLL);

/// Run the program and expect an the given string on stdout.
void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut);

/// Test that the program fails to run with the given error message.
void EXPECT_COMPILE_DEATH(std::string &SourcePrg, std::string &ExpectedErr);
}

#endif
