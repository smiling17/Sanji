#include <iostream>
#include <string>

class MyInfo {
	public:
		void setName(std::string name) { name_ = name; }
		void getName() { fprintf(stdout, "NAME : %s\n", name_.c_str()); }
		MyInfo() {}
		
		/* constructor */
		MyInfo(const MyInfo& ) { fprintf(stdout, "===== TYPE 1 =====\n"); }
		MyInfo(MyInfo&& obj) { fprintf(stdout, "===== TYPE 2 =====\n"); }

		/* operator */
		MyInfo& operator=(const MyInfo& obj) = default; 
		MyInfo& operator=(MyInfo&& obj) {
			fprintf(stdout, "MOVE OPERATOR\n");
			return *this;
		}
	private:
		std::string name_;
};

int main() {
	fprintf(stdout, "===== INFO 1 =====\n");
	MyInfo info1;
	info1.setName("sangjun lee");
	info1.getName();

	fprintf(stdout, "===== INFO 2 =====\n");
	MyInfo info2(info1);
	info2.getName();

	fprintf(stdout, "===== INFO 3 =====\n");
	MyInfo info3;
	info3 = info1;
	info3.getName();

	fprintf(stdout, "===== INFO 4 =====\n");
	MyInfo info4;
	info4 = std::move(info1);
	info4.getName();

	return 0;
}
