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

  const char* cmd = "SCAN_RESULT";
  char buf[2048]={0}; /* output buffer */
  size_t len = sizeof(buf) - 1;
  if (wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL) < 0) {
    printf("wpa_command: wpa_ctrl_request failed\n");
    wpa_ctrl_close(ctrl);
    return -1;
  }
  if (strncmp(buf, "FAIL", 4) == 0) {
    /* if command is failed */
    printf("wpa_command: Command failed (FAIL received)\n");
    return -1;
  }
  buf[len] = '\0'; /* len : actual data size */
  fprintf(stdout, "%s", buf);
  fprintf(stdout, "*****buf length : %d*****\n", len);

  /* STEP 4 */
  fprintf(stdout, "[WPA_CLI] step4 : connection close\n");
  wpa_ctrl_close(ctrl);
  fprintf(stdout, "[WPA_CLI] test end\n");
  return 0;
}
