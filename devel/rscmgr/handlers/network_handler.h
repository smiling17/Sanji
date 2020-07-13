#ifndef _TOS_RESOURCE_NETWORK_HANDLER_H_
#define _TOS_RESOURCE_NETWORK_HANDLER_H_

#include "resource_handler.h"

namespace tos {
	namespace resource {
		namespace network {

			class NetworkHandler : public ResourceHandler final {
				public:
					enum class EventType = {};
					struct EventData = {};

					NetworkHandler();
					~NetworkHandler();

					void getTraficInfo(const pid_t pid = 0);
					void getTcpTraficInfo(const pid_t pid = 0);
					void getUdpTraficInfo(const pid_t pid = 0);

				private:
			};

		} /* namespace network */
	} /* namespace resource */
} /* namespace tos */

#endif /* #ifndef _TOS_RESOURCE_NETWORK_HANDLER_H_ */
