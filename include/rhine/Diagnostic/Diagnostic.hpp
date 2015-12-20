//-*- C++ -*-

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "rhine/Parse/Parser.hpp"

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
  std::string StringStreamInput;
  const Parser::Location SourceLoc = {};

  DiagnosticPrinter(std::ostream &ErrStream);
  DiagnosticPrinter(const Parser::Location Loca);

  template <typename T> DiagnosticPrinter &operator<<(const T &V) {
    if (SourceLoc.Begin.Filename.empty()) {
      ErrorStream << V;
      return *this;
    }
    assert(SourceLoc.Begin.Filename == SourceLoc.End.Filename);
    assert(SourceLoc.Begin.Line == SourceLoc.End.Line);

    std::istream *InStream;
    std::istringstream Iss(StringStreamInput);
    std::ifstream InFile(SourceLoc.Begin.Filename);

    if (SourceLoc.Begin.Filename == "string stream") {
      InStream = &Iss;
    } else {
      if (!InFile)
        ErrorStream << ColorCode(ANSI_COLOR_RED)
                    << "fatal: " << ColorCode(ANSI_COLOR_WHITE)
                    << "Unable to open file " << SourceLoc.Begin.Filename
                    << ColorCode(ANSI_COLOR_RESET) << std::endl;
      InStream = &InFile;
    }
    ErrorStream << ColorCode(ANSI_COLOR_WHITE) << SourceLoc.Begin.Filename
                << ":" << SourceLoc.Begin.Line << ":" << SourceLoc.Begin.Column
                << ": " << ColorCode(ANSI_COLOR_RED)
                << "error: " << ColorCode(ANSI_COLOR_WHITE) << V
                << ColorCode(ANSI_COLOR_RESET);
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

#endif
