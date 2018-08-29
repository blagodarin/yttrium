#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  if (argc != 3)
    return 1;
  std::string const what = argv[1];
  std::string const with = argv[2];
  for (std::string line; std::getline(std::cin, line); )
    std::cout << (what == line ? with : line) << '\n';
  return 0;
}
