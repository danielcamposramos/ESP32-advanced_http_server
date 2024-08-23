#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "driver/uart.h"
#include "string.h"

#define TXD_PIN 17
#define RXD_PIN 16
#define RX_BUF_SIZE 1024
static char incoming_message[RX_BUF_SIZE];

static const char *TAG = "SERVER";

static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(TAG,"URL: %s",req->uri);
    httpd_resp_send(req, incoming_message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void init_server()
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  ESP_ERROR_CHECK(httpd_start(&server, &config));

  httpd_uri_t default_url = {
    .uri ="/",
    .method = HTTP_GET,
    .handler = on_default_url
  };
  httpd_register_uri_handler(server,&default_url);

}
static void configuracao_uart()
{
  uart_config_t uart_config = {
    .baud_rate = 9600, // 9600 115200
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
  uart_param_config(UART_NUM_1 ,&uart_config);
  uart_set_pin(UART_NUM_1,TXD_PIN,RXD_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_1,RX_BUF_SIZE,0,0,NULL,0);
}

void app_main(void)
{
  configuracao_uart();
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init();
  ESP_ERROR_CHECK(wifi_connect_sta("Your WIFI", "Change This", 10000));

  init_server();
leitura_gps:
  memset(incoming_message, 0 , sizeof(incoming_message));
  uart_read_bytes(UART_NUM_1,(uint8_t *) incoming_message,RX_BUF_SIZE,pdMS_TO_TICKS(500));
  printf("Dados do GPS: %s\n",incoming_message);
goto leitura_gps;
}
