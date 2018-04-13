#include <stdio.h>

#include <string>
#include <vector>

class InfoBase {
	public:
		InfoBase(){}
		~InfoBase(){}
		virtual void setInfo() = 0;
		virtual void getInfo() = 0;
	private:
};

class InfoStudent : public InfoBase {
	public:
		static InfoStudent* getInstance() {
			if (instance == nullptr)
				instance = new InfoStudent;
			return instance;
		}
		virtual void setInfo() override {
			this->info = 10;
		}
		virtual void getInfo() override {
			fprintf(stdout, "I'm student : %d\n", this->info);
		}
	private:
		int info;
		static InfoStudent* instance;
};

class InfoParent : public InfoBase {
	public:
		static InfoParent* getInstance() {
			if (instance == nullptr)
				instance = new InfoParent;
			return instance;
		}
		virtual void setInfo() override {
			this->info = 100;
		}
		virtual void getInfo() override {
			fprintf(stdout, "I'm Parent : %d\n", this->info);
		}
	private:
		int info;
		static InfoParent* instance;

};

InfoParent* InfoParent::instance = nullptr;
InfoStudent* InfoStudent::instance = nullptr;

int main () {
	InfoBase* student = InfoStudent::getInstance();
	InfoBase* parent = InfoParent::getInstance();
	student->setInfo();
	student->getInfo();
	parent->setInfo();
	parent->getInfo();
	return 0;
}


