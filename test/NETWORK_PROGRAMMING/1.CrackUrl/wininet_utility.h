#ifndef _WININET_UTILITY_H_
#define _WININET_UTILITY_H_

//#include <include/wininet.h>

#include <string>

enum wininet_port {
	INTERNET_INVALID_PORT_NUMBER = -1,
};

enum wininet_scheme {
	INTERNET_SCHEME_UNKNOWN = -1,
	INTERNET_SCHEME_HTTP = 0,
	INTERNET_SCHEME_HTTPS = 1,
	INTERNET_SCHEME_FTP = 2,
};

namespace toc {
	namespace tuk2 {
		class WininetUrl16 {
			public :
				WininetUrl16() = delete;
				WininetUrl16(std::string& url);
				~WininetUrl16() {}
				std::string getHostName();
				std::string getUserName();
				std::string getPassword();
				std::string getProtocol();
				std::string getUrlPath();
				std::string getExtraInfo();
				unsigned int getScheme();
				unsigned int getPort();

			private:
				std::string url_;
				std::string canonocalized_url_;
				std::string host_name_;
				std::string user_name_;
				std::string password_;
				std::string protocol_;
				std::string url_path_;
				std::string extra_info_;
				unsigned int port_;
				unsigned int scheme_;

		};
	} /* namespace toc */
} /* namespace tuk2 */

#endif

