// ==================== Antena ====================
const bool one_ant = true;

const int ant_qtd = 4;
ANTENA antena[ant_qtd];
ANTENA_COMMANDS antena_commands;
const byte min_power = 10;
const byte max_power = 33;
const byte min_rssi = 40;
byte write_power = max_power;

// ==================== Tags ====================
const int max_tags = 1000;
TAG tags[max_tags];
int current_tag = 0;
TAG_COMMANDS tag_commands;

// ==================== Serial ====================
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
bool ignore_read = false;
bool always_send = false;

// ==================== Últimos pacotes ====================
LAST_PACKS last_packs;
const int max_packs = 10;
String last_packs_read[max_packs];

// ==================== Globais gerais ====================
byte temperatura = 0;
int gpi_stop_delay = 0;
bool hotspot_on = true;
bool buzzer_on = false;
bool decode_gtin = true;

// ==================== Watchdog ====================
#define WATCHDOG_TIMEOUT 10
