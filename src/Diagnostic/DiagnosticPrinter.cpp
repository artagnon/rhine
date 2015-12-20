#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Parse/Parser.hpp"

using Location = rhine::Parser::Location;

namespace rhine {

DiagnosticPrinter::DiagnosticPrinter(std::ostream &ErrStream)
    : ErrorStream(ErrStream) {}

DiagnosticPrinter::DiagnosticPrinter(Parser::Location Loca)
    : SourceLoc(Loca) {}
}
