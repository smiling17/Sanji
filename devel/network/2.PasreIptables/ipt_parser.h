#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include <vector>

#define TFW_DLOG(__svrt) std::cout

class IptablesParser {
	public:
		IptablesParser(const std::string& input) : input_(input) {}
		void parse();
		
		enum class Chain : char {
			Unknown = -1,
			PreRouting,
			PostRouting,
			Input,
			Output,
			Forward,
		};
		enum class Policy : char {
			Unknown = -1,
			Accept,
			Drop,
		};
		enum class Proto : char {
			Unknown = -1,
			Tcp,
			Udp,
			All,
		};
		enum class Target : char {
			Unknown = -1,
			Accept,
			Drop,
			Reject,
			Log,
			Masqurade,
		};
		struct Rule {
			using Port = std::string;
			Target target = Target::Unknown;
			Proto proto = Proto::Unknown;
			std::string ip_src;
			std::string ip_dst;
			Port port_src;
			Port port_dst;
		};
		struct ChainInfo {
			void reset() {
				policy = Policy::Unknown;
				rules.clear();
			}
			using RuleContainer = std::vector<struct Rule>;
			Policy policy = Policy::Unknown;
			RuleContainer rules;
		};
		using TableInfo = std::map<Chain, ChainInfo>;
		static const std::string kAnywhere;

		enum class Extension : char {
			Unknown,
			SrcPort,
			DstPort,
			State,
			ToIp,
			ToPort,
		};


	private:
		using RegexStrIter = std::sregex_iterator;
		enum class RegexIdx : int {
			All = 0,
			Target,
			Protocol,
			Opt,
			Source,
			Destination,
			extentsion,
		};
		static std::map<Extension, std::regex> kRegexExtension;

		static const std::regex kChainPtn;
		static const std::regex kRulePtn;
		static const std::regex kHeaderPtn;

		void parseLine(const std::string& line);
		void parseChainLine(const std::string& line, Chain& chain, ChainInfo& chain_info);
		void parseRuleLine(const std::string& line, struct ChainInfo& chain_info);
		void parseExtensionLine(const std::string& line, struct Rule& rule);
		void handleExtensionLine(const Extension ext, const std::string& param, struct Rule& rule);

		Target str2Target(const std::string& str);
		Proto str2Proto(const std::string& str);
		Chain str2Chain(const std::string& str);
		Policy str2Policy(const std::string& str);

		void print(const TableInfo table);
		const int getRegexIdx(RegexIdx idx) const;

		std::string input_;
};

namespace debug {

using StringContainer = std::vector<std::string>;

struct Chain {
  using Type = IptablesParser::Chain;
  static const StringContainer Strings;
};

const StringContainer Chain::Strings{"PRE_ROUTING", "POST_ROUTING", "INPUT", "OUTPUT", "FORWARD"};

struct Policy {
  using Type = IptablesParser::Policy;
  static const StringContainer Strings;
};

const StringContainer Policy::Strings{"ACCEPT", "DENY"};

struct Proto {
  using Type = IptablesParser::Proto;
  static const StringContainer Strings;
};

const StringContainer Proto::Strings{"TCP", "UDP", "ALL" };

struct Target {
  using Type = IptablesParser::Target;
  static const StringContainer Strings;
};

const StringContainer Target::Strings{"ACCEPT", "DROP", "REJECT",
                                             "LOG", "MASQURADE"};


template <typename T>
const std::string GetString(typename T::Type type) {
  if (type == T::Type::Unknown) {
    return "UNKOWN";
  }
  return T::Strings[static_cast<int>(type)];
}

const std::string GetString(const std::string str) {
  if (str.empty()) {
    return "UNKNOWN";
  }
  return str;
}

} /* namespace debug */



//const std::string IptablesParser::kRegexRuleStr = "(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(.*)";
//const std::string IptablesParser::kRegexChianStr = "\\s*Chain\\s+(\\S+)\\s+\(policy\s+(\\w+)\)";
//const std::string IptablesParser::kRegexCheckChianStr = "\\s*Chain\\s+";

const std::regex IptablesParser::kRulePtn = std::regex("(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(.*)");
const std::regex IptablesParser::kChainPtn = std::regex("^\\s*Chain\\s+(\\S+)\\s+\\(policy\\s+(\\w+)\\)");
const std::regex IptablesParser::kHeaderPtn = std::regex("^\\s*target\\s+prot\\s+opt\\s+source\\s+destination(.*)");

std::map <IptablesParser::Extension, std::regex> IptablesParser::kRegexExtension = {
	{IptablesParser::Extension::SrcPort, std::regex("\\bspts?:(\\S+)") },
	{IptablesParser::Extension::DstPort, std::regex("\\bdpts?:(\\S+)")},
	{IptablesParser::Extension::State, std::regex("\\bstate\\s+\\S+")},
};

const std::string IptablesParser::kAnywhere = "anywhere";

void IptablesParser::parse() {

	std::istringstream stream(input_);
	std::string line;
	TableInfo table_info = {};
	ChainInfo chain_info;
	Chain chain = Chain::Unknown;
	bool chain_found = false;
	while (std::getline(stream, line)) {
		if (line.empty()) {
			continue;
		}
		if (std::regex_match(line, kChainPtn)) {
			if (chain_found) {
				table_info[chain] = chain_info;
				chain_info.reset();
				chain = Chain::Unknown;
			}
			else {
				chain_found = true;
			}
			parseChainLine(line, chain, chain_info);
		}
		else if (std::regex_match(line, kHeaderPtn)) {
			continue;
		}
		else if (std::regex_match(line, kRulePtn)) {
			parseRuleLine(line, chain_info);
		}
	}
	if (chain_found) {
		table_info[chain] = chain_info;
	}
	print(table_info);
	return;
}

const int IptablesParser::getRegexIdx(RegexIdx idx) const {
	return static_cast<int>(idx);
}


void IptablesParser::parseChainLine(const std::string& line, Chain& chain, ChainInfo& chain_info) {
	RegexStrIter iter(line.begin(), line.end(), kChainPtn);
	RegexStrIter end;
	while (iter != end) {
		std::smatch m = *iter;
		chain = str2Chain(m.str(1));
		chain_info.policy = str2Policy(m.str(2));
		++iter;
		break;
	}
}

void IptablesParser::parseRuleLine(const std::string& line, struct ChainInfo& chain_info) {
	RegexStrIter iter(line.begin(), line.end(), kRulePtn);
	RegexStrIter end;
	struct Rule rule;
	while (iter != end) {
		std::smatch m = *iter;
		rule.target = str2Target(m.str(1));
		rule.proto = str2Proto(m.str(2));
//		rule.opt = m.str(3);
		rule.ip_src = m.str(4);
		rule.ip_dst = m.str(5);
		parseExtensionLine(m.str(6), rule);
		++iter;
	}
	chain_info.rules.push_back(rule);
}

void IptablesParser::parseExtensionLine(const std::string& line, struct Rule& rule) {
	std::smatch m;
	for (auto it : kRegexExtension) {
		if (std::regex_search(line, m, it.second)) {
			handleExtensionLine(it.first, m.str(1), rule);
		}
	}
}

void IptablesParser::handleExtensionLine(const Extension ext, const std::string& param, struct Rule& rule) {
	switch (ext) {
		case Extension::SrcPort:
		case Extension::DstPort:
			rule.port_dst = param;
			break;
		case Extension::State:
		case Extension::ToIp:
		default:
			break;
	}
}

void IptablesParser::print(const TableInfo table) {
	std::cout << "Print" << std::endl;
	for (auto it : table) {
		TFW_DLOG(INFO) << "Chain name : " << debug::GetString<debug::Chain>(it.first) << std::endl;
		TFW_DLOG(INFO) << "Policy : " << debug::GetString<debug::Policy>(it.second.policy) << std::endl;
		for (auto& r : it.second.rules) {
			TFW_DLOG(INFO) << "Target : " << debug::GetString<debug::Target>(r.target) << std::endl;
			TFW_DLOG(INFO) << "Proto : " << debug::GetString<debug::Proto>(r.proto) << std::endl;
			TFW_DLOG(INFO) << "IP(src) : " << (r.ip_src) << std::endl;
			TFW_DLOG(INFO) << "IP(dst) : " << (r.ip_dst) << std::endl;
			TFW_DLOG(INFO) << "Port(src) : " << (r.port_src) << std::endl;
			TFW_DLOG(INFO) << "Port(dst) : " << (r.port_dst) << std::endl;
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}
}

IptablesParser::Target IptablesParser::str2Target(const std::string& str) {
	if (str == "ACCEPT") {
		return Target::Accept;
	}
	else if (str == "DROP") {
		return Target::Drop;
	}
	else if (str == "REJECT") {
		return Target::Reject;
	}
	else if (str == "LOG") {
		return Target::Log;
	}
	else if (str == "MASQURADE") {
		return Target::Masqurade;
	}
	else {
		return Target::Unknown;
	}
}

IptablesParser::Proto IptablesParser::str2Proto(const std::string& str) {
	if (str == "tcp") {
		return Proto::Tcp;
	}
	else if (str == "udp") {
		return Proto::Udp;
	}
	else if (str == "all") {
		return Proto::All;
	}
	else {
		return Proto::Unknown;
	}
}

IptablesParser::Chain IptablesParser::str2Chain(const std::string& str) {
	if (str == "PREROUTING") {
		return Chain::PreRouting;
	}
	else if (str == "POSTROUTING") {
		return Chain::PostRouting;
	}
	else if (str == "INPUT") {
		return Chain::Input;
	}
	else if (str == "OUTPUT") {
		return Chain::Output;
	}
	else if (str == "FORWARD") {
		return Chain::Forward;
	}
	else {
		return Chain::Unknown;
	}
}

IptablesParser::Policy IptablesParser::str2Policy(const std::string& str) {
	if (str == "ACCEPT") {
		return Policy::Accept;
	}
	else if (str == "DROP") {
		return Policy::Drop;
	}
	else {
		return Policy::Unknown;
	}
}


