
#include <stdio.h>


/* typedef를 이용해서 선언할 경우 */
typedef enum {
	Red,
	Blue,
	Green,
} Color;

typedef enum Car_ {
	Genesis,
	Sonata,
	Avante,
} Car;

/* typedef 없이 사용할 경우 */
enum Class {
	Math,
	History,
	Science,
};

enum {
	Sangjun,
	Kiwoong,
	Sangbin,
};

void Myfunc(Color a, Car b) {
	fprintf(stdout, "Color 출력 : %d\n", a);
	fprintf(stdout, "Car 출력 : %d\n", b);
}

void Myfunc2(int a, int b) {
	fprintf(stdout, "int a 출력 : %d\n", a);
	fprintf(stdout, "int b 출력 : %d\n", b);
}


int main () {
	int ret = -1;
	enum Class cls;
	Car car;
//	cls = 1; /* [ERROR] */
//	cls = Genesis; /* [ERROR] */
	cls = Math;

	fprintf(stdout, "========== TEST1 ==========\n");
	/* TEST1 : enum 끼리 비교(기존 enum의 특징) */
	if (Red == Genesis)
		fprintf(stdout, "typedef enum비교 성공!\n");

	if (Math == Sangjun)
		fprintf(stdout, "기본 enum비교 성공!\n");

	/* TEST2 : enum 출력 */
	fprintf(stdout, "========== TEST2 ==========\n");
	fprintf(stdout, "Red 출력 : %d\n", Red);
	fprintf(stdout, "Avante 출력 : %d\n", Avante);
	fprintf(stdout, "Math 출력 : %d\n", Math);
	fprintf(stdout, "Sangjun 출력 : %d\n", Sangjun);

	/* TEST3 : enum type의 인자 사용 */
	fprintf(stdout, "========== TEST3 ==========\n");
//	Myfunc(1, 1); /* [ERROR] 숫자대입 */
	Myfunc(Red, Avante); /* enum 대입 */
//	Myfunc(Math, Sangjun); /* [ERROR] 다른 enum 타입 대입 */

	/* TEST4 : int 인자의 함수에서 enum 사용 */
	fprintf(stdout, "========== TEST4 ==========\n");
	Myfunc2(Red, Avante);
	Myfunc2(Math, Sangjun);

	ret = 0;
	return ret;
}

/**
 *
 * < 결론 >
 *
 * - enum안의 변수명(?!)은 각각 내부에 상수를 담는다.
 * - [중요]enum 끼리 그냥 비교는 가능하다. (int도 가능)
 * - [중요] enum type으로 선언할 경우 해당 타입만 들어갈 수 있다. (int , 다른타입 X)
 * - [중요] int형 타입에는 enum이 그냥 들어갈 수 있다.
 *
 */
