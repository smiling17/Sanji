#include <string>
#include <vector>
#include <iostream>
#include <functional>


class Person {
	public:
		void setAge(int age) { age_ = age; }
		void getAge() { std::cout << "AGE : " << age_ << std::endl; }
		Person(int age) : age_(age) {}
		Person() {}
	private:
		int age_ = -1;
		unsigned int height_;
		std::string name_;
		std::string hometown_;
};

struct Animal {
	int age;
	std::string name;
	bool alive;
};

struct Info {
//	Info() : age(30) {}
	int weight;
	int height;
	const int age = 30;
};

int main() {

	std::cout << "===== TEST 1 ====\n";
	Person person;
	person.getAge();
	person.setAge(28);
	person.getAge();

	std::cout << "===== TEST 2 ====\n";
	Person person2(10);
	person2.getAge();
	
	std::cout << "===== TEST 3 ====\n";
	Animal animal_1;
	std::cout << "ANIMAL1's age : " << animal_1.age << std::endl;
	std::cout << "ANIMAL1's name : " << animal_1.name << std::endl;
	std::cout << "ANIMAL1's alive : " << animal_1.alive << std::endl;
	Animal animal_2 = {0, }; 
	std::cout << "ANIMAL2's age : " << animal_2.age << std::endl;
	Animal animal_3 = {};
	std::cout << "ANIMAL3's age : " << animal_3.age << std::endl;
	std::cout << "ANIMAL3's name : " << animal_3.name << std::endl;
	std::cout << "ANIMAL3's alive : " << animal_3.alive << std::endl;
//	Animal animal_4 = {"tiger", 30, }; 
//	std::cout << "ANIMAL4's age : " << animal_4.age << std::endl;
//	std::cout << "ANIMAL4's name : " << animal_4.name << std::endl;
//	std::cout << "ANIMAL4's alive : " << animal_4.alive << std::endl;
//	Animal animal_4 = {"tiger", true};
	std::cout << "===== TEST 4 ====\n";
	Info info1;
	fprintf(stdout, "age : %d\n", info1.age);

	return 0;
}
