#include <Preferences.h>

Preferences prefs;

extern const bool one_ant;
extern const int ant_qtd;
extern ANTENA_COMMANDS antena_commands;
extern ANTENA antena[];

extern byte session;
extern byte max_session;

extern bool read_on;
extern bool start_reading;
extern bool gpi_start;
extern int gpi_stop_delay;
extern bool ignore_read;
extern bool always_send;

extern const byte min_power;
extern const byte max_power;
extern const byte min_rssi;
extern bool simple_send;
extern bool hotspot_on;
extern bool keyboard;
extern bool buzzer_on;
extern bool decode_gtin;

void save_config()
{
    const int save_time = 10000; // intervalo mínimo entre salvamentos (ms)
    static unsigned long last_save_time = 0;
    static String old_config = "";
    static bool first_time = true;

    if ((millis() - last_save_time) < save_time)
        return;

    last_save_time = millis();

    // === Monta string só para comparar ===
    String new_config = "";
    for (int i = 0; i < ant_qtd; i++)
    {
        new_config += String(antena[i].active) + ",";
        new_config += String(antena[i].power) + ",";
        new_config += String(antena[i].rssi) + ";";
    }

    new_config += String(session) + ",";
    new_config += String(start_reading) + ",";
    new_config += String(gpi_start) + ",";
    new_config += String(gpi_stop_delay) + ",";
    new_config += String(ignore_read) + ",";
    new_config += String(always_send) + ",";
    new_config += String(simple_send) + ",";
    new_config += String(hotspot_on) + ",";
    new_config += String(keyboard) + ",";
    new_config += String(buzzer_on) + ",";
    new_config += String(decode_gtin);

    if (first_time)
    {
        old_config = new_config;
        first_time = false;
        return;
    }

    if (new_config == old_config)
        return;

    old_config = new_config;

    // === Salva de fato, variável por variável ===
    prefs.begin("config", false);

    // Antenas
    for (int i = 0; i < ant_qtd; i++)
    {
        String key_prefix = "ant" + String(i);
        prefs.putBool((key_prefix + "_act").c_str(), antena[i].active);
        prefs.putUChar((key_prefix + "_pwr").c_str(), antena[i].power);
        prefs.putUChar((key_prefix + "_rssi").c_str(), antena[i].rssi);
    }

    // Demais parâmetros
    prefs.putUChar("session", session);
    prefs.putBool("start_reading", start_reading);
    prefs.putBool("gpi_start", gpi_start);
    prefs.putInt("gpi_stop_delay", gpi_stop_delay);
    prefs.putBool("ignore_read", ignore_read);
    prefs.putBool("always_send", always_send);
    prefs.putBool("simple_send", simple_send);
    prefs.putBool("hotspot_on", hotspot_on);
    prefs.putBool("keyboard", keyboard);
    prefs.putBool("buzzer_on", buzzer_on);
    prefs.putBool("decode_gtin", decode_gtin);

    prefs.end();
}

void load_config()
{
    prefs.begin("config", true);

    // Antenas
    for (int i = 0; i < ant_qtd; i++)
    {
        String key_prefix = "ant" + String(i);

        bool active = prefs.getBool((key_prefix + "_act").c_str(), antena[i].active);
        byte power = prefs.getUChar((key_prefix + "_pwr").c_str(), antena[i].power);
        byte rssi = prefs.getUChar((key_prefix + "_rssi").c_str(), antena[i].rssi);

        power = constrain(power, min_power, max_power);
        rssi = max(rssi, min_rssi);

        if (one_ant)
            active = true;

        antena_commands.set_antena(i + 1, active, power, rssi);
    }

    session = prefs.getUChar("session", session);
    start_reading = prefs.getBool("start_reading", start_reading);
    gpi_start = prefs.getBool("gpi_start", gpi_start);
    gpi_stop_delay = prefs.getInt("gpi_stop_delay", gpi_stop_delay);
    ignore_read = prefs.getBool("ignore_read", ignore_read);
    always_send = prefs.getBool("always_send", always_send);
    simple_send = prefs.getBool("simple_send", simple_send);
    hotspot_on = prefs.getBool("hotspot_on", hotspot_on);
    keyboard = prefs.getBool("keyboard", keyboard);
    buzzer_on = prefs.getBool("buzzer_on", buzzer_on);
    decode_gtin = prefs.getBool("decode_gtin", decode_gtin);

    prefs.end();

    // Mantém consistência com sistema
    read_on = start_reading;
}
