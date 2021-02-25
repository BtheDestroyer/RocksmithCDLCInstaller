#include <fstream>
#include <filesystem>
#include <iostream>

constexpr const char* k_ext = ".psarc";
#ifdef _WIN64
constexpr const char* k_installLocation = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Rocksmith2014\\dlc\\cdlc\\";
#else
constexpr const char* k_installLocation = "C:\\Program Files\\Steam\\steamapps\\common\\Rocksmith2014\\dlc\\cdlc\\";
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
  const std::string destinationPath(k_installLocation + sourceFile);
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
