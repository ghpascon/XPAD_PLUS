class TELNET
{
protected:
    WiFiServer telnetServer{23};
    WiFiClient telnetClient;

public:
    void config_telnet()
    {
        telnetServer.begin();
        telnetServer.setNoDelay(true);
        Serial.println("Telnet server iniciado na porta 23");
    }

    // Aceita novo cliente se necessário
    void maintain_client()
    {
        if (!telnetClient || !telnetClient.connected())
        {
            WiFiClient newClient = telnetServer.available();
            if (newClient)
            {
                if (telnetClient && telnetClient.connected())
                {
                    telnetClient.stop();
                }
                telnetClient = newClient;
                telnetClient.setNoDelay(true);
                Serial.println("Cliente Telnet conectado");
            }
        }
    }

    bool telnet_write(const String &msg, bool newline = true)
    {
        maintain_client();
        if (!telnetClient || !telnetClient.connected())
            return false;

        Serial.println("Telnet Write: " + msg);

        if (newline)
            return telnetClient.println(msg) > 0;
        return telnetClient.print(msg) > 0;
    }

    // Verifica se chegou algo no telnet; lê até '\n' ou até timeout_ms e faz um print no Serial
    String check_telnet(uint32_t timeout_ms = 200)
    {
        maintain_client();

        if (!telnetClient || !telnetClient.connected())
            return "";
        if (!telnetClient.available())
            return "";

        unsigned long start = millis();
        String line;

        while (millis() - start < timeout_ms)
        {
            while (telnetClient.available())
            {
                char c = (char)telnetClient.read();
                if (c == '\r')
                    continue; // ignora CR
                if (c == '\n')
                {
                    // imprime a linha completa quando encontrar newline
                    return line;
                }
                line += c;
                // Limita tamanho para evitar consumo excessivo de memória
            }
        }

        if (line.length() > 0)
        {
            return line;
        }
    }
};
