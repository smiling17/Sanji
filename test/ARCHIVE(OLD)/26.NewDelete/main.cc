#include <stdio.h>
#include <string.h>

typedef char *CHAR, *SANGJUN;

int main () {
//	SANGJUN ptr = reinterpret_cast<SANGJUN>(new char[100]);
	char* tmp = new char[100];
	char swap[20];
	fprintf(stdout, "tmp 주소 : %x\n", tmp);
	SANGJUN ptr = reinterpret_cast<SANGJUN>(tmp);
	fprintf(stdout, "ptr 주소 : %x\n", ptr);
	strncpy(ptr, "sangjun", 100);
	strncat(ptr, "\0", 100);
	snprintf(swap, sizeof(swap), "%x", ptr);
//	ptr++; /* error : Invalid pointer */
	fprintf(stdout, "ptr 내용 : %s", ptr);
	delete ptr;
	fprintf(stdout, "========== TEST : delete 이후 ==========\n");
	fprintf(stdout, "ptr주소 : %x\n", ptr); /* 주소는 초기화 안됨 */
	fprintf(stdout, "swap 주소 : %s\n", &swap);
	fprintf(stdout, "swap 이름 : %s\n", swap);
	fprintf(stdout, "ptr 내용 : %s\n", ptr); /* 엉뚱한 값이 들어옴 */
	if (ptr)
		fprintf(stdout, "ptr은 여전히 살아있음..\n");
	return 0;
}
