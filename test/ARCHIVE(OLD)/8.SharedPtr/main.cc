#include <stdio.h>

#include <iostream>
#include <memory> /* shared ptr ... */
#include <string> /* std::string */

class MyInfo {
	public:
		MyInfo() { std::cout << "생성자! " << std::endl; }
		MyInfo(std::string info) {
			info_ = info;
			std::cout << "생성자 : " << info_ << std::endl; }
		~MyInfo() { std::cout << "소멸자 : " << info_ << std::endl; }
		void getMyname () { std::cout << "이름출력 " << name_ << std::endl; }
		void setMyname (std::string name) {name_ = name;}
	private:
		std::string name_;
		std::string info_;
};

typedef std::shared_ptr<MyInfo> MyInfo_ptr;
typedef std::auto_ptr<MyInfo> MyInfo_aptr;
typedef std::unique_ptr<MyInfo> MyInfo_uptr;

int main() {
	int ret = -1;

	fprintf(stdout, "\n========== TEST1 ==========\n");
	MyInfo_ptr info1(new MyInfo("info1"));
	info1->setMyname("sangjun");
	info1->getMyname();

	fprintf(stdout, "\n========== TEST2 ==========\n");
	/* 여기까지는 각각이 메모리를 가리키고 있다. */
	MyInfo_ptr info2(new MyInfo("info2"));
	info2->getMyname();
	info1->getMyname();

	fprintf(stdout, "\n========== TEST3 ==========\n");
	info2 = info1;
	info2->getMyname();
	info1->getMyname();

	fprintf(stdout, "\n========== TEST4 ==========\n");
	MyInfo_uptr info3(new MyInfo("info3"));
	MyInfo_uptr info4;
//	info4 = info3; /* error */
	
	fprintf(stdout, "\n========== TEST5 ==========\n");
	fprintf(stdout, "\n========== info5 생성자(객체)  ==========\n");
	MyInfo_aptr info5(new MyInfo("info5"));
	if (info5.get() == NULL)
		fprintf(stdout, "info5 값 초기화\n");
	else
		fprintf(stdout, "info5 값 존재\n");
	
	/* 복사 생성자로 확인 */
	fprintf(stdout, "\n========== info6 생성자(복사)  ==========\n");
	MyInfo_aptr info6(info5); 
	if (info5.get() == NULL)
		fprintf(stdout, "info5 값이 없어짐\n");
	else
		fprintf(stdout, "info5 값 유지\n");
	
	/* 초기화 확인 */
	fprintf(stdout, "\n========== info7 생성자(NULL)  ==========\n");
	MyInfo_aptr info7;
	if (info7.get() == NULL)
		fprintf(stdout, "info7 초기화 O\n\n");
	else
		fprintf(stdout, "info7 초기화 X\n\n");

//	info6 = info5; /* [ERROR] 대입 X */
//	info6(info5); /* [ERROR] 이후 복사 X */
	fprintf(stdout, "\n========== TEST END ==========\n");
	ret = 0;
	return ret;
}

/**
 *
 * < 결론 >
 *
 * 스마트 포인터는 자원관리에 활용
 * 소멸자는 해제되면 알아서 해제됨
 * 스마트 포인터가 가리키는 것은 처음 초기화할 때만 가능 (unique)
 * shared_ptr의 경우 자원공유가 가능하고 ref count를 통해 관리
 * unique_ptr은 auto_ptr과 달리 복사생성자 지원 X (대입도 안됨)
 * 내부 함수는 .get(), .reset()등으로 할 수 있다.
 *
 */
