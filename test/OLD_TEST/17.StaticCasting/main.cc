#include <stdio.h>

#include <iostream>
#include <memory>
#include <string>

class InfoParent {
	public:
		int getType() { return type_; }
		virtual ~InfoParent() {}
	protected:
		InfoParent(int type) : type_(type) {}
	private:
		int type_;
};

class InfoChild : public InfoParent {
	public:
		InfoChild(int type) : InfoParent(type) {}
		~InfoChild() {}
		void setAge(int age) { age_ = age; }
		int getAge() { return age_; }
	private:
		int age_;
};

int main() {
	int ret = -1;
	fprintf(stdout, "===== TEST1 =====\n");
	InfoParent *parent = new InfoChild(5);
	fprintf(stdout, "parent type : %d\n", parent->getType());
	
	fprintf(stdout, "===== TEST2 =====\n");
//	InfoChild *child = parent; /* error */
//	InfoChild *child = dynamic_cast<InfoChild*>(parent); /* success */
	InfoChild *child = static_cast<InfoChild*>(parent); /* success */
	fprintf(stdout, "child type : %d\n", child->getType());

	fprintf(stdout, "===== TEST3 =====\n");
	InfoChild child2(10);
	fprintf(stdout, "child2 type : %d\n", child2.getType());
	
	InfoParent parent2 = static_cast<InfoParent>(child2);
	fprintf(stdout, "parent2 type : %d\n", parent2.getType());

	InfoChild *child3 = static_cast<InfoChild*>(&parent2);
	fprintf(stdout, "child3 type : %d\n", child3->getType());
	
	ret = 0;
	return ret;
}
