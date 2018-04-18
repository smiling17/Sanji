
#include "func.h"
#include <stdio.h>

//struct Mystruct;

struct MyInfo {
	int year;
};

void MyfuncA(int paramA) {
	struct MyInfo info;

	info.year = paramA;
	fprintf(stdout, "MyfuncA : %d\n", info.year);
}
