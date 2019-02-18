// Includes for Wifi
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <sys/param.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <string.h>
#include <sstream>

#define HOST_IP_ADDR CONFIG_TARGET_IPV4_ADDR
#define PORT CONFIG_TARGET_PORT

static EventGroupHandle_t wifi_event_group;

const int IPV4_GOTIP_BIT = BIT0;

static const char *TAG = "FNET";
// static const char *payload = "Message from ESP32";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  return ESP_OK;
}

void network_init(void){
  // INIT FLASH
  nvs_flash_init();
  // WIFI init
  tcpip_adapter_init();
  esp_event_loop_init(event_handler, NULL);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_STA);

  wifi_config_t sta_config = {};
  strcpy((char*)sta_config.sta.ssid, CONFIG_ESP_WIFI_SSID);
  strcpy((char*)sta_config.sta.password, CONFIG_ESP_WIFI_PASSWORD);
  sta_config.sta.bssid_set = false;

  esp_wifi_set_config(WIFI_IF_STA, &sta_config);
  // Starts ESP WiFi radio
  esp_wifi_start();
  // Connects to the wi-fi
  esp_wifi_connect();
  // Wait for IP
  vTaskDelay(3000/ portTICK_PERIOD_MS);
  // uint32_t bits = IPV4_GOTIP_BIT ;
  // xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
  ESP_LOGI(TAG, "Connected");
}

void network_close(void) {
  esp_wifi_stop();
}

class FSocketClient {

private:
  int fsock;
  std::string FMessage = "Hello There!";
  struct sockaddr_in destAddr;

public:

  void finitialize(void){

    char addr_str[128];
    int addr_family;
    int ip_protocol;

    destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

    fsock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (fsock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        // break;
    }
    ESP_LOGI(TAG, "Socket created");
  }

  void fconnect(void) {
    // Endless attempts to connect
    while (true) {
      int err = connect(fsock, (struct sockaddr *)&destAddr, sizeof(destAddr));
      if (err == 0) {
          ESP_LOGI(TAG, "Successfully connected");
          break;
      }
      ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
      vTaskDelay(2000/ portTICK_PERIOD_MS);
    }
  }

  void fsend(std::string msg){
    char char_msg[msg.length()+1];
    strcpy(char_msg, msg.c_str());
    int err = send(fsock, char_msg, msg.length(), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
    }
  }

  void fclose(void) {
    shutdown(fsock, 0);
    close(fsock);
  }


};
