#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

//void Display(void);


#define DEBUG os_printf

struct status_reg_device
{
	unsigned tcp:1;
	unsigned i2c:1;
	unsigned i2creaderror:1;
	unsigned tcpscaned:1;
	unsigned mqtt:1;
	unsigned gotIP:1;
	unsigned mqttOK:1;
	unsigned tcp_sent:1;
	u8 mqttCount;
	unsigned errtimer:1;
}status;


struct status_programmer
{
	unsigned on:1;    //Power On or Off
    unsigned mode:2;  //Mode 0-manual;1-sleep;2-auto
    unsigned wifiLan:1; //On Off local wifi lan esp8266
    unsigned wifiWan:1; //On Off connection to User Wifi
    unsigned wifiServer:1; //parameter for wifi dhcp
} volatile  program;



struct programer
{
	unsigned number:4;
	unsigned mode:2;
	u16      timeOn;
	u16      timeOff;
} volatile programer[10];


volatile u8 seconds;
volatile u8 minutes;
volatile u8 hours;
volatile u8 days;
volatile u8 date;
volatile u8 month;
volatile u8 year;
volatile u8 msbtemp;
volatile u8 lsbtemp;
//volatile u8 on_hour;
//volatile u8 on_minute;
//volatile u8 off_hour;
//volatile u8 off_minute;
char temperature[10];
char lan_ssid[32];
char lan_pwd[16];
u8 lan_type;
char wan_ssid[32];
char wan_pwd[16];
volatile char scan_wifi[200];
volatile u8 programNumberPower;
volatile u8 programNumber;
volatile u8 programMode;
volatile u16 timeOn;
volatile u16 timeOff;
volatile u16 sleep;
volatile u16 currentSleep;
volatile u8 lastState;
volatile float current;
volatile bool clearDisplayFlag;
//bool displayOn;
volatile u8  displayTimer;
#define DISPLAYTIMER 40
u16 displayMode;
u8 selectMenu;
bool changeMenu;
u8  maxSelect;
u8 key;
bool volatile keypush;


volatile u8 error;
//volatile u8 wifi_station_try_connect;

// #define POWER_MUX PERIPHS_IO_MUX_MTDO_U
// #define POWER_FUNC FUNC_GPIO15
// #define POWER_PIN 15
// #define POWER_ON GPIO_OUTPUT_SET(POWER_PIN,1)
// #define POWER_OFF GPIO_OUTPUT_SET(POWER_PIN,0)
#define POWER_ON gpio16_output_set(1)
#define POWER_OFF gpio16_output_set(0)
#define WAITTIME 20000 //10 seconds
#define EXITTIME 1000
#define EXITTIMEUPDATE 5000
#define MAXMENU 3
#define MAXMENULAN 2
#define ERROREXITTIME 10000 //10 seconds


#define MQTT_BUF_SIZE 1024
#define MQTT_KEEPALIVE 120 /*seconds*/
#define MQTT_RECONNECT_TIMEOUT 10 /*seconds*/
#define PROTOCOL_NAMEv31
#define QUEUE_BUFFER_SIZE 2048


typedef struct {
	uint8 ok;
	uint32 mqttPort;
    char mqttHost[32];
    char mqttUser[16];
    char mqttPassword[16];
    char mqttTopicRead[32];
    char mqttTopicWrite[32];
} mqttserver;


#define MQTT_PORT 12137
#define MQTT_HOST "54.75.92.193" //"m20.cloudmqtt.com"
#define MQTT_USER "esp8266"
#define MQTT_PASS "homeL@b"
#define MQTT_CLIENT_ID "1"
#define DEFAULT_SECURITY 0



//#define INFO
#endif
