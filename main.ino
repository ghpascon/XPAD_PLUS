#include "libs.h"
#include "vars.h"

// ==================== Setup ====================
void setup()
{
  // Inicializa o sistema de arquivos
  if (!LittleFS.begin())
  {
    Serial.println("Erro ao iniciar LittleFS!");
    return;
  }

  // Configura o Watchdog para ambos os núcleos
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = WATCHDOG_TIMEOUT * 1000,
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
      .trigger_panic = true};
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);

  // Carrega configurações
  config_file_commands.get_config();

  // Inicializa módulos
  myserial.setup();
  web_server.setup();
  tag_commands.clear_tags();
  last_packs.clear();
  rgb.setup();
  pins.setup();

  // Pausa para estabilidade
  delay(500);
}

// ==================== Loop ====================
void loop()
{
  // Reseta o Watchdog
  esp_task_wdt_reset();

  // Processamento de serial
  myserial.loop();

  // Verifica entradas
  pins.check_inputs();

  // Processa módulos de leitura e LEDs
  reader_module.functions();
  myserialcheck.loop();
  rgb.state();

  // Atualiza saídas
  pins.set_outputs();

  // verifica a config
  config_file_commands.save_config();
}
