#ifndef _TOS_RESOURCE_RESOURCE_MODEL_H_
#define _TOS_RESOURCE_RESOURCE_MODEL_H_

#include <string>

namespace tos {
	namespace resource {

		class ResourceModel {
			public:
				enum class RscType : unsigned int {
					none = 0,
					cpu = (1 << 0),
					mem = (1 << 1),
					net = (1 << 2),
				};

				struct RscNetInfo {};
				struct RscCpuInfo {};
				struct RscMemInfo {};
				struct RscProc {
					std::string proc;
					pid_t pid;
					struct RscNetInfo;
					struct RscCpuInfo;
					struct RscMemInfo;
				};
				struct RscGlobal {
					struct RscCpuInfo;
					struct RscMemInfo;
					struct RscNetInfo;
				};
			public:

				ResourceModel();
				bool getRscInfo(const pid_t pid, struct RscProc& data);
				bool getRscInfo(struct RscGlobal& data);

			private:
				bool getProcessInfo();
				bool getCpuInfo(const pid_t pid = 0);
				bool getMemoryInfo(const pid_t pid = 0);
				bool getNetworkInfo(const pid_t pid = 0);

			private:
		};

		using RscType = ResourceModel::RscType
		using T = std;:underlying_type<ResourceModel::RscType>::type;
		inline RscType operator|(RscType lhs, RscType rhs) {
			return static_cast<RscType>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}
		inline RscType operator|=(RscType lhs, RscType rhs) {
			return static_cast<RscType>(static_cast<T>(lhs) |= static_cast<T>(rhs));
		}
		inline RscType operator&(RscType lhs, RscType rhs) {
			return static_cast<RscType>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}
		inline RscType operator&=(RscType lhs, RscType rhs) {
			return static_cast<RscType>(static_cast<T>(lhs) &= static_cast<T>(rhs));
		}
		inline RscType operator~(RscType lhs, RscType rhs) {
			return static_cast<RscType>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}
	} /* namespace resource */
} /* namespace tos */

#endif /* #ifndef _TOS_RESOURCE_RESOURCE_MODEL_H_ */
