#include "ipt_parser.h"

int main () {
	const char *cmd = "iptables -t filter -n -L";
	FILE* fp = popen(cmd, "r");
	std::string output;
	char buf[256];
	while (fgets(buf, sizeof(buf), fp) != nullptr) {
		output += buf;
		output += '\n';
	}
	pclose(fp);
	IptablesParser parser(output);
	parser.parse();

	return 0;
}

