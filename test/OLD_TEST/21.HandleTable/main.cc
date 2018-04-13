#include <stdio.h>

#include <memory>
#include <vector>
#include <string>
#include <iostream>

class MyClass {
	public:
		MyClass() {}
		~MyClass() {}
		int addStudent (std::string name) {
			fprintf(stdout, "before size : %d\n", students_.size());
			int index = students_.size();
			std::shared_ptr<std::string> student(new std::string(name));
			students_.push_back(student);
			fprintf(stdout, "after size : %d\n", students_.size());
			return index;
		}
		void findStudent (unsigned int num) {
			if (num < students_.size() ) {
				if (students_.at(num).get() != nullptr)
					fprintf(stdout, "학생확인 : %s\n", students_.at(num).get()->c_str());
				else
					fprintf(stdout, "학생없음!\n");
			}
		}
		void totalStudent() {
			fprintf(stdout, "total students : %d\n", students_.size());
			fprintf(stdout, "total capacity : %d\n", students_.capacity());
		}
		void deleteStudnet(std::string name) {
			for (auto it = students_.begin(); it != students_.end(); ++it) {
				if ((*it).get()->compare(name) == 0)
					fprintf(stdout, "delete ! %s\n", (*it).get()->c_str());
//					students_.erase(it);
					(*it).reset();
					break;
			}
		}
	private:
		std::vector<std::shared_ptr<std::string>> students_;
};

int main () {
	MyClass my_class;
	int num1 = my_class.addStudent("이상준");
	int num2 = my_class.addStudent("김성현");
	int num3 = my_class.addStudent("김기웅");
	int num4 = my_class.addStudent("조건륜");
	fprintf(stdout, "=========== TEST ==========\n");
	my_class.totalStudent();
	my_class.findStudent(num1);
	my_class.findStudent(num2);
	my_class.findStudent(num3);
	my_class.findStudent(num4);
	my_class.deleteStudnet("이상준");
	my_class.totalStudent();
	my_class.findStudent(num1);
	my_class.findStudent(num2);
	my_class.findStudent(num3);
	my_class.findStudent(num4);
	return 0;
}

/**
 * 결론
 * 1) vector를 쓰면 size()로 인덱스 할 수 있으나
 * 2) 중간에 원소를 삭제할 경우 인덱스가 달라지기 때문에 문제가 생긴다.
 * 3) shared_ptr() 를 이용한 reset
 */
