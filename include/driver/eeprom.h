
#define EEPROM_ADDRESS 0xA0
#define EEPROM_STATUS 0x01
#define EEPROM_HOUR_ON 0x02
#define EEPROM_MINUTE_ON 0x03
#define EEPROM_HOUR_OFF  0x04
#define EEPROM_MINUTE_OFF 0x05
#define EEPROM_LAN_TYPE 0x06
#define EEPROM_WAN_SSID 0x20
#define EEPROM_WAN_PWD 0x40
#define EEPROM_LAN_SSID 0x60
#define EEPROM_LAN_PWD 0x80
#define EEPROM_PROGRAMER 0xA0
#define EEPROM_PAGE 32


//#define EEPROM_CRYPTO 0xAA
#define EEPROM_MQTT_SERVER 0x0100

//#define EEPROM_MQTT_PORT 0xF0
//#define EEPROM_MQTT_HOST 0xF8  //Len of host 32
//#define EEPROM_MQTT_USER 0x118 //Len of user 16
//#define EEPROM_MQTT_PASSWORD 0x128 //Len  of password
//#define EEPROM_MQTT_TOPIC_READ 0x138 //Len of Topic read 32
//#define EEPROM_MQTT_TOPIC_WRITE 0x158 //Len of Topic Write 32

//next free 0x178







u8 eepromReadByte(u16 address);
bool eepromReadData(u16 address, char* data);
bool eepromWriteData(u16 address, char *data);
bool eepromWriteByte(u16 address, u8 data);
bool eeprom_readStaus();
//bool eeprom_readTimer();
bool eeprom_init();
bool eeprom_saveStatus();
bool eeprom_saveTimer();
bool eeprom_readProgramer();
bool eeprom_writeProgramer(u8 number);
bool eeprom_initProgramer();
//bool eeprom_writePASSWORD (char *password, u16 address);
//bool eeprom_writeSSID(char *ssid, u16 address);
void eeprom_readAray(u16 start,u16 len);
mqttserver eeprom_readMqttServer();
bool eeprom_initMqttServer(mqttserver mqtt_server);
bool eeprom_writeMqttServer(mqttserver wServer);
//u8 checkPageBytes(void);
