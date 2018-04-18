#include <stdio.h>

#include <string>
#include <vector>
#include <functional>

#include <curl/curl.h>

/* libcurl */

//typedef std::function<int()> CALLBACK;
int (*CALLBACK)();
typedef std::function<size_t(char*, size_t, size_t, void*)> CURL_CALLBACK;
//typedef int (*CALLBACK)();

/* MyInfo Setting */
struct MyInfo {
	void setCallback(class School *school);
//	void setCurlCallback(CURL* session);
	int MyCallback();
	size_t MyCurlCallback(char* a, size_t b, size_t c, void* d);
	std::string name; 
	unsigned int age;
	unsigned int num;
};

size_t MyInfo::MyCurlCallback(char* a, size_t b, size_t c, void* d) {
	fprintf(stdout, "콜백성공!\n");
	return 0;
}

//void MyInfo::setCurlCallback(CURL* session) {
//	curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, std::bind(&MyInfo::MyCurlCallback, this));
//}

int GlobalCallback() {
	fprintf(stdout, "global callback()!\n");
	return 0;
}

class School {
	public:
		School() : isCallback(false) {}
		int getCallback() {
			if (this->isCallback == false) {
				fprintf(stdout, "there is no callback func!\n");
				return -1;
			}
			fprintf(stdout, "getCallback start\n");
			this->call_back_();
			fprintf(stdout, "getCallback end\n");
			return 0;
		}
		void setCallback(const CALLBACK call_back);
		void findStudent(int num);
		void addStudent(struct MyInfo student);
		void presentStudent(struct MyInfo student);
	private:
		std::vector<struct MyInfo> students_;
		bool isCallback;
		CALLBACK call_back_;
//		std::function<int()> call_back_;
};

void School::setCallback(const CALLBACK call_back) {
	this->call_back_ = call_back;
	this->isCallback = true;
	fprintf(stdout, "setCallback!\n");
	this->call_back_();
}

void School::addStudent(struct MyInfo student) {
	this->students_.push_back(student);
	fprintf(stdout, "add student\n"); 
}

void School::findStudent(int num) {
	for (auto it: this->students_) {
		if (it.num == num) {
			presentStudent(it);
			return;
		}
	}
}

void School::presentStudent(struct MyInfo student) {
	fprintf(stdout, "===== present student =====\n");
	fprintf(stdout, "my age : %d\n", student.age);
	fprintf(stdout, "my num : %d\n", student.num);
}

void MyInfo::setCallback(class School *school) {
	school->setCallback(std::bind(&MyInfo::MyCallback, this));
}

int MyInfo::MyCallback() {
	fprintf(stdout, "Callback함수 : %d\n", this->age);
	return 0;
}

int main() {
	class School school;
	struct MyInfo info;
	struct MyInfo *pinfo = new struct MyInfo();
	pinfo->name = "sangjun lee";
	pinfo->age = 29;
	pinfo->num = 1;
	
	pinfo->setCallback(&school);
	CALLBACK func = GlobalC allback;
	CALLBACK func = pinfo->MyCallback;

//	school.setCallback(pinfo->MyCallback);
	school.addStudent(*pinfo);
	info.name = "조건륜";
	info.age = 29;
	info.num = 2;
	info.MyCallback();
	school.addStudent(info);
	info.name = "김성현";
	info.age = 30;
	info.num = 3;
	info.MyCallback();
	school.addStudent(info);
	fprintf(stdout, "===== TEST #1 =====\n");
	school.getCallback();
	fprintf(stdout, "===== TEST #2 =====\n");


	fprintf(stdout, "===== TEST END =====\n");

	return 0;

}
