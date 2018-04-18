#include <memory>
#include <string>
#include <iostream>

class MyInfo {
	public:
		MyInfo(int age, std::string& name) : age_(age), name_(&name){}
		MyInfo(MyInfo& Info) : name_(Info.name_) {}
		~MyInfo() {}
		void getHeight() { std::cout << "Height : " << height_ << std::endl; }
		void getAge() { std::cout << "Age : " << age_ << std::endl; }
		void getMyname() { std::cout << "Name : " << *(name_.get()) << std::endl; }
			
	private:
		int age_;
		std::shared_ptr<std::string> name_;
		int height_;
		int weight_;
};

/* TEST : struct 전반선언 */
typedef struct _my_class_t my_class_t;

struct _my_class_t {
	std::string teacher;
	int class_num;
};


int main () {
	int ret = -1;
	std::string *name = new std::string("sangjun");
	std::shared_ptr<std::string> name2(new std::string("sanghyun"));
	std::cout << "이름 선언 : " << *name << std::endl;
	std::cout << "이름 선언 : " << *name2.get() << std::endl;

	/* TEST1 : name의 자원에 대해 선언 */
	MyInfo info1(3, *name);
	info1.getAge();
	info1.getMyname();

	/* TEST2  : name의 자원에 대한 참조를 증가 */
	MyInfo* info2 = new MyInfo(info1);
	info2->getAge();
	info2->getMyname();

	ret = 0;
	return ret;
}
