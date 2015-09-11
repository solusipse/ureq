#include <osapi.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
#include <gpio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ESP8266

#include "../../ureq/ureq.c"

//const char ssid[32] = "ssid";
//const char password[32] = "password";

bool gpioStatus = false;

// Data structure for the configuration of your wireless network.
// Will contain ssid and password for your network.
struct station_config stationConf;


void ICACHE_FLASH_ATTR setGpio( int status ) {
    if (status == 0)
        gpio_output_set(BIT0, 0, BIT0, 0);
    else
        gpio_output_set(0, BIT0, BIT0, 0);
}

void ICACHE_FLASH_ATTR reverseGpio() {
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT0) {
        //Set GPIO2 to LOW
        gpio_output_set(0, BIT0, BIT0, 0);
    } else {
        //Set GPIO2 to HIGH
        gpio_output_set(BIT0, 0, BIT0, 0);
    }
}


void ICACHE_FLASH_ATTR ssRecvCb(void *arg, char *data, unsigned short len) {
    char buf[64];
    int l;
    struct espconn *pespconn = (struct espconn *)arg;

    os_printf("Incoming connection\n");

    HttpRequest req;
    ureq_run(&req, data);
    
    os_printf("%s\n", req.response);

    espconn_sent(pespconn, req.response, strlen(req.response));

    ureq_close(&req);
    
    espconn_disconnect(pespconn);
}

void ICACHE_FLASH_ATTR ssConnCb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;

    os_printf("Connection from: %d.%d.%d.%d:%d\n",
        pespconn->proto.tcp->remote_ip[0],
        pespconn->proto.tcp->remote_ip[1],
        pespconn->proto.tcp->remote_ip[2],
        pespconn->proto.tcp->remote_ip[3],
        pespconn->proto.tcp->remote_port);

    espconn_regist_recvcb(pespconn, ssRecvCb);
}

void ICACHE_FLASH_ATTR ssServerInit() {

    struct espconn * pSimpleServer;

    pSimpleServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
    ets_memset( pSimpleServer, 0, sizeof( struct espconn ) );

    espconn_create( pSimpleServer );
    pSimpleServer->type = ESPCONN_TCP;
    pSimpleServer->state = ESPCONN_NONE;
    pSimpleServer->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
    pSimpleServer->proto.tcp->local_port = 80;

    espconn_regist_connectcb(pSimpleServer, ssConnCb);
    espconn_accept(pSimpleServer);
    espconn_regist_time(pSimpleServer, 3600, 0);
}

void ICACHE_FLASH_ATTR wifiInit() {
    // TODO: wifi configuration via http
    //os_memcpy(&stationConf.ssid, ssid, 32);
    //os_memcpy(&stationConf.password, password, 32);

    // STATION_MODE, SOFTAP_MODE or STATIONAP_MODE.
    //wifi_set_opmode( STATION_MODE );
    wifi_set_opmode( SOFTAP_MODE );

    //wifi_station_set_config(&stationConf);
}

char *s_home() {
    return "Hello World!";
}



void user_init(void) {
    // Uart init
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    os_printf("\nStarting...\n");

    // Add page to ureq
    ureq_serve("/", s_home, "GET");

    // Some gpio-related stuff
    gpio_init();
    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);
    //Set GPIO2 high
    gpio_output_set(BIT0, 0, BIT0, 0);

    // Wifi setup
    wifiInit();
    ssServerInit();
  
    // Print a message that we have completed user_init on debug uart
    os_printf("Ready.\n");
}
