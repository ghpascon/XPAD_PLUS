// 2025-10-29
#include "pins.h"
#include "helpers.h"
#include "libs.h"
#include "vars.h"
#include <freertos/semphr.h>

// ==================== Setup ====================
void setup()
{
    // Initialize the file system
    if (!LittleFS.begin())
    {
        Serial.println("Error initializing LittleFS!");
        fs_loaded = false;
    }

    // Configure the Watchdog for both cores
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = WATCHDOG_TIMEOUT * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true};
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);

    // Load configuration
    if (fs_loaded)
        config_file_commands.get_config();

    // Initialize modules
    connection.setup();
    myserial.setup();
    web_server.setup();
    tag_commands.clear_tags();

    // Pre-reserve String buffers for tags to reduce heap churn/fragmentation
    // cada EPC/TID esperado tem ~24 chars hex; reservar um pouco mais (32)
    for (int i = 0; i < max_tags; i++)
    {
        tags[i].epc.reserve(24);
        tags[i].tid.reserve(24);
    }
    // criar mutex para proteger accesso a tags[]
    tags_mutex = xSemaphoreCreateMutex();
    rgb.setup();
    pins.setup();
    reader_module.setup();
    // Pause for stability
    delay(500);
}

// ==================== Loop ====================
void loop()
{
    // Reset the Watchdog
    esp_task_wdt_reset();

    // Process serial communication
    myserial.loop();

    // Check inputs
    pins.check_inputs();

    // Process reader and LED modules
    reader_module.functions();
    myserialcheck.loop();
    rgb.state();

    // Update outputs
    pins.set_outputs();

    // Save configuration
    config_file_commands.save_config();
}
