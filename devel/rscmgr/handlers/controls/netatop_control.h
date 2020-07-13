#ifndef TOS_RESOURCE_CONTROLS_NETATOP_CONTROL_H_
#define TOS_RESOURCE_CONTROLS_NETATOP_CONTROL_H_

#include "netatop_common.h"

namespace tos {
	namespace resource {
		namespace controls {

			class NetatopControl final {
				public:
					using TrafficInfo = struct netpertask;

					NetatopControl();
					~NetatopControl();
					bool getTrafficInfo(const pid_t pid, TrafficInfo& info);

				private:
					void init();
					void copy(const struct netpertask& src, TrafficInfo& dst);
					bool kmoduleEnabled();

				private:
					int fd_;

			};

		} /* namespace controls */
	} /* namespace resource */
} /* namespace tos */


#endif /* #ifndef TOS_RESOURCE_CONTROL_NETATOP_CONTROL_H_ */
