#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  if (argc >= 2)
  {
    if (std::string const command = argv[1]; command == "replace" && argc == 4)
    {
      std::string const what = argv[2];
      std::string const with = argv[3];
      for (std::string line; std::getline(std::cin, line); )
        std::cout << (what == line ? with : line) << '\n';
      return 0;
    }
  }
  return 1;
}
