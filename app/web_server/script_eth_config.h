void eth_config_script()
{
    server.on("/eth_config", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/html/eth_config.html", "text/html"); });

    server.on("/get_eth_config", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String json = "{";
        json += "\"dhcp_on\":" + String(dhcp_on ? "true" : "false") + ",";
        json += "\"static_ip\":\"" + static_ip + "\",";
        json += "\"gateway_ip\":\"" + gateway_ip + "\",";
        json += "\"subnet_mask\":\"" + subnet_mask + "\"";
        json += "}";
        request->send(200, "application/json", json); });

    server.on("/save_eth_config", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("dhcp_on", true)) {
            dhcp_on = request->getParam("dhcp_on", true)->value() == "1";
        }
        
        if (!dhcp_on) {
            if (request->hasParam("static_ip", true)) {
                static_ip = request->getParam("static_ip", true)->value();
            }
            if (request->hasParam("gateway_ip", true)) {
                gateway_ip = request->getParam("gateway_ip", true)->value();
            }
            if (request->hasParam("subnet_mask", true)) {
                subnet_mask = request->getParam("subnet_mask", true)->value();
            }
        }
        
        config_file_commands.save_config();
        connection.setup(); // Reinicia a conexão com as novas configurações
        request->redirect("/eth_config"); });

    server.on("/table_eth_att", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String json = "[[\"DHCP\",\"" + String(dhcp_on ? "ON" : "OFF") + "\"],";
        json += "[\"STATIC IP\",\"" + static_ip + "\"],";
        json += "[\"GATEWAY IP\",\"" + gateway_ip + "\"],";
        json += "[\"SUBNET MASK\",\"" + subnet_mask + "\"],";
        if (eth_connected) {
            json += "[\"CONNECTION\",\"CONNECTED\"],";
            IPAddress ip = ETH.localIP();
            json += "[\"CURRENT IP\",\"" + ip.toString() + "\"]";
        } else {
            json += "[\"CONNECTION\",\"DISCONNECTED\"],";
            json += "[\"CURRENT IP\",\"-\"]";
        }
        json += "]";
        request->send(200, "application/json", json); });
}