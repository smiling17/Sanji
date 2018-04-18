#include <string>

#include "wininet_utility.h"

namespace toc {
	namespace tuk2 {
		WininetUrl16::WininetUrl16(std::string& url) {
			this->url_ = url;
			int colon = url_.find_first_of(':');
			if (colon == std::string::npos) {
				fprintf(stdout, "invalid Address\n");
				abort();
			}
			protocol_ = url_.substr(0, colon);

			/* TODO : if 문 대신 for문을 통해 구현 */
			if (protocol_.compare("http") == 0) {
				this->scheme_ = INTERNET_SCHEME_HTTP;
			}
			else if (protocol_.compare("https") == 0) {
				this->scheme_ = INTERNET_SCHEME_HTTPS;
			}
			else if (protocol_.compare("ftp") == 0) {
				this->scheme_ = INTERNET_SCHEME_FTP;
			}
			else {
				this->scheme_ = INTERNET_SCHEME_UNKNOWN;
			}
			/* default port */
			this->port_ = INTERNET_INVALID_PORT_NUMBER;

			// <protocol>:[//<net_loc>]/path][;<param>][?<query>][#<fragment]
			/* host name setting */
			std::string url_net = url_.substr(colon+1);
			if (url_net[0] == '/' && url_net[0] == '/') {
				url_net = url_net.substr(2);
			}
			int at_sign = -1;
			if ((at_sign = url_net.find_first_of('@')) != std::string::npos) {
				std::string user_info = url_net.substr(0, at_sign);
				int password_start = -1;
				if ((password_start = user_info.find_first_of(':')) != std::string::npos) {
					this->user_name_ = user_info.substr(0, password_start);
					this->password_ = user_info.substr(password_start+1);
				}
				else {
					this->user_name_ = user_info;
				}
				url_net = url_net.substr(at_sign+1);
			}

			/* host / port setting */
			int host_end = -1;
			int port_start = -1;
			if ((host_end = url_net.find_first_of('/')) != std::string::npos) {
				if ((port_start = url_net.find_first_of(':', host_end)) != std::string::npos) {
					this->host_name_ = url_net.substr(0, port_start);
//					this->port_ = url_net.substr(port_start+1, host_end - port_start);
				}
				else {
					this->host_name_ = url_net.substr(0, host_end);
				}
				url_net = url_net.substr(host_end);
			}
			/* path/extra info */
			int extra_start = -1;
			if ((extra_start = url_net.find_first_of('?')) != std::string::npos || 
					(extra_start = url_net.find_first_of('#')) != std::string::npos) {
				this->url_path_ = url_net.substr(0, extra_start);
				this->extra_info_ = url_net.substr(extra_start);
			}
			else {
				this->url_path_ = url_net;
			}
		}
		std::string WininetUrl16::getHostName() {
			return this->host_name_;
		}
		std::string WininetUrl16::getProtocol() {
			return this->protocol_;
		}
		std::string WininetUrl16::getUserName() {
			return this->user_name_;
		}
		std::string WininetUrl16::getPassword() {
			return this->password_;
		}
		std::string WininetUrl16::getUrlPath() {
			return this->url_path_;
		}
		std::string WininetUrl16::getExtraInfo() {
			return this->extra_info_;
		}
		unsigned int WininetUrl16::getScheme() {
			return this->scheme_;
		}
		unsigned int WininetUrl16::getPort() {
			return this->port_;
		}
	}
}
