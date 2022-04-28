#include <filesystem>

#include <iostream>

int main(int argc, char** argv)
{
  auto p = std::filesystem::current_path();
  auto s = p.string();
  std::cout << s << std::endl;
  return 0;
}
