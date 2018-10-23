#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
  const char myname[] = "sangjun";
  const char* ptr = myname;
  ptr = NULL;
  return 0;
  /*
  memset(ptr, 0x0, 100);
  snprintf(ptr, 100, "sangjun_lee");
  fprintf(stdout, "MY NAME(BEFORE) : %s\n", ptr);
  if (ptr) {
    free(ptr);
    ptr = NULL;
  }
  fprintf(stdout, "MY NAME(AFTER) : %s\n", ptr);
  return 0;
  */
}
