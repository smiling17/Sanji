#include <iostream>
#include <thread>

#include <unistd.h>

class MyInfo {
	public:
		MyInfo() {}
		~MyInfo() { this->waitTimer(); }
		void startTimer(int time) {
			fprintf(stdout, "TIMER를 시작합니다.\n");
			for (; time != 0; --time) {
				fprintf(stdout, "%d초 남았습니다.\n", time);
				this->time = time;
				sleep(1);
			}
			fprintf(stdout, "일이 끝났습니다.\n");
			return;
		}
		void testTimer () {
			fprintf(stdout, "testTimer를 시작합니다. \n");
			trd_ = std::thread( [=] {startTimer(3); });
			std::cout << "thread ID : " << trd_.get_id() << std::endl;
//			trd_.detach();
		}
		void waitTimer () {
			this->trd_.join();
		}
		void testOneMore() {
			if (this->trd_.joinable()) {
				fprintf(stdout, "\njoin하기를 기다립니다. \n");
				this->trd_.join();
			}
			fprintf(stdout, "\njoin이 끝났습니다. 새로운 일을 시작합니다. \n");
			trd_ = std::thread( [=] {startTimer(10); });
			std::cout << "thread ID : " << trd_.get_id() << std::endl;
		}
		int getTime() { return this->time; }
		
	private:
		std::thread trd_;
		int time;
};

int main() {
	MyInfo* info_ptr = new MyInfo();
	info_ptr->testTimer();
	info_ptr->testOneMore();
	int time = 5;
	fprintf(stdout, "%d초 후에 프로그램을 종료합니다.\n", time);
	sleep(time);
	fprintf(stdout, "남은시간 : %d\n", info_ptr->getTime());
	fprintf(stdout, "이제 종료합니다. 사요나라\n");
	return 0;
}
