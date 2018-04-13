#include <iostream>
#include <string>
#include <vector>

#define INVALID_MEMBER "invalid_member"

class Company {
	using MEMBER_NUMBER = int; 
//	using COMPANY_NOMEMBER = "invalid_member"; /* error : no type */
	public:
		std::string findMember(int num) {
			if (members_.size() > num) {
				std::string name = members_.at(num);
				return name;
			}
			else {
//				return "";
				return INVALID_MEMBER;
//				return COMPANY_NOMEMBER;
			}
		}
		int setMember(std::string& member) {
			int num = members_.size();
			members_.push_back(member);
			return num;
		}
	private:
		std::vector<std::string> members_;
};

int main() {
	Company com;
	std::string mem1("이상준");
	std::string mem2("김성현");
	std::string mem3("조건륜");
	std::string mem4("김기웅");
	com.setMember(mem1);
	com.setMember(mem2);
	com.setMember(mem3);
	com.setMember(mem4);
	std::cout << "memeber 1 : " << com.findMember(0) << std::endl;
	std::cout << "memeber 1 : " << com.findMember(1) << std::endl;
	std::cout << "memeber 2 : " << com.findMember(2) << std::endl;
	std::cout << "memeber 3 : " << com.findMember(3) << std::endl;
	std::cout << "memeber 4 : " << com.findMember(4) << std::endl;
	std::cout << "memeber 10 : " << com.findMember(10) << std::endl;
	return 0;
}