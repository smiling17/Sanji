/**
 * @file    main.c
 * @brief   TODO brief documentation here.
 *
 * @history
 */

#include <stdio.h>
#include <string.h>
int test_api_num_args_2(const char* req_type) {

int testApi(const char* req_type) {
  if (strstr(req_type, "OK")) {
    fprintf(stdout, "CHECK OK\n");
  }
  else {
    fprintf(stdout, "CHECK FAIL\n");
  }
  return 0;
}

int main () {
  int ret = testApi("OK");
  int ret2 = testApi("FAIL");

  return ret;
}

/********************************************
 *
 *
 * SUMMARY
 *
 *
 *******************************************/

/* end of main.c */
