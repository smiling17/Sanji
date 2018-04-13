#include <string>
#include <iostream>

int main() {
	char* strA = "sunghyun kim";
	char strB[] = "sangjun lee";
	const char* strC = "jiyoung park"; /* 포인터 자체가 const */
	const char strD[] = "dahyun choi"; /* 배열속에 들어가는 인자가 const char */
	const char* const strE = "sangbin jung";

	strB[0] = 'k'; /* OK */
	fprintf(stdout, "strC before : %s\n", strC);
	strC = "hyunsoon kim";
	fprintf(stdout, "strC after : %s\n", strC);
	strE = "heonjin park"; /* FAIL : 주소 뿐만아니라 값도 const */
	strD[0] = 'b';
	return 0;
}
