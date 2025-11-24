class serial_set_cmd
{
public:
    // Recebe vários comandos separados por "|"
    void serial_set_all_cmd(String cmd)
    {
        const String sep = "|";
        while (cmd.length() > 0)
        {
            int idx = cmd.indexOf(sep);
            String current_cmd = (idx == -1) ? cmd : cmd.substring(0, idx);
            current_cmd.trim();

            if (current_cmd.length() > 0)
                set_cmd_handler(current_cmd);

            if (idx == -1)
                break; // acabou
            cmd = cmd.substring(idx + 1);
        }
    }

private:
    // Trata comandos booleanos genéricos
    void handle_boolean_command(const String &cmd, const String &prefix, bool &variable, bool &matched)
    {
        if (cmd.startsWith(prefix))
        {
            variable = cmd.endsWith("on");
            matched = true;
        }
    }

    // Trata um comando individual
    void set_cmd_handler(String cmd)
    {
        bool cmd_ok = true;

        myserial.write("#CMD:" + cmd); // log/debug

        if (cmd.startsWith("set_ant:"))
        {
            cmd.replace("set_ant:", "");
            serial_set_ant(cmd);
        }
        else if (cmd.startsWith("session:"))
        {
            cmd.replace("session:", "");
            session = cmd.toInt();
            if (session > max_session)
                session = 0x00;
        }
        else if (cmd.startsWith("read_power:"))
        {
            cmd.replace("read_power:", "");
            antena_commands.set_power_all(cmd.toInt());
        }
        else if (cmd.startsWith("buzzer:"))
        {
            buzzer_on = cmd.endsWith("on");
        }
        else if (cmd.startsWith("gpi_stop_delay:"))
        {
            gpi_stop_delay = cmd.substring(strlen("gpi_stop_delay:")).toInt();
        }
        else if (cmd.startsWith("decode_gtin:"))
        {
            decode_gtin = cmd.endsWith("on");
        }
        else
        {
            // Tenta tratar como comando booleano
            bool matched = false;
            handle_boolean_command(cmd, "start_reading:", start_reading, matched);
            handle_boolean_command(cmd, "gpi_start:", gpi_start, matched);
            handle_boolean_command(cmd, "always_send:", always_send, matched);
            handle_boolean_command(cmd, "simple_send:", simple_send, matched);
            handle_boolean_command(cmd, "keyboard:", keyboard, matched);

            if (!matched)
                cmd_ok = false;
        }

        myserial.write(cmd_ok ? "#SET_CMD:OK" : "#SET_CMD:NOK");
    }

    // Configura antena a partir de string "n,active,power,rssi"
    void serial_set_ant(String cmd)
    {
        const String sep = ",";
        int idx = cmd.indexOf(sep);
        if (idx == -1)
            return;

        int current_ant = cmd.substring(0, idx).toInt();
        cmd = cmd.substring(idx + 1);

        idx = cmd.indexOf(sep);
        if (idx == -1)
            return;
        String current_active = cmd.substring(0, idx);
        cmd = cmd.substring(idx + 1);

        idx = cmd.indexOf(sep);
        if (idx == -1)
            return;
        byte current_power = cmd.substring(0, idx).toInt();
        cmd = cmd.substring(idx + 1);

        byte current_rssi = cmd.toInt();

        current_power = constrain(current_power, min_power, max_power);
        current_rssi = max(current_rssi, min_rssi);

        antena_commands.set_antena(
            current_ant,
            (current_active == "on"),
            current_power,
            current_rssi);
    }
};
