
#include "func.h"
#include <stdio.h>
#include <string.h>

struct MyInfo {
	char name[16];
};

void MyfuncB(const char* paramB) {
	struct MyInfo info;
	strncpy(info.name, paramB, 16);
	fprintf(stdout, "MyfuncB : %s\n", info.name);
}
