
#include <iostream>
#include <fstream>
#include <vector> 

#include "activation_xml.h"
#include "third_party/rapidxml/rapidxml.hpp"


namespace actctx {

	static const std::string kAssemblyIdentity = "assemblyIdentity";
	static int kAssemCnt = 1;

	ManifestParser::ManifestParser(const std::string& file_path) : file_path_(file_path) {
		bool ret = load();
		if (!ret) {
		}
	}

	void ManifestParser::print() {
	}

	bool ManifestParser::load() {
		std::ifstream stream(file_path_);
		std::vector<char> buffer(
				(std::istreambuf_iterator<char>(stream)),
				 std::istreambuf_iterator<char>());
		buffer.emplace_back('\0');

		auto doc = new rapidxml::xml_document<char>();
		doc->parse<0>(const_cast<char*>(buffer.data()));
		doc_ = doc;

		parseData();

		return false;
	}

	void ManifestParser::parseData() {
		walk(doc_->first_node());

#if 0
		for (auto item = doc_->first_node(); item; item = item->next_sibling()) {

			auto name = item->name();

			for (auto subltem = item->first_node(); subltem; subltem = subltem->next_sibling()) {
				auto key = subltem->name();
				auto val = subltem->value();
			}
		}

		auto assem_node = doc_->first_node(kAssemblyIdentity.c_str());
		if (assem_node == nullptr) {
			return;
		}
#endif
	}

	void ManifestParser::walk(const rapidxml::xml_node<>* node, int indent) {
		const auto ind = std::string(indent * 4, ' ');
//		printf("%s", ind.c_str());

		const rapidxml::node_type t = node->type();
		switch(t) {
			case rapidxml::node_element:
				{
					if (kAssemblyIdentity.compare(node->name()) == 0) {
						std::cout << "Find AssemblyIdentity (" << kAssemCnt << ")" << std::endl;
						++ kAssemCnt;
//					printf("<%.*s", node->name_size(), node->name());
					for(const rapidxml::xml_attribute<>* a = node->first_attribute()
							; a
							; a = a->next_attribute()
					   ) {
						std::cout << "\t" << a->name() << " : " << a->value() << std::endl;
//						printf(" %.*s", a->name_size(), a->name());
//						printf("='%.*s'", a->value_size(), a->value());
					}
//					printf(">\n");
					}

					for(const rapidxml::xml_node<>* n = node->first_node()
							; n
							; n = n->next_sibling()
					   ) {
						walk(n, indent+1);
					}
//					printf("%s</%.*s>\n", ind.c_str(), node->name_size(), node->name());
				}
				break;

			case rapidxml::node_data:
//					std::cout << "node_data" << std::endl;
//				printf("DATA:[%.*s]\n", node->value_size(), node->value());
				break;

			default:
//				printf("NODE-TYPE:%d\n", t);
				break;
		}
	}


}

int main() {
	using namespace actctx;

	std::string file = "manifests/test2.manifest";
	ManifestParser parser(file);
	parser.print();

	return 0;
}
