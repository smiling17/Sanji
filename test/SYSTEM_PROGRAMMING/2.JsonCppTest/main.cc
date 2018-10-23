#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>

static const char* req_msg_list[5] = {
  "link_changed",
  "ipaddr_chaged",
  "router_changed",
  "wifi_changed",
  "net_status_changed"
};

int main() {
  Json::Value req_msg;
  Json::StyledWriter writer;
  /* Write (client -> server */
  req_msg["type"] = "register";
  for (int i = 0; i < sizeof(req_msg_list)/sizeof(char*); ++i) {
    req_msg["event_group"] = req_msg_list[i];
    std::string req_msg_output = writer.write(req_msg);
    std::cout << "JSON SEND MSG(STRING) : " << req_msg_output << std::endl;
    fprintf(stdout, "JON SEND MSG(CHAR) : %s\n", req_msg_output.c_str());
    std::cout << "==================================================" << std::endl;
  }
  return 0;
}
