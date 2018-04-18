#include <stdio.h>

void myfunc(void* paramInt, void* paramStr) {
	int year = 0;
	char *name = NULL;

	year = (int)paramInt;
	name = (char*)paramStr;

	fprintf(stdout, "Year : %d, Name : %s\n", year, name);
}

int main () {

	int ret = 0;
	int year = 2017;
	int *pYear = NULL;
	char *name = "sangjun";
	void* vYear = NULL;
	void* vName = NULL;

//	*vYear = year; /* [ERROR] void라는 변수타입에는 넣을수 없음 */
//	vYear = year; /* [ERROR] 주소값이 들어가야 하는데 변수값이 입력 */
	
//	vYear = (int*)year; /*[SUCCESS] vYear에는 year에 있는 값이 주소처럼, NOT GODD. */
	
	vYear = (void *)year; /* [SUCCESS] ,but 주소가 출력됨 */
	vName = (void *)name;

	pYear = (int *)&year; /* 컴파일 성공, but 이건 주소를 2017, NOT GOOD */
	
	fprintf(stdout, "pYear 출력 : %d\n", *pYear); 

	myfunc((void*)year, (void*)name);
	myfunc(vYear, vName);

	return 0;
}

/**
 *
 * < 결론 >
 *
 * - void 포인터에는 어떤 값이든 메모리 주소로 인식
 * - 캐스팅 : 변수의 타입을 바꾸는 것
 *
 */
