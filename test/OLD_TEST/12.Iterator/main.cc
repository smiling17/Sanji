#include <vector>
#include <string>
#include <iostream>
#include <memory>

struct info {
	std::string myname;
	int age;
}

class MyInfo {
	public:
		refreshInfo();
		setInfo(std::shared_ptr<struct Info> info);

	private:
		std::vector<std::shared_ptr<struct Info>> info_list_;
}
