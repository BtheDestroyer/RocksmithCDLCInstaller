#pragma once
#include "json.hpp"

struct Settings
{
  bool Load(const char* path);
  bool Save(const char* path) const;

  std::string path;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, path);
