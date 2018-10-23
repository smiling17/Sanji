#include <string>
#include <iostream>


struct Person {
  size_t num;
  size_t height;
  std::string name;
};

int main () {
  Person* pPerson = nullptr;
  Person* pPersonArray = new Person[10];
  /* setting */
  for (int i = 0; i < 10; ++i) {
    pPersonArray[i].num = i + 10;
  }
  /* getting */
  for (int i = 0; i < 10; ++i) {
    std::cout << "NUM 출력" << i << ":::" << pPersonArray[i].num  << std::endl;
  }
  return 0;
}
