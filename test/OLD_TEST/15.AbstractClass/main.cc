#include <iostream>
#include <string>

class MyInfoChild;

class MyInfoParent {
	public:
//		static MyInfoParent* getInstance() {
//			if (parent_ != NULL) {
//				parent_ = MyInfoChild::getInstanceChild();
//			}
//			return parent_;
//		};
		~MyInfoParent() {};
		virtual void getName() = 0;
		virtual void setName(std::string name) = 0;
	private:
		static MyInfoParent* parent_;
};

class MyInfoChild : public MyInfoParent {
	public:
		static MyInfoChild* getInstanceChild() {
			if (child_ != NULL) {
				child_ = new MyInfoChild("건륜");
			}
			return child_;
		}
		MyInfoChild(std::string name) : name_(name) {};
		~MyInfoChild() {};
		virtual void getName() override { std::cout << "이름출력 : " << name_ << std::endl; }
		virtual void setName(std::string name) override { name_ = name; }
	private:
		std::string name_;
		static MyInfoChild* child_;
};

int main() {
	MyInfoParent* parent = new MyInfoChild("상준");

	parent->getName();

	return 0;
}
