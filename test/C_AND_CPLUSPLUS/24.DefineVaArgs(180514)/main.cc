#include <string>
#include <iostream>
#include <vector>

#define USB_DEBUG

#define SYSTEM_DEBUG(...) \
  fprintf(stdout, "ARGS : %s\n", ##__VA_ARGS__)

#define SYSTEM_DEBUG_V22(...) \
  char *ptr = nullptr;        \
  va_list vl;                 \
  va_start(vl);               \
  vfprintf(vl);               \
  va_end(vl);                 \

int main() {
  int ret = 0;
  SYSTEM_DEBUG("a", "b", "c");
  SYSTEM_DEBUG_V2("a", "b", "c");
  return 0;
}
