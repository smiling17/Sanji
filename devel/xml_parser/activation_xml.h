#ifndef _TEST_ACTIVATION_XML_H_
#define _TEST_ACTIVATION_XML_H_

#include <list>

#include "third_party/rapidxml/rapidxml.hpp"

namespace actctx {

	class ManifestParser {
		public:
			ManifestParser(const std::string& file_path);
			void print();

		private:
			struct Assembly {
				std::string id_type;
				std::string name;
				std::string lang;
				std::string arch;
				std::string token;
				std::string version;
			};

			std::list<Assembly> assem_list_;
			std::string file_path_;
			rapidxml::xml_document<char> *doc_;

		private:
			bool load();
			void parseData();
			void walk(const rapidxml::xml_node<>* node, int indent = 0);
	};
} /* namespace actctx */


#endif /* #ifndef _TEST_ACTIVATION_XML_H_ */
