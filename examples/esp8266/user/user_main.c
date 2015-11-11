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

#define UREQ_ESP8266
#define UREQ_USE_FILESYSTEM

#include "../../../ureq.h"

// 6 works best
// this value was obtained experimentally
#define MAX_CONNS 6

struct HttpConnection {
    HttpRequest r;
    int id;
};

struct HttpConnection conns[MAX_CONNS];

//const char ssid[32] = "ssid";
//const char password[32] = "password";

bool gpioStatus = false;

// Data structure for the configuration of your wireless network.
// Will contain ssid and password for your network.
struct station_config stationConf;

/*
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
*/

void ICACHE_FLASH_ATTR ssRecvCb(void *arg, char *data, unsigned short len) {
    char buf[64];
    int l;
    struct espconn *pespconn = (struct espconn *)arg;

    if (strlen(data) >= 1024) {
        // This is esp8266 specific, it just has too little memory
        // to answer all big requests with 413
        espconn_disconnect(pespconn);
        return;
    }

    HttpRequest r = ureq_init(data);

    ureq_run(&r);

    int i;
    for(i=0; i< MAX_CONNS-1; i++) {
        if (conns[i].id == 0)
            break;
        if (i == MAX_CONNS) {
            conns[0].id == 0;
            i = 0;
            break;
        }
    }

    conns[i].r = r;
    conns[i].id = pespconn->proto.tcp->remote_port;

    espconn_sent(pespconn, r.response.data, r.len);
    os_printf("Request %s from %d.%d.%d.%d. Status: %d. ID: %d.\n",
        r.url,
        pespconn->proto.tcp->remote_ip[0],
        pespconn->proto.tcp->remote_ip[1],
        pespconn->proto.tcp->remote_ip[2],
        pespconn->proto.tcp->remote_ip[3],
        r.response.code,
        conns[i].id);
}

struct HttpConnection *getHttpConnection(void *arg) {
    struct espconn *c = (struct espconn *)arg;
    int i;
    for(i=0; i<MAX_CONNS; i++) {
        if (conns[i].id == c->proto.tcp->remote_port)
            return &conns[i];
    }
    return NULL;
}

void ICACHE_FLASH_ATTR ssSentCb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;
    struct HttpConnection *c = getHttpConnection(pespconn); 

    if (c->r.complete == 1) {
        espconn_disconnect(pespconn);
        return;
    }
    
    ureq_run(&c->r);
    espconn_sent(pespconn, c->r.response.data, c->r.len);
    if (c->r.len < 1024) {
        espconn_disconnect(pespconn);
    }

}

void ICACHE_FLASH_ATTR ssDiscCb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;
    struct HttpConnection *c = getHttpConnection(pespconn);
    if (!c) return;
    ureq_close(&c->r);
    c->id = 0;
}

void ICACHE_FLASH_ATTR ssConnCb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;

    //printf("HEAP SIZE: %d\n", system_get_free_heap_size());

    if (pespconn->link_cnt > MAX_CONNS) {
        espconn_disconnect(pespconn);
        os_printf("Too many connections at once.\n");
        return;
    }

    espconn_regist_recvcb(pespconn, ssRecvCb);
    espconn_regist_sentcb(pespconn, ssSentCb);
    espconn_regist_disconcb(pespconn, ssDiscCb);
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

    //espconn_set_opt(pSimpleServer, ESPCONN_REUSEADDR);

    espconn_regist_connectcb(pSimpleServer, ssConnCb);
    espconn_accept(pSimpleServer);
    espconn_regist_time(pSimpleServer, 0, 0);
}

void ICACHE_FLASH_ATTR wifiInit() {
    // TODO: wifi configuration via http
    //os_memcpy(&stationConf.ssid, ssid, 32);
    //os_memcpy(&stationConf.password, password, 32);
    // STATION_MODE, SOFTAP_MODE or STATIONAP_MODE.
    wifi_set_opmode( STATION_MODE );
    //wifi_set_opmode( STATION_MODE );

    wifi_station_set_config(&stationConf);
}

char *s_home() {
    return "Hello World!";
}

char *s_get(HttpRequest *r) {
    /* This one shows how to handle GET parameters.
     * Please note, that ureq_get_param_value uses
     * common buffer for all operations, so store
     * copy data from it before calling it again */
    char *arg;

    strcpy(r->buffer, "data: ");
    arg = ureq_get_param_value(r, "data");
    strcat(r->buffer, arg);

    strcat(r->buffer, "<br>data2: ");
    arg = ureq_get_param_value(r, "data2");
    strcat(r->buffer, arg);

    return r->buffer;
}

char *s_post(HttpRequest *r) {
    if ( strcmp(POST, r->type) != 0 )
        return  "Try requesting this page with POST method!<br>"
                "Feel free to use this form:<br>"
                "<form method='post'>"
                "Data: <input type='text' name='data'><br>"
                "Data2: <input type='text' name='data2'><br>"
                "<input type='submit' value='Submit'>"
                "</form>";

    char *arg;

    strcpy(r->buffer, "data: ");
    arg = ureq_post_param_value(r, "data");
    strcat(r->buffer, arg);

    strcat(r->buffer, "<br>data2: ");
    arg = ureq_post_param_value(r, "data2");
    strcat(r->buffer, arg);

    return r->buffer;
}

char *s_redirect(HttpRequest *r) {
    r->response.code = 302;
    r->response.header = "Location: /redirected";
    return "";
}

char *s_redirected() {
    return "Redirected from /redirect";
}

void user_init(void) {
    // Uart init
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    os_printf("\nStarting...\n");

    int i;
    for(i=0; i<MAX_CONNS; i++) {
        // c field is used as indicator for free/used connection slots
        conns[i].id = 0;
    }

    // Add page to ureq
    //ureq_serve("/", s_home, "GET");
    ureq_serve("/get", s_get, "GET");
    ureq_serve("/post", s_post, "ALL");
    ureq_serve("/redirect", s_redirect, "GET");
    ureq_serve("/redirected", s_redirected, "GET");

    /*
    // Some gpio-related stuff
    gpio_init();
    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);
    //Set GPIO2 high
    gpio_output_set(BIT0, 0, BIT0, 0);
    */

    // Wifi setup
    wifiInit();
    ssServerInit();
  
    // Print a message that we have completed user_init on debug uart
    os_printf("Ready.\n");
}
