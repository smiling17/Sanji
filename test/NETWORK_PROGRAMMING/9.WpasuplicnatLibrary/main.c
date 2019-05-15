/**
 * @file    main.c
 * @brief   TODO brief documentation here.
 *
 * @history
 */
#include "wpa_ctrl.h"

int main() {
  fprintf(stdout, "[WPA_CLI] test start\n");

  /* STPE 1 */
  fprintf(stdout, "[WPA_CLI] step1 : connect to supplicant\n");
  struct wpa_ctrl *ctrl;
  const char *wpa_path = "/var/run/wpa_supplicant/wlx18a6f71396be"; /* iface */
  ctrl = wpa_ctrl_open(wpa_path);
  if (!ctrl) {
    fprintf(stdout, "[WPA_CLI} fail to connect wpa_supplicant\n");
    return -1;
  }

  /* STEP 2 */
  fprintf(stdout, "[WPA_CLI] step2 : attach (if possible)\n");
  if (wpa_ctrl_attach(ctrl) == 0) {
  fprintf(stdout, "[WPA_CLI] step2 : failed\n");
    wpa_ctrl_detach(ctrl);
  }

  /* STEP 3 */
  fprintf(stdout, "[WPA_CLI] step3 : request (scan_result)\n");
  //  const char* cmd = "BSS 1"; /* specific info of BSS(idx or bssid) */

  const char* ssid = "이상준lsj";
  const char* bssid = "b2:70:2d:bd:a8:25";
  const char* passwd = "12341234";
  char cmd[1024]={0}; /* input command */
  char buf[2048]={0}; /* output buffer */
  size_t len = sizeof(buf) - 1;
  int idx = 0;

  /* scan */
  snprintf(cmd, sizeof(cmd), "%s", "SCAN");
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  sleep(3);
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* get scan results */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "%s", "SCAN_RESULTS");
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* add_network */
  char index[4]={0};
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(index) - 1;
  snprintf(cmd, sizeof(cmd), "%s", "ADD_NETWORK");
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), index, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* set ssid */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "SET_NETWORK %s ssid \"%s\"", index, ssid);
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* set bssid */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "SET_NETWORK %s bssid %s", index, bssid);
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* set password */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "SET_NETWORK %s psk \"%s\"", index, passwd);
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* set auth */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "SET_NETWORK %s key_mgmt %s", index, "WPA-PSK");
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);

  /* select AP */
  memset(cmd, 0x0, sizeof(cmd));
  memset(buf, 0x0, sizeof(buf));
  len = sizeof(buf) - 1;
  snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %s", index);
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);
  if (strncmp(buf, "FAIL", 4) == 0) {
    /* if command is failed */
    printf("wpa_command: Command failed (FAIL received)\n");
    return -1;
  }
  /* STEP 4 */
  fprintf(stdout, "[WPA_CLI] step4 : connection close\n");
  wpa_ctrl_close(ctrl);
  fprintf(stdout, "[WPA_CLI] test end\n");
  return 0;
}
