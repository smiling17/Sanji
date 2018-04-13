#include <string>
#include <iostream>
#include <vector>

class MyInfo {
	public:
		class MyPhone {
			public:
				void printPhoneNum() { fprintf(stdout, "%s\n", phone_num_.c_str()); }
			private:
				std::string phone_num_;
		};
		MyPhone* createPhone() {
			MyPhone* phone = new MyPhone();
			return phone;
		}
		void printMyinfo(int info) { fprintf(stdout, "***** int info %d*****\n", info); }
		void printMyinfo(const char* info) { fprintf(stdout, "***** char info %s*****\n", info); }
	private:
		class MySchool {
			public:
			private:
				std::string elementary;
				std::string middle;
				std::string high;
		};
		MySchool* createSchool();
		std::string name;
};

int main () {
	MyInfo info;
	info.printMyinfo(NULL);
	info.printMyinfo(nullptr);
//	MySchool school;
	MyInfo::MyPhone phone;
	return 0;
}
