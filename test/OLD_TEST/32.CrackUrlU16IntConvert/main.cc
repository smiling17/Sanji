#include <stdio.h>

#include <string>
#include <iostream>

struct UrlComponent {
	std::u16string host_name;
	std::u16string protocol;
	int scheme;
};

enum scheme {
	SCHEME_NONE = 0,
	SCHEME_FTP = 1,
	SCHEME_HTTP = 2,
};

int find_scheme(std::u16string protocol) {
	if (protocol.compare(u"ftp") == 0) {
		return SCHEME_FTP;
	}
	else if (protocol.compare(u"http") == 0) {
		return SCHEME_HTTP;
	}
	else {
		return SCHEME_NONE;
	}
}

int main() {
	int ret = -1;
	std::u16string http_url(u"http://www.naver.com/index.html");
	std::u16string ftp_url(u"ftp://www.google.com/index.html");

	int colon = http_url.find_first_of(':');
	if (colon == std::string::npos) {
		fprintf(stdout, "invalid Address!!\n");
	}
	fprintf(stdout, "\n========== TEST 1 : PROTOCOL 확인 ==========\n");
	std::u16string protocol = http_url.substr(0, colon);
	for (auto it = protocol.begin(); it != protocol.end(); ++it) {
		fprintf(stdout, "protocol name : %c\n", (*it));
	}

	fprintf(stdout, "\n========== TEST 2 : HOST 확인 ==========\n");
	int scheme = find_scheme(protocol);
	fprintf(stdout, "Scheme : %d\n", scheme);
	std::u16string host_name = http_url.substr(colon+1);
	fprintf(stdout, "hostname 0 : %c\n", host_name[0]);
	fprintf(stdout, "hostname 1 : %c\n", host_name[1]);
	fprintf(stdout, "hostname 2 : %c\n", host_name[2]);
	if (host_name[0] == '/' || host_name[1] == '/') {
		fprintf(stdout, "변경하자!\n");
		host_name = host_name.substr(2, host_name.size());
	}
	else {
		fprintf(stdout, "invalid address");
	}
	for (auto it = host_name.begin(); it != host_name.end(); ++it) {
		fprintf(stdout, "host name : %c\n", (*it));
	}
	fprintf(stdout, "\n========== TEST 3 : lenth 확인 ==========\n");
	fprintf(stdout, "origin lenth : %d\n", http_url.size());
	fprintf(stdout, "protocol lenth : %d\n", protocol.size());
	fprintf(stdout, "\n========== TEST 3 : u16 -> int 형변환 확인 ==========\n");
	std::u16string port = u"22";
	int port_num = std::stoi(port, nullptr, 2);

	return 0;
}
