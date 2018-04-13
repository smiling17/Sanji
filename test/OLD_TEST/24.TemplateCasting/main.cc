
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

class BaseInfo {
	public:
		BaseInfo(int type) : type_(type) {}
		void setType(int type) { type_ = type; }
		void getType() { fprintf(stdout, "type : %d\n", type_); }
	private:
		int type_;
};

class DerivedInfo1 : public BaseInfo {
	public:
		DerivedInfo1(int type) : BaseInfo(type) {}
};

class DerivedInfo2 : public BaseInfo {
	public:
		DerivedInfo2(int type) : BaseInfo(type) {}

	private:
};

class InfoFactory {
	public:
		template <class Info>
		Info* createInfo(int index) {
			Info *info = new Info(index);
			return info;
		}

};

int main() {
	InfoFactory factory;
	DerivedInfo1 *info1 = factory.createInfo<DerivedInfo1>(10);
	DerivedInfo2 *info2 = factory.createInfo<DerivedInfo2>(20);

	info1->getType();
	info2->getType();
	return 0;
}

