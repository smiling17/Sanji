//#include <stdlib.h>
//#include <string.h>
#include <stdio.h>
//#include <iostream>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
void select();

#ifdef __cplusplus
}
#endif

void select() {
	fprintf(stdout, "select()! \n");
}

int main() {
	int ret = -1;

//	int a = fprintf();
//	fprintf(stdout, "A값 출력: %d\n", a);
	fprintf(stdout, "Main함수!\n");
	select();
	ret = 0;

	return ret;
}
