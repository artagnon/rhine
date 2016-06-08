#pragma once

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "rhine/Parse/Parser.hpp"
#include "llvm/ADT/STLExtras.h"

using namespace std;

namespace rhine {
#define ANSI_COLOR_RED "\x1b[31;1m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33;1m"
#define ANSI_COLOR_BLUE "\x1b[34;1m"
#define ANSI_COLOR_MAGENTA "\x1b[35;1m"
#define ANSI_COLOR_CYAN "\x1b[36;1m"
#define ANSI_COLOR_WHITE "\x1b[37;1m"
#define ANSI_COLOR_RESET "\x1b[0m"

class ColorCode {
  std::string ColorF;

public:
  ColorCode(std::string Color) : ColorF(Color) {}
  friend std::ostream &operator<<(std::ostream &Dest, ColorCode const &Code) {
    if ((&Dest == &std::cerr && isatty(fileno(stderr))) ||
        (&Dest == &std::cout && isatty(fileno(stdout))))
      for (char ch : Code.ColorF) {
        Dest.put(ch);
      }
    return Dest;
  }
};

class DiagnosticPrinter {
public:
  std::ostream &ErrorStream = std::cerr;
  const Parser::Location SourceLoc = {};

  DiagnosticPrinter(std::ostream &ErrStream) : ErrorStream(ErrStream){};
  DiagnosticPrinter(const Parser::Location Loca) : SourceLoc(Loca){};

  template <typename T> DiagnosticPrinter &operator<<(const T &V) {
    unsigned ColSz = 200;

    if (SourceLoc.Filename.empty()) {
      ErrorStream << V;
      return *this;
    }

    unique_ptr<istream> InStream;
    if (SourceLoc.StringStreamInput) {
      InStream = llvm::make_unique<stringstream>(*SourceLoc.StringStreamInput);
    } else {
      InStream = llvm::make_unique<ifstream>(SourceLoc.Filename);
      if (!InStream->good()) {
        ErrorStream << ColorCode(ANSI_COLOR_RED)
                    << "fatal: " << ColorCode(ANSI_COLOR_WHITE)
                    << "Unable to open file " << SourceLoc.Filename
                    << ColorCode(ANSI_COLOR_RESET) << std::endl;
      }
    }
    std::ostringstream Oss;
    Oss << ColorCode(ANSI_COLOR_WHITE) << SourceLoc.Filename << ":"
        << SourceLoc.Begin.Line << ":" << SourceLoc.Begin.Column << ": "
        << ColorCode(ANSI_COLOR_RED) << "error: " << ColorCode(ANSI_COLOR_WHITE)
        << V << ColorCode(ANSI_COLOR_RESET);
    istringstream WrapStream(Oss.str());
    std::string Word;
    unsigned CharsWritten = 0;
    while (WrapStream >> Word) {
      if (Word.size() + CharsWritten > ColSz) {
        ErrorStream << std::endl;
        CharsWritten = 0;
      }
      ErrorStream << Word << " ";
      CharsWritten += Word.size() + 1;
    }
    std::string FaultyLine;
    for (unsigned int i = 0; i < SourceLoc.Begin.Line; i++)
      std::getline(*InStream, FaultyLine);

    ErrorStream << std::endl
                << FaultyLine << std::endl
                << std::setfill(' ') << std::setw(SourceLoc.Begin.Column)
                << ColorCode(ANSI_COLOR_GREEN) << '^';
    unsigned int end_col =
        SourceLoc.End.Column > 0 ? SourceLoc.End.Column - 1 : 0;
    if (end_col != SourceLoc.Begin.Column)
      ErrorStream << std::setfill('~')
                  << std::setw(end_col - SourceLoc.Begin.Column) << '~';
    ErrorStream << ColorCode(ANSI_COLOR_RESET) << std::endl;
    return *this;
  }

  DiagnosticPrinter &operator<<(std::ostream &(*F)(std::ostream &)) {
    F(ErrorStream);
    return *this;
  }
};
}
