#include "netatop_control.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

namespace tos {
	namespace resource {
		namespace controls {

NetatopControl::NetatopControl() {
	init();
}

void NetatopControl::init() {
	if ( (fd_ = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("Open raw socket");
		exit(2);
	}
	if (!kmoduleEnabled()) {
		perror("netatop kernel module");
		exit(2);
	}
	int ret = getsockopt(netsock, SOL_IP, NETATOP_GETCNT_TGID,
			&npt, &len);
}

bool NetatopControl::kmoduleEnabled() {
	struct netpertask npt;
	socklen_t len = sizeof(npt);

	int ret = getsockopt(fd_, SOL_IP, NETATOP_PROBE,
			&npt, &len);

	return (ret == 0);
}

		} /* namespace controls */
	} /* namespace resource */
} /* namespace tos */
	return true;
}

bool NetatopControl::getTrafficInfo(const pid_t pid, TrafficInfo& info) {
	struct netpertask npt;
	socklen_t len = sizeof(npt);

	int ret = getsockopt(fd_, SOL_IP, NETATOP_GETCNT_TGID,
			&npt, &len);

	if (ret != 0) {
	}
	copy(npt, info);
}

void NetatopControl::copy(const struct netpertask& src, TrafficInfo& dst) {
	dst.tc.tcpsndpacks = src.tc.tcpsndpacks;
	dst.tc.tcpsndbytes = src.tc.tcpsndbytes;
	dst.tc.tcprcvpacks = src.tc.tcprcvpacks;
	dst.tc.tcprcvbytes = src.tc.tcprcvbytes;

	dst.tc.udpsndpacks = src.tc.udpsndpacks;
	dst.tc.udpsndbytes = src.tc.udpsndbytes;
	dst.tc.udprcvpacks = src.tc.udprcvpacks;
	dst.tc.udprcvbytes = src.tc.udprcvbytes;
}

		} /* namespace controls */
	} /* namespace resource */
} /* namespace tos */
