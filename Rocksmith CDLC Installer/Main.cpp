#include <fstream>
#include <filesystem>
#include <iostream>
#include "Settings.h"

constexpr const char* k_ext = ".psarc";
constexpr const char* k_settingsFile = "RSCDLCI_settings.json";
#ifdef _WIN64
constexpr const char* k_steamInstallLocation = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Rocksmith2014\\dlc\\cdlc\\";
#else
constexpr const char* k_steamInstallLocation = "C:\\Program Files\\Steam\\steamapps\\common\\Rocksmith2014\\dlc\\cdlc\\";
#endif

void pause()
{
  std::cout << "Press enter/return to close..." << std::endl;
  std::cin.get();
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Drag a " << k_ext << " file onto this application to install it!" << std::endl;
    pause();
    return -1;
  }
  Settings settings;
  const std::string exeFile(argv[0]);
  std::string settingsFile(exeFile.substr(0, exeFile.find_last_of('\\') + 1));
  settingsFile += k_settingsFile;
  if (!settings.Load(settingsFile.c_str()))
  {
    // No settings loaded
    // Ask for install path
    std::cout << "Where should CDLC be installed?" << std::endl << "Copy the path and right click this window to paste it or type it out manually, then press enter/return." << std::endl;
    std::cout << "If you installed Rocksmith 2014 through Steam and used the default settings, the path is:" << std::endl << "  " << k_steamInstallLocation << std::endl << "Path: ";
    settings.path.clear();
    char buffer[1024];
    std::cin.getline(buffer, sizeof(buffer));
    settings.path = buffer;
    if (settings.path.size() == 0)
    {
      std::cerr << "No path provided! Please restart the program and provide a valid path!" << std::endl;
      pause();
      return -6;
    }
    if (settings.path.back() != '\\')
    {
      settings.path += '\\';
    }
    if (!settings.Save(settingsFile.c_str()))
    {
      std::cerr << "Failed to save settings! Make sure this program is stored in a location you can write files without admin access." << std::endl;
      pause();
      return -7;
    }
  }

  const std::string sourcePath(argv[1]);
  if (!std::strcmp(&sourcePath.back() - sizeof(k_ext), k_ext))
  {
    std::cerr << "Provided file does not have the " << k_ext << " extension!" << std::endl;
    pause();
    return -2;
  }
  std::string sourceFile;
  std::size_t lastSlash = sourcePath.find_last_of('\\');
  if (lastSlash == std::string::npos)
  {
    lastSlash = sourcePath.find_last_of('/');
  }
  if (lastSlash != std::string::npos)
  {
    sourceFile = sourcePath.substr(lastSlash + 1);
  }
  else
  {
    sourceFile = sourcePath;
  }
  std::ifstream source(sourcePath, std::ios::binary | std::ios::ate);
  if (source.fail())
  {
    std::cerr << "Couldn't open source file: " << sourcePath << std::endl;
    pause();
    return -3;
  }
  const std::streampos filesize = source.tellg();
  source.seekg(0, std::ios::beg);
  const std::string destinationPath(settings.path + sourceFile);
  if (std::filesystem::exists(destinationPath))
  {
    std::cout << "Destination file already exists: " << destinationPath << std::endl;
    char input = '\0';
    const auto isYes = [&input]()
    {
      return input == 'y' || input == 'Y' || input == '\r' || input == '\n';
    };
    const auto isNo = [&input]()
    {
      return input == 'n' || input == 'N';
    };
    do
    {
      std::cout << "Replace it? (Y/N): ";
      std::cin >> input;
    } while (!isYes() && !isNo());
    if (isNo())
    {
      return -4;
    }
  }
  std::ofstream dest(destinationPath, std::ios::binary | std::ios::trunc);
  if (dest.fail())
  {
    std::cerr << "Couldn't open destination file: " << destinationPath << std::endl;
    pause();
    return -5;
  }
  std::vector<char> buffer;
  buffer.resize(size_t(filesize));
  source.read(buffer.data(), filesize);
  source.close();
  dest.write(buffer.data(), filesize);
  dest.close();
  std::cout << "Installed!" << std::endl;
  if (std::remove(sourcePath.c_str()))
  {
    std::cerr << "Couldn't delete source file: " << sourcePath << std::endl;
  }
  return 0;
}
