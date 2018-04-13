/**
 * @file    wininet_common.h
 * @brief   TODO brief documentation here.
 *
 * @history
 */

#ifndef _WININET_COMMON_DLL_H_
#define _WININET_COMMON_DLL_H_

/***********************************************************
 * 함수 형식 정의 (2017-08-24)
 *
 * dlls
 *  - int WINAPI funcA(){ } 로 정의
 *  - return toc::winapi::Foo::FooFunction(); TUK2내 함수 호출
 *  2017-08-31 ~
 *  - retrun toc::winapi::fucA()로 통일
 *
 * lib
 *  - namespace : toc, winapi 이중 사용
 *  - 생성자는 사용 X : ex. Foo() = delete; 로 정의
 *  - 함수이름은 그대로 사용 -> TOC와 같은 prefix 사용하지 않는다.
 *  - static 함수로 정의 (DLL이름의 객체 내)
 *  - 다른 내용은 있으면 안된다.
 *  - 내부 구현체는 tuk2 namespace로 구현
 *
 */

#define name_space1 toc
#define name_space2 winapi
//#define class_name Wininet

/***********************************************************
 *
 * TUK2_FUNCTION
 *
 * DLLS -> LIB/TUK2 내 함수로 변환
 *  - define으로 정의(전처리기)
 *
 * ex. int Foo(int a, int b) 함수 변환
 *    TUK2_FUNCTION(int, FOO, a, b) -> tuk2::winapi::wininet::FOO(a,b) 로 치환
 *
 */

#define TUK2_CONVERT(func) name_space1::name_space2::func
#define TUK2_FUNCTION(type, func, ...) TUK2_CONVERT(func)(__VA_ARGS__)

#endif /* _WININET_COMMON_DLL_H_ */
