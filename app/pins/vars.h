#define in_1_pin 4
#define in_2_pin 5
#define in_3_pin 6

bool in_1 = false;
bool in_2 = false;
bool in_3 = false;

extern bool gpi_start;
extern bool read_on;

extern int gpi_stop_delay;

extern const int ant_qtd;

const int gpo_qtd = 3;
const int gpo_pin[gpo_qtd] = {7, 16, 18};
bool gpo[gpo_qtd] = {false, false, false};

extern MySerial myserial;

#define buzzer_pin 10
