#include <exception>
#include <fstream>
#include <streambuf>
#include <string>
#include "Settings.h"

bool Settings::Load(const char* path)
{
  try
  {
    using json = nlohmann::json;
    std::ifstream file(path);
    if (!file)
    {
      return false;
    }
    json j(json::parse(file));
    file.close();
    *this = j.get<Settings>();
  }
  catch (std::exception e)
  {
    return false;
  }
  return true;
}

bool Settings::Save(const char* path) const
{
  try
  {
    using json = nlohmann::json;
    json j(*this);
    std::ofstream file(path);
    if (!file)
    {
      return false;
    }
    std::string jdump = j.dump();
    file.write(jdump.c_str(), jdump.size());
    file.close();
  }
  catch (std::exception e)
  {
    return false;
  }
  return true;
}
