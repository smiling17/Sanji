
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct myInfo {
	int age;
	char* name;
	char address[16];
};

int main() {
	int ret = -1;
	struct myInfo infoA;
	int namelen = 100;

	infoA.name = (char*)malloc(100);
	infoA.age = 30;
	strncpy(infoA.address, "1359", 16);
	snprintf(infoA.name, 100, "sangjun_lee");
	fprintf(stdout, "내이름을 출력하자(infoA) : %s\n", infoA.name);

	/* 구조체 복사 : shallow capy */
	struct myInfo infoB;
	infoB = infoA;
	fprintf(stdout, "내이름을 출력하자(infoB) : %s\n", infoB.name);

	/* 정보변경 */
	strncat(infoB.name, " tmaxOS", namelen);
	fprintf(stdout, "변경된 이름(infoB) : %s\n", infoB.name);
	fprintf(stdout, "변경된 이름(infoA) : %s\n", infoA.name);
	if (strncmp(infoA.name, infoB.name, namelen) == 0) /* 이름이 같을 경우 */
		fprintf(stdout, "변경영향을 받는다!\n");
	else
		fprintf(stdout, "따로 영향을 받는다!\n");

	ret = 0;

	/* TEST2 : 구조체 포인터 테스트 */
	struct myInfo *infoC = NULL;
	infoC = &infoB;
	fprintf(stdout, "내이름을 출력하자(infoC) : %s\n", infoC->name);

	/* TEST3 : 배열할당 테스트 */
	fprintf(stdout, "내 주소를 출력(infoA) : %s\n", infoA.address);
	fprintf(stdout, "내 주소를 출력(infoB) : %s\n", infoB.address);
	strncat(infoA.address, " 1359", 16);
	fprintf(stdout, "변경주소 출력(infoA) : %s\n", infoA.address);
	fprintf(stdout, "변경주소 출력(infoB) : %s\n", infoB.address);

	/* TEST4 : 배열과 포인터 관계 */
	fprintf(stdout, "배열의 이름(infoA) : %s\n", infoA.address);
	fprintf(stdout, "배열의 이름 첫번째(infoA) : %c\n", infoA.address[0]);
	fprintf(stdout, "배열의 첫번째(infoA) : %s\n", &infoA.address[0]);
	fprintf(stdout, "배열의 두번째(infoA) : %s\n", &infoA.address[1]);
	fprintf(stdout, "배열의 세번째(infoA) : %s\n", &infoA.address[2]);
//	infoA.address = &infoA.address[0]; 
//	infoA.address는 포인터 상수이므로 대입불가
	char *addressB = NULL;
	addressB = &infoA.address[1]; /* 포인터 변수에는 대입 가능 */
	fprintf(stdout, "포인터변수 대입(addressB) : %s\n", addressB);
	/**********************************************************************
	 * 
	 * 결론
	 *
	 * 1) 구조체를 복사하면 객체와 같이 shallow copy가 된다.
	 * 2) 포인터 변수에서는 주소값을 바꿀수 있다.
	 * 3) 배열포인터는 포인터 상수이므로 가리키는 주소를 바꿀 수 없다 (배열이름)
	 * 4) 배열의 출력은 주소값으로부터 시작하여 문자형태(char)로 출력하는데
	 * 마지막은 NULL을 기준으로 한다.
	 * 5) 배열내 인자를 넣으면 주소가 아닌 값이기 때문에 %s가아닌 %c로 출력
	 * 
	 */

	return ret;
}
