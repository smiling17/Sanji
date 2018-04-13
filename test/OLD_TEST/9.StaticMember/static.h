#include <string>
#include <iostream>

class Myinfo {
	public:
		void getMyname() { std::cout << "My name : " << name_ << std::endl; }
		void setMyname(std::string name) { name_ = name; }
	private:
		static std::string name_;
};

//std::string Myinfo::name_ = "sangbin";
