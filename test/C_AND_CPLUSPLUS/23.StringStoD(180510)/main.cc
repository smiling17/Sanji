#include <string>
#include <iostream>


int main() {
  std::string test = std::to_string(0);
  double test_double = stod(test);
  fprintf(stdout, "test_double : %d\n", test_double);

  return 0;
}
