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

private:
    void change_password_cmd(String cmd)
    {
        if (!cmd.startsWith("#change_password:"))
        {
            myserial.write("#ERROR:Invalid command prefix");
            return;
        }

        String payload = cmd.substring(17); // remove "#change_password:"

        // Split by ';' into max 3 parts: EPC;NEW_PASSWORD;OLD_PASSWORD
        String parts[3];
        int count = 0;
        int start = 0;
        for (int i = 0; i < payload.length() && count < 3; i++)
        {
            if (payload.charAt(i) == ';')
            {
                parts[count++] = payload.substring(start, i);
                start = i + 1;
            }
        }
        parts[count++] = payload.substring(start); // last part

        // Validate number of parts (2 or 3)
        if (count < 2 || count > 3)
        {
            myserial.write("#ERROR:Invalid parameters count");
            return;
        }

        String epc = parts[0];
        String new_password = parts[1];
        String old_password = (count == 3) ? parts[2] : "00000000";

        // Validate EPC (24 hex chars)
        if (epc.length() != 24 || !validateHex(epc, 24))
        {
            myserial.write("#ERROR:Invalid EPC");
            return;
        }

        // Validate new password (8 hex chars)
        if (new_password.length() != 8 || !validateHex(new_password, 8))
        {
            myserial.write("#ERROR:Invalid new password");
            return;
        }

        // Validate old password (8 hex chars)
        if (old_password.length() != 8 || !validateHex(old_password, 8))
        {
            myserial.write("#ERROR:Invalid old password");
            return;
        }

        reader_module.change_password(epc, new_password, old_password);
    }

    void protected_mode_cmd(String cmd)
    {
        if (!cmd.startsWith("#protected_mode:"))
        {
            myserial.write("#ERROR:Invalid command prefix");
            return;
        }

        String payload = cmd.substring(16); // remove "#protected_mode:"

        // Split by ';' into 3 parts: EPC;PASSWORD;ENABLE
        String parts[3];
        int count = 0;
        int start = 0;
        for (int i = 0; i < payload.length() && count < 3; i++)
        {
            if (payload.charAt(i) == ';')
            {
                parts[count++] = payload.substring(start, i);
                start = i + 1;
            }
        }
        parts[count++] = payload.substring(start); // last part

        // Validate number of parts (must be 3)
        if (count != 3)
        {
            myserial.write("#ERROR:Missing parameters");
            return;
        }

        String epc = parts[0];
        String password = parts[1];
        String enable_str = parts[2];
        enable_str.toLowerCase();

        // Validate EPC (24 hex chars)
        if (epc.length() != 24 || !validateHex(epc, 24))
        {
            myserial.write("#ERROR:Invalid EPC");
            return;
        }

        // Validate password (8 hex chars)
        if (password.length() != 8 || !validateHex(password, 8))
        {
            myserial.write("#ERROR:Invalid password");
            return;
        }

        // Validate enable parameter
        if (enable_str != "on" && enable_str != "off" && enable_str != "true" && enable_str != "false")
        {
            myserial.write("#ERROR:Invalid enable parameter");
            return;
        }

        bool enable = (enable_str == "on" || enable_str == "true");
        // mudar a senha antes (usando senha padrão como old_password)
        reader_module.change_password(epc, password);

        reader_module.protected_mode_tag(epc, password, enable);
    }

    void protected_inventory_cmd(String cmd)
    {
        if (!cmd.startsWith("#protected_inventory:"))
        {
            myserial.write("#ERROR:Invalid command prefix");
            return;
        }

        String payload = cmd.substring(21); // remove "#protected_inventory:"

        // Split by ';' into max 2 parts: ENABLE;PASSWORD
        String parts[2];
        int count = 0;
        int start = 0;
        for (int i = 0; i < payload.length() && count < 2; i++)
        {
            if (payload.charAt(i) == ';')
            {
                parts[count++] = payload.substring(start, i);
                start = i + 1;
            }
        }
        parts[count++] = payload.substring(start); // last part

        // Validate number of parts (1 or 2)
        if (count < 1 || count > 2)
        {
            myserial.write("#ERROR:Invalid parameters count");
            return;
        }

        String enable_str = parts[0];
        enable_str.toLowerCase();
        String password = (count == 2) ? parts[1] : "00000000";

        // Validate enable parameter
        if (enable_str != "on" && enable_str != "off" && enable_str != "true" && enable_str != "false")
        {
            myserial.write("#ERROR:Invalid enable parameter");
            return;
        }

        // Password will be validated inside protected_inventory function
        bool enable = (enable_str == "on" || enable_str == "true");

        // Save configuration
        protected_inventory_enabled = enable;
        protected_inventory_password = password;

        // enable + password
        myserial.write(String(enable) + " " + password);

        reader_module.setup_reader();
    }

public:
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

        else if (cmd.startsWith("#change_password:"))
        {
            change_password_cmd(cmd);
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

        // ================= Prefix config =================
        else if (cmd.startsWith("#prefix:"))
        {
            String v = cmd.substring(String("#prefix:").length());
            v.trim();
            // Permite prefix vazio (length >= 0)
            prefix = v;
            config_file_commands.save_config();
            myserial.write("#PREFIX:" + prefix);
        }
        else if (cmd == "#get_prefix")
        {
            myserial.write("#PREFIX:" + prefix);
        }

        else if (cmd.startsWith("#protected_mode:"))
        {
            protected_mode_cmd(cmd);
        }

        else if (cmd.startsWith("#protected_inventory:"))
        {
            protected_inventory_cmd(cmd);
        }

        else if (cmd == "#get_protected_inventory")
        {
            myserial.write("#PROTECTED_INVENTORY:" + String(protected_inventory_enabled ? "ENABLED" : "DISABLED"));
            myserial.write("#PROTECTED_INVENTORY_PASSWORD:" + protected_inventory_password);
        }

        else
        {
            myserial.write("#INVALID_CMD");
        }
    }
};
