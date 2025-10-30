#include "vars.h"
#include "serial_write_commands.h"
#include "serial_set_cmd.h"

class MySerialCheck : private serial_port_write, private serial_set_cmd
{
public:
    void loop()
    {
        // Lê o input da serial
        String input = myserial.check_serial();
        check_read_state();

        if (input.length() > 0)
        {
            // Se for um set_cmd, envia tudo de uma vez
            if (input.startsWith("#set_cmd:"))
            {
                check_commands(input);
                return; // não continua separando
            }

            int startIndex = 0;
            int separatorIndex = input.indexOf('|');

            // Loop para processar todos os comandos separados por '|'
            while (separatorIndex != -1)
            {
                String cmd = input.substring(startIndex, separatorIndex);
                check_commands(cmd); // Processa o comando
                startIndex = separatorIndex + 1;
                separatorIndex = input.indexOf('|', startIndex);
            }

            // Processa o último comando (ou o único, se não houver '|')
            String lastCmd = input.substring(startIndex);
            if (lastCmd.length() > 0)
            {
                check_commands(lastCmd);
            }
        }
    }

    void check_read_state()
    {
        static bool last_read = false;
        if (last_read == read_on)
            return;
        last_read = read_on;
        myserial.write("#READ:" + String(read_on ? "ON" : "OFF"));
    }

    void check_commands(String cmd)
    {
        if (cmd == "#ping" || cmd == "ping")
        {
            myserial.write("#PONG");
        }
        else if (cmd == "#read:on" || cmd == "readtag on")
        {
            read_on = true;
        }
        else if (cmd == "#read:off" || cmd == "readtag off")
        {
            read_on = false;
        }

        else if (cmd == "#get_tags")
        {
            tag_commands.tag_data_display();
        }

        else if (cmd == "#get_tags_all")
        {
            tag_commands.tag_data_display_all();
        }

        else if (cmd == "#clear")
        {
            tag_commands.clear_tags();
        }

        else if (cmd.startsWith("#write:"))
        {
            write_tag_cmd(cmd);
        }

        else if (cmd.startsWith("#password"))
        {
            reader_module.change_password(cmd);
        }

        else if (cmd.startsWith("#set_cmd:"))
        {
            cmd.replace("#set_cmd:", "");
            serial_set_all_cmd(cmd);
            reader_module.setup_reader();
        }

        else if (cmd == "#get_state")
        {
            myserial.write(read_on ? "#READING" : "#IDLE");
        }

        else if (cmd == "#get_power")
        {
            myserial.write("#POWER:" + String(antena[0].power));
        }

        else if (cmd == "#get_session")
        {
            myserial.write("#SESSION:" + String(session));
        }

        else if (cmd.startsWith("#hotspot:"))
        {
            hotspot_on = cmd.endsWith("on");
            reader_module.setup_reader();
        }

        else if (cmd == "#restart")
        {
            ESP.restart();
        }

        else if (cmd == "#get_serial")
        {
            myserial.write("#SERIAL:" + get_esp_name());
        }

        else if (cmd.startsWith("readmode"))
        {
            keyboard = cmd.endsWith("hid");
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("readpower:"))
        {
            cmd.replace("readpower ", "");
            antena_commands.set_power_all(cmd.toInt());
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("gen2session:"))
        {
            String lastCharStr = cmd.substring(cmd.length() - 1);

            session = lastCharStr.toInt();
            if (session > max_session)
                session = 0x00;
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("initreadtag:"))
        {
            start_reading = cmd.endsWith("on");
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("#session:"))
        {
            cmd.replace("#session:", "");
            session = cmd.toInt();
            if (session > max_session)
                session = 0x00;
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("#read_power:"))
        {
            cmd.replace("#read_power:", "");
            antena_commands.set_power_all(cmd.toInt());
            reader_module.setup_reader();
        }

        else if (cmd.startsWith("#buzzer:"))
        {
            cmd.replace("#buzzer:", "");
            buzzer_on = cmd.endsWith("on");
            myserial.write(buzzer_on ? "#BUZZER:ON" : "#BUZZER:OFF");
        }

        else if (cmd.startsWith("#decode_gtin:"))
        {
            decode_gtin = cmd.endsWith("on");
        }

        else if (cmd == "#get_info")
        {
            myserial.write("#NAME:" + get_esp_name());
            myserial.write("#BT_MAC:" + get_bt_mac());
            myserial.write("#ETH_MAC:" + String(ETH.macAddress()));
            myserial.write("#IP:" + ETH.localIP().toString());
        }

        else
        {
            myserial.write("#INVALID_CMD");
        }
    }
};
