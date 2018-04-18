#include <stdio.h>

#include <string>
#include <iostream>

#define INFO_HANDLER(func) 					\
		if (parent_)						\
			parent_->func();				\
		else 								\
			fprintf(stdout, "정보없음\n");  \
		
enum class Topic {
	Unknown = 0,
	Age = 1,
	Height = 2,
	Weight = 3,
};

class InfoGetter {
	public:
		InfoGetter(InfoGetter* getter, Topic topic) : parent_(getter), topic_(topic) {}
		~InfoGetter() {}
		virtual void getAge() { INFO_HANDLER(getAge) }
		virtual void getHeight() { INFO_HANDLER(getHeight) }
		virtual void getWeight() { INFO_HANDLER(getWeight) }
//		virtual void getAge() {
//			if (parent_) {
//				parent_->getAge();
//			}
//			else {
//				fprintf(stdout, "정보없음!\n");
//			}
//		}
//		virtual void getHeight() {
//			if (parent_) {
//				parent_->getHeight();
//			}
//			else {
//				fprintf(stdout, "정보없음!\n");
//			}
//		}
//		void getWeight() {
//			if (parent_) {
//				parent_->getWeight();
//			}
//			else {
//				fprintf(stdout, "정보없음!\n");
//			}
//		}
	private:
		InfoGetter* parent_;
		Topic topic_;
};

class Information1 : public InfoGetter {
	public:
		Information1(InfoGetter* parent) : age_(29), InfoGetter(parent, Topic::Age) {}
		~Information1() {}
		virtual void getAge() {
			fprintf(stdout, "age : %d\n", age_);
		}
	private:
		unsigned int age_;
};

class Information2 : public InfoGetter {
	public:
		Information2(InfoGetter *parent) : height_(180), InfoGetter(parent, Topic::Weight) {}
		~Information2() {}
		virtual void getHeight() {
			fprintf(stdout, "height : %d\n", height_);
		}
		void getWeight() {
			fprintf(stdout, "Weight : 70\n");
		}
		private:
		unsigned int height_;
};

int main () {
	
	fprintf(stdout, "===== TEST 1 =====\n");
	Information1 *info1 = new Information1(nullptr);
	Information2 *info2 = new Information2(info1);
	info1->getAge();
	info1->getHeight();
	info2->getAge();
	info2->getHeight();
	info2->getWeight();

	fprintf(stdout, "===== TEST 2 =====\n");
	InfoGetter *getter2 = static_cast<InfoGetter*>(info2);
	getter2->getAge();
	getter2->getHeight();
	getter2->getWeight();
	return 0;
}
