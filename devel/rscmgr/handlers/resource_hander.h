#ifndef _TOS_RESOURCE_RESOURCE_HANDLER_H_
#define _TOS_RESOURCE_RESOURCE_HANDLER_H_

namespace tos {
	namespace resource {

		class ResourceHandler {
			public:
				virtual void enable() = 0;
				virtual void disable() = 0;
				virtual bool isEanbled() = 0;

				template<class Handler>
					class ResourceObserver {
						public:
							using EventType = Handler::EventType;
							using EventData = Handler::EventData;

							void onEvent(const EventType& type, const EventData& data) = 0;
							void registerEvent(const EventType type);
							void unregisterEvent(const EventType type);

						private:
					};

			protected:
				/* observer for notification */
				ResourceHandler();
		};
	} /* namespace resource */
} /* namespace tos */

#endif  /* #ifndef _TOS_RESOURCE_RESOURCE_HANDLER_H_ */
