#include <iostream>
#include <string>

unsigned int stringToInt(const char *netmask) {
	std::string netmask_str(netmask);
	int comma = -1, addr_part = -1;
	unsigned int addr = 0;
	while ((comma = netmask_str.find_first_of(".")) != std::string::npos) {
		fprintf(stdout, "COMMA find : %d\n", comma);
		addr <<= 8;
		addr_part = std::stoi(netmask_str.substr(0, comma));
		addr |= addr_part;
		netmask_str = netmask_str.substr(comma+1);
	}
	fprintf(stdout, "COMMA END\n");
	addr = addr << 8;
	addr_part = std::stoi(netmask_str);
	addr |= addr_part;

	return addr;
}

bool isValidNetmask(unsigned int netmask) {
	bool mask_start = false;
	int count = 0;
	do {
		++count;
		if (netmask & 1) {
			if (!mask_start) {
				mask_start = true;
				continue;
			}
		}
		else {
			if (!mask_start)
				continue;
			else {
				return false;
			}
		}
	}
	while ((netmask >>= 1) != 0);
	fprintf(stdout, "count : %d\n", count);
	return true;
}

int main () {
	char netmask[16] = {0};
	fprintf(stdout, "유효 넷마스크에 대한 값을 입력하시오\n");
	scanf("%s", netmask);
	fprintf(stdout, "INPUT : %s\n", netmask);
	unsigned int ret = stringToInt(netmask);
	fprintf(stdout, "OUTPUT : %u\n", ret);
	fprintf(stdout, "OUTPUT(16진수) : %x\n", ret);
	bool valid = isValidNetmask(ret);
	if (valid) {
		fprintf(stdout, "유효한 넷마스크\n");
	}
	else {
		fprintf(stdout, "Invalid Netmask\n");
	}
	return 0;
}
