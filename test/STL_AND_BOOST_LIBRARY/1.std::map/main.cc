#include <string>
#include <iostream>
#include <unordered_map>

#include <stdio.h>


/* Cookie data : name / age pair */
class CookieData {
	public:
		void insertCookie(std::string url, int value);
		int getCookie(std::string url);
		void printCookie() {
			for (auto it : cookie_data_) {
				fprintf(stdout, "name : %s\n", it.first.c_str());
				fprintf(stdout, "age : %d\n", it.second);
			}
		}
		void findCookie(std::string url) {
			auto it = cookie_data_.find(url);
			if (it != cookie_data_.end()) {
				fprintf(stdout, "FIND : age = %d\n", it->second);
			}
			else {
				fprintf(stdout, "NOT FOUND : %s\n", url.c_str());
			}
		}
	private:
		std::unordered_map<std::string, int> cookie_data_;
};

void CookieData::insertCookie(std::string url, int value) {
	if (value < 0) {
		fprintf(stdout, "Invalid value of url : %s", url.c_str());
		return;
	}
	cookie_data_.insert( {url, value} );
}

int main() {
	CookieData* cookie = new CookieData();
	fprintf(stdout, "===== INSERT TEST =====\n");
	cookie->insertCookie( std::string("이상준"), 29 );
	cookie->insertCookie( std::string("김성현"), 30 );
	cookie->printCookie();
	fprintf(stdout, "===== FIND TEST =====\n");
	cookie->findCookie("이상준");
	cookie->findCookie("조건륜");
	if (cookie) {
		delete cookie;
	}
	return 0;
}

