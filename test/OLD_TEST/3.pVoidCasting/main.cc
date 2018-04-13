
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct myInfo myInfo_t;
struct myInfo {
	int age;
	int year;
	char name[32];
	char address[32];
};

void myFunc(void* data) {
	int ret = -1;
	char name[16];
	char* prefix;

	fprintf(stdout, "============== TEST 1 ===============\n");
	fprintf(stdout, "input 확인 : %s\n", (char*)data);
	/* void pointer로 넘어온 인자를 복사 */
	strncpy(name, "sangjun", 16);
	strncat(name, (char*)data, 16); /* FAIL */
	/* prefix 인풋으로 받았을 때 */
	prefix = (char *)data;
	strncat(name, prefix, 16); /* SUCCESS */
	/* 느낌표 추가 */
	strncat(name, " !!!", 16);
	fprintf(stdout, "내이름은? %s\n", name);
}

int main() {
	int ret = -1;
	char prefix[8] = {0, };
	void* input;
	char* chr;

	/* TEST1 : void* input 테스트 */
	strncpy(prefix, " Lee", 8);
	input = (void*)prefix;
	myFunc(input);

	/* TEST2 : struct, struct* 사이즈 테스트 */
	myInfo_t *pInfo;
	myInfo_t Info;

	fprintf(stdout, "============== TEST 2 ===============\n");
	fprintf(stdout, "구조체사이즈 : %d\n", sizeof(Info));
	fprintf(stdout, "구조체사이즈 : %d\n", sizeof(myInfo_t));
	fprintf(stdout, "포인터사이즈 : %d\n", sizeof(pInfo));
	fprintf(stdout, "포인터사이즈 : %d\n", sizeof(myInfo_t *));
	fprintf(stdout, "포인터연산 사이즈 : %d\n", sizeof(* pInfo));
//	fprintf(stdout, "포인터연산 사이즈 : %d\n", sizeof(* Info)); 컴파일 에러
	fprintf(stdout, "============== TEST 3 ===============\n");
	/* TEST3 : 자료형 사이즈 테스트 */
	fprintf(stdout, "char* 사이즈 : %d\n", sizeof(char*));
	fprintf(stdout, "char 사이즈 : %d\n", sizeof(char));
	fprintf(stdout, "int 사이즈 : %d\n", sizeof(int));
	fprintf(stdout, "int* 사이즈 : %d\n", sizeof(int*));
	fprintf(stdout, "void* 사이즈 : %d\n", sizeof(void*));
	fprintf(stdout, "long 사이즈 : %d\n", sizeof(long));
	fprintf(stdout, "long long 사이즈 : %d\n", sizeof(long long));
	fprintf(stdout, "long long* 사이즈 : %d\n", sizeof(long long *));
	fprintf(stdout, "long* 사이즈 : %d\n", sizeof(long*));

	ret = 0;
	return ret;
}

/**
 *
 * < 결론 >
 *
 * TEST1
 *
 * 1) void*로 넘어오는 값은 그 자체가 주소값이다
 * 2) 그래서 따로 &를 처리할 필요가 없다.
 *
 * TEST2
 *
 * 1) 포인터의 실제 사이즈를 받고싶을 때는 앞에 *연산을 쓰자
 * 2) 포인터의 사이즈는 32/64bit에 영향을 받는다.
 *
 * TEST3
 *
 * 1) 포인터는 메모리 주소의 크기이므로 자료형과는 전혀 상관없다.
 * 	32bit로 나타낼수 있는 실제 주소는 4G로 한정되어 있음. (2^32개)
 * 	실제 주소공간은 4G까지 있으므로 타입은 해당 메모리 주소에서 얼마만큼
 * 	정의할건가에 달림
 * 2) int나 long이나 크기는 4bytes
 *
 */
