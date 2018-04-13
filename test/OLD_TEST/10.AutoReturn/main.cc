#include <type_traits>

#include <stdio.h>

enum class Color : int {
	RED,
	BLUE,
	GREEN,
	BLACK,
};

enum class Music : char {
	ROCK,
	DANCE,
	RAP,
};

/* e라는 값을 E타입으로 static casting 해서 반환 */
template <typename E>
//template <class D> /* [ERROR] E가 명시되지 않음 */
constexpr auto my_type(E e) -> typename std::underlying_type<E>::type {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

#if 0
/* [ERROR] E의 선언을 알 수 없음 */
constexpr auto my_type2(E e) -> std::underlying_type<E>::type {
	return static_cast<std::underlying_type<E>::type>(e);
}
#endif

template <typename E>
#if 0 /* [ERROR] typename 키워드를 사용해야 한다. */
constexpr auto my_type2(E e) -> std::underlying_type<E>::type {
	return static_cast<std::underlying_type<E>::type>(e);
}
#endif
constexpr auto my_type2(E e) -> typename std::underlying_type<E>::type {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

int main() {
	int ret = -1;
	int Passwd[4] = {5, 5, 9, 0};

	fprintf(stdout, "비밀번호 1자리 : %d\n", Passwd[0]);
	fprintf(stdout, "비밀번호 2자리 : %d\n", Passwd[1]);
	fprintf(stdout, "비밀번호 3자리 : %d\n", Passwd[2]);
	fprintf(stdout, "비밀번호 4자리 : %d\n", Passwd[3]);

	fprintf(stdout, "\n========== INDEX 확인 ==========\n");

//	fprintf(stdout, "비밀번호 1자리 : %d\n", Passwd[RED]); /* [ERROR] RED 모름 */
//	fprintf(stdout, "비밀번호 1자리 : %d\n", Passwd[my_type(RED)]); /*[ERROR]
//	RED의 범위를 모름 */
	fprintf(stdout, "비밀번호 1자리 : %d\n", Passwd[my_type(Color::RED)]);
	fprintf(stdout, "비밀번호 2자리 : %d\n", Passwd[my_type(Color::BLUE)]);
	fprintf(stdout, "비밀번호 3자리 : %d\n", Passwd[my_type(Color::GREEN)]);
	fprintf(stdout, "비밀번호 4자리 : %d\n", Passwd[my_type(Color::BLACK)]);

	fprintf(stdout, "\n========== INDEX 확인 (typename X) ==========\n");
	fprintf(stdout, "비밀번호 1자리 : %d\n", Passwd[my_type2(Color::RED)]);
	fprintf(stdout, "비밀번호 2자리 : %d\n", Passwd[my_type2(Color::BLUE)]);
	fprintf(stdout, "비밀번호 3자리 : %d\n", Passwd[my_type2(Color::GREEN)]);
	fprintf(stdout, "비밀번호 4자리 : %d\n", Passwd[my_type2(Color::BLACK)]);
	ret = 0;
	return ret;
}

/**
 *
 * < 결론 >
 *
 * 템플릿은 바로 아래에 선언된 함수나 객체에만 영향을 준다.
 * enum class를 선언할 경우 반드시 범위를 설정해줘야 한다.
 * 템플릿을 사용할 때는 typename을 써주자.
 * underlying_type 함수를 쓸때는 앞에 typename 사용하기ㅁ
 *
 */
