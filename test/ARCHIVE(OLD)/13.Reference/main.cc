#include <stdio.h>

#include <string>

class MyInfo {
	public:
		MyInfo(int age) : age_(age) { fprintf(stdout, "MY INFO 생성\n");}
		MyInfo(MyInfo* info) : age_(info->getAge()) { fprintf(stdout, "MY INFO 포인터 카피 생성\n"); }
		MyInfo(MyInfo& info) : age_(4) { fprintf(stdout, "MY INFO 참조 카피 생성\n");}
		~MyInfo() {}
		void setAge(int age) { age_ = age; }
		int getAge() { return age_; }
	private:
		std::string name_;
		int age_;
};

class MyClass : public MyInfo {
	public:
		MyClass (MyInfo* info) : MyInfo(info) { fprintf(stdout, "MY CLASS 생성\n");}
		~MyClass() {}
		
	private:
		int year_;
		std::string my_class_;
};

int main() {
	int ret = -1;
	MyInfo* info = new MyInfo(3);
	fprintf(stdout, "나이를 출력해보자!(info1) : %d\n", (*info).getAge());
	MyInfo* info2 = new MyInfo(info);
	fprintf(stdout, "나이를 출력해보자!(info2) : %d\n", info2->getAge());
	MyInfo* info3 = new MyInfo(*info);
	fprintf(stdout, "나이를 출력해보자!(info3) : %d\n", info3->getAge());
	ret = 0;
	MyClass* info4 = new MyClass(info);
	fprintf(stdout, "나이를 출력해보자!(info4) : %d\n", info4->getAge());
	return ret;
}
