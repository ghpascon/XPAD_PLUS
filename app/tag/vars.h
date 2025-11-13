
extern const int max_tags;
extern TAG tags[];
extern int current_tag;

extern const int ant_qtd;
extern ANTENA antena[];

extern PINS pins;

extern bool read_on;
extern bool always_send;

extern MySerial myserial;
extern bool simple_send;
extern bool buzzer_on;
extern bool decode_gtin;
// Mutex para proteger acesso concorrente a `tags[]`
extern SemaphoreHandle_t tags_mutex;

extern String prefix;