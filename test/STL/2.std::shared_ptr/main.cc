#include <string>
#include <memory>
#include <iostream>

class InfoBase {
	public:
		InfoBase() : name_("sangjun_lee2") {}
		InfoBase(std::string& name) : name_(name) {}
		void getName() const { std::cout << "NAME : " << name_ << std::endl; }
		void setName(std::string name) { name_ = name; }
	private:
		std::string name_;
};

class InfoDerivedWeight : public InfoBase {
	public:
		InfoDerivedWeight() = default;
		InfoDerivedWeight(const InfoBase* parent) : parent_(parent){
		}
		void getParentName() { return this->parent_->getName(); }
		void getWeight() { std::cout << "WEIGHT : " << weight_ << std::endl; }
		void setWeight(unsigned int weight) { weight_ = weight; }
	private:
		unsigned int weight_;
		std::shared_ptr<const InfoBase> parent_;
};

class InfoDerivedAge : public InfoBase {
	public:
		InfoDerivedAge() {}
		InfoDerivedAge(const InfoBase& parent) {}
		void getAge() { std::cout << "AGE : " << age_ << std::endl; }
		void setAge(unsigned int age) { age_ = age; }
	private:
		unsigned int age_;
};

int main() {
	std::shared_ptr<InfoBase> info1(new InfoDerivedWeight());
	InfoDerivedAge info2;
	InfoDerivedAge *info3 = new InfoDerivedAge();

	std::cout << "===== TEST 1 : shared_ptr initialization =====" << std::endl; /* std::endl : \n */
	InfoDerivedWeight *info4 = new InfoDerivedWeight(static_cast<InfoBase*>(info3));
	info4->getParentName();
	std::cout << "===== TEST 2 : shared_ptr reset =====" << std::endl;
	return 0;
}
