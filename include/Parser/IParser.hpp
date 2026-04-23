#pragma once

#include <Parser/Node.hpp>
#include <memory>

class IParser {
 public:
  IParser() = default;
  virtual ~IParser() = default;
  IParser(const IParser&) = delete;
  IParser(IParser&&) = delete;
  IParser operator=(const IParser&) = delete;
  IParser operator=(IParser&&) = delete;

  virtual std::unique_ptr<INode> parse() = 0;
};