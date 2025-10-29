String get_esp_name()
{
    uint64_t chipid = ESP.getEfuseMac();
    char id_str[13];
    sprintf(id_str, "%012llX", chipid);
    return "SMTX-" + String(id_str);
}