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

//namespace toc {
//	namespace tuk2 {
template <typename T>
class WininetUrl16 {
	using STRING = std::basic_string<T>;
	public :
		WininetUrl16() = delete;
		WininetUrl16(T& url);
		~WininetUrl16() {}

		/* getter */
		void refreshUrl(T url);
		T getHostName();
		T getUserName();
		T getPassword();
		T getProtocol();
		T getUrlPath();
		T getExtraInfo();
		T getPort();

	private:
		void init(T url);

		T url_;
		T canonocalized_url_;
		T host_name_;
		T user_name_;
		T password_;
		T protocol_;
		T url_path_;
		T extra_info_;
		T port_;

};
//	} /* namespace toc */
//} /* namespace tuk2 */

#endif

