#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>

class WininetManager {
	enum message {
		UNKNOWN = 0,
		NORMAL,
		SECURE,
		SUPER,
		COMPANY,
	};
	public:
		bool setMessage(int message) {
			async_lock_.lock();
			this->async_thrd_ = std::thread( [=] {processMessage(message); });
			return true;
		}
		bool getMessage() {
			async_lock_.lock();
//			std::lock_guard<std::mutex> guard(async_lock_);
			std::cout << "MY MESSAGE : " << this->message << std::endl;
			async_lock_.unlock();
			return true;
		}
		~WininetManager() {
			std::cout << "aysnc thread join start\n";
			this->async_thrd_.join();
			std::cout << "aysnc thread join end\n";
		}
	private:
		bool processMessage(int message) {
//			std::lock_guard<std::mutex> guard(async_lock_);
			std::cout << "[processMessage] sleep for 2 seconds\n";
			std::this_thread::sleep_for(std::chrono::seconds(10));
			/* for Read Callback */
			this->message = message;
			async_lock_.unlock();
			return true;
		}
		std::thread async_thrd_;
		std::mutex async_lock_;
		int message;
};

int main() {
	WininetManager* manager = new WininetManager();
	manager->setMessage(2);
	std::cout << "READ CALLBACK call\n";
	manager->getMessage();
	return 0;
}
