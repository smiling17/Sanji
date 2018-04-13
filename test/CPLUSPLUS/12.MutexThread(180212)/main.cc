#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <chrono> /* time */

/* TODO :객체화 */

class TmaxTeam {
	public:
		static void saveMember(const std::string& name, const std::string& team) {
			std::cout << "[SAVE_NAME] SLEEP!!! name : " << name << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "[SAVE_NAME] WAKE UP!!! name : " << name << std::endl;
			std::lock_guard<std::mutex> guard(g_lock);
			//	g_lock.lock();
			g_team_map[name] = team;
			//	g_lock.unlock();
		}
		static std::map<std::string, std::string> getTmaxMap() { return g_team_map; } 
	private:
		static std::map<std::string, std::string> g_team_map;
		static std::mutex g_lock;
};

std::map<std::string, std::string> TmaxTeam::g_team_map;
std::mutex TmaxTeam::g_lock;

int main() {
	std::thread t1(TmaxTeam::saveMember, "sangjun", "sk1-1");
	std::thread t2(TmaxTeam::saveMember, "joongwon", "pk1-1");
	std::cout << "[MAIN] CHECK MEMBER START" << std::endl;
	for (const auto &pair : TmaxTeam::getTmaxMap()) {
		std::cout << pair.first << " : " << pair.second << std::endl;
	}
	std::cout << "[MAIN] CHECK MEMBER END" << std::endl;
	t1.join();
	t2.join();
	return 0;
}
