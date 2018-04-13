
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
 *
 * TEST 내용
 *
 * 1) 구조체를 캐스팅
 * 2) 포인터 타입, 기본 자료타입으로 할때의 차이
 *
 */

typedef int passwd_t[4];
typedef struct Myinfo Myinfo_t;
struct Myinfo {
	passwd_t passwd_;
	char name_[16];
	char address_[16];
	int age_;
};

typedef struct MyinfoB MyinfoB_t;
struct MyinfoB {
	int year_;
	passwd_t passwd_;
	char name_[16];
	char address_[16];
	int age_;
};

int main(int args, char** argv) {
	int ret = -1;
	Myinfo_t info;
	/* info 내 자료 넣기 */
	info.passwd_[0] = 5;
	info.passwd_[1] = 5;
	info.passwd_[2] = 9;
	info.passwd_[3] = 0;
	strncpy(info.name_, "sangjun lee", 16);
	strncpy(info.address_, "TmaxOS", 16);
	info.age_ = 30; 
	/* TEST1 : 그냥 자료타입 선언후 casting */
	passwd_t Passwd; /* = int Passwd[4]; */
	passwd_t *pPasswd;
	void* pVoid;
	int *pintPasswd;

	/* TEST : 구조체 이름의 의미 */
	fprintf(stdout, "구조체 이름 출력 : %d\n", info);
	fprintf(stdout, "구조체 주소 출력 : %d\n", &info);
	fprintf(stdout, "구조체 첫번째 멤버 값 출력 : %d\n", info.passwd_[0]);
	fprintf(stdout, "구조체 첫번째 멤버 주소 출력 : %d\n", &info.passwd_[0]);

	/* TEST2 : Casting 테스트 */
	fprintf(stdout , "===================== TEST2 ===================\n");
	pintPasswd = (int *)&info; /* OK */
//	pintPasswd = reinterpret_cast<int*>(&info); /* OK */
//	pintPasswd = static_cast<int*>(&info); /* FAIL */
	pVoid = static_cast<void*>(&info);
	pintPasswd = static_cast<int*>(pVoid); /* OK : void에서 넘어옴 */
	pPasswd = reinterpret_cast<passwd_t *>(&info); /* OK */
//	pintPasswd = reinterpret_cast<passwd_t *>(&info); /* FAIL : 캐스팅 타입이
//	다름 */
	fprintf(stdout, "비밀번호 첫자리 (포인터) : %d\n", pintPasswd[0]);
	fprintf(stdout, "비밀번호 첫자리 (포인터) : %d\n", pPasswd[0]);

	fprintf(stdout , "===================== TEST3 ===================\n");
	/* TEST3 : 구조체 첫번째 인자 테스트 */
	MyinfoB_t infoB;
	/* info 내 자료 넣기 */
	infoB.year_ = 2017;
	infoB.passwd_[0] = 5;
	infoB.passwd_[1] = 5;
	infoB.passwd_[2] = 9;
	infoB.passwd_[3] = 0;
	strncpy(infoB.name_, "sangjun lee", 16);
	strncpy(infoB.address_, "TmaxOS", 16);
	info.age_ = 30; 
	fprintf(stdout, "구조체 이름 출력 : %d\n", infoB);
	fprintf(stdout, "구조체 주소 출력 : %d\n", &infoB);
	fprintf(stdout, "구조체 첫번째 멤버 값 출력 : %d\n", infoB.year_);
	fprintf(stdout, "구조체 첫번째 멤버 주소 출력 : %d\n", &infoB.year_);

	ret = 0;
	return ret;
}

/**
 *
 * 결론
 *
 * TEST1, TEST3
 * - 구조체 변수의 이름은 첫번째 멤버의 값을 가진다.
 * - 구조체 멤버의 주소는 첫번째 멤버의 주소와 동일.
 * - 첫번째 맴버가 배열로 선언될 경우 배열의 첫번째 값과 동일
 *
 * TEST2
 * - C++에서 C-style 캐스팅과 비슷한 것은 reinterpret_cast 
 * - Casting 은 주소값을 대상으로 들어가야 한다.
 *
 */


