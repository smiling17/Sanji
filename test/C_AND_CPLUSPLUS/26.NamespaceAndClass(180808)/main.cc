#include <iostream>
#include <string>
#include <vector>

class InfoParent {
  public:
    InfoParent() {}
    ~InfoParent() {}
};

class InfoChild : public InfoParent {
  public:
    InfoChild(std::string school) : school_(school) {}
    ~InfoChild() {}
    namespace School {}
      void getSchoolName() {
        fprintf(stdout, "SCHOOL : %s\n", school_.c_str());
      }
  private:
      std::string school_;
};

int main() {
  class InfoChild child("부경고등학교");
  child.getSchoolName();
  return 0;
}
