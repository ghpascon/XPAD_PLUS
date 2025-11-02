// FreeRTOS semaphore type
#include <freertos/semphr.h>

// ==================== Antena ====================
const bool one_ant = true;

const int ant_qtd = 4;
ANTENA antena[ant_qtd];
ANTENA_COMMANDS antena_commands;
const byte min_power = 10;
const byte max_power = 33;
const byte min_rssi = 40;
byte write_power = 28;

// ==================== Tags ====================
const int max_tags = 300;
TAG tags[max_tags];
int current_tag = 0;
TAG_COMMANDS tag_commands;

// ==================== Serial ====================
CONNECTION connection;
MySerial myserial;
MySerialCheck myserialcheck;
bool simple_send = false;
bool keyboard = false;

// ==================== Pinos ====================
PINS pins;

// ==================== Leitor ====================
READER reader_module;

// ==================== Servidor Web ====================
WEB_SERVER web_server;

// ==================== Configurações ====================
CONFIG_FILE config_file_commands;

// ==================== LEDs RGB ====================
LED_RGB rgb;

// ==================== Leitura ====================
bool read_on = false;
byte session = 0x00;
byte max_session = 0x03;

// ==================== Modos ====================
bool start_reading = false;
bool gpi_start = false;
bool always_send = false;

// ==================== Globais gerais ====================
byte temperatura = 0;
int gpi_stop_delay = 0;
bool hotspot_on = true;
bool buzzer_on = false;
bool decode_gtin = true;
bool fs_loaded = true;

// ==================== Watchdog ====================
#define WATCHDOG_TIMEOUT 10

bool btConnected = false;
bool eth_connected = false;
// Mutex para proteger acesso ao array `tags`
SemaphoreHandle_t tags_mutex = nullptr;

// ==================== Ethernet Configuration ====================
bool dhcp_on = false;
String static_ip = "192.168.1.101";
String gateway_ip = "192.168.1.1";
String subnet_mask = "255.255.255.0";