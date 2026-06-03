#pragma once


#include "Parser/IParser.hpp"

class MockParser : public IParser {
public:
std::unique_ptr<ProgramNode> buildProgram() override {
  return nullptr;
}
};