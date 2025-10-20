class reader_write_commands : public commands_reader
{
public:
    // Write tag using EPC or TID with optional mask
    void write_tag(byte *new_epc, byte *password, String targetType, byte *mask_bytes = nullptr, byte *current_epc = nullptr)
    {
        targetType == "epc" ? write_tag_epc(mask_bytes, new_epc, password) : write_tag_tid(new_epc, password, mask_bytes);
    }

    // Write TID with mask
    void write_tag_tid(byte *new_epc, byte *password, byte *mask_bytes)
    {
        byte reader_write_command[] = {
            0x28,
            0xff,
            0x03,
            0x06,
            0xff,

            0x01,
            0x02,

            new_epc[0],
            new_epc[1],
            new_epc[2],
            new_epc[3],
            new_epc[4],
            new_epc[5],
            new_epc[6],
            new_epc[7],
            new_epc[8],
            new_epc[9],
            new_epc[10],
            new_epc[11],

            password[0],
            password[1],
            password[2],
            password[3],

            0x02,

            0x00,
            0x00,
            0x60,

            mask_bytes[0],
            mask_bytes[1],
            mask_bytes[2],
            mask_bytes[3],
            mask_bytes[4],
            mask_bytes[5],
            mask_bytes[6],
            mask_bytes[7],
            mask_bytes[8],
            mask_bytes[9],
            mask_bytes[10],
            mask_bytes[11],
        };

        uint16_t crcValue = uiCrc16Cal(reader_write_command, sizeof(reader_write_command));
        byte crc1 = crcValue & 0xFF;
        byte crc2 = (crcValue >> 8) & 0xFF;
        write_bytes(reader_write_command, sizeof(reader_write_command), crc1, crc2);
    }

    // Write EPC
    void write_tag_epc(byte *current_epc, byte *new_epc, byte *password)
    {
        byte reader_write_command[] = {
            0x24,
            0xff,
            0x03,
            0x06,
            0x06,

            current_epc[0], current_epc[1], current_epc[2], current_epc[3],
            current_epc[4], current_epc[5], current_epc[6], current_epc[7],
            current_epc[8], current_epc[9], current_epc[10], current_epc[11],

            0x01,
            0x02,

            new_epc[0], new_epc[1], new_epc[2], new_epc[3],
            new_epc[4], new_epc[5], new_epc[6], new_epc[7],
            new_epc[8], new_epc[9], new_epc[10], new_epc[11],

            password[0], password[1], password[2], password[3]};

        uint16_t crcValue = uiCrc16Cal(reader_write_command, sizeof(reader_write_command));
        byte crc1 = crcValue & 0xFF;
        byte crc2 = (crcValue >> 8) & 0xFF;
        write_bytes(reader_write_command, sizeof(reader_write_command), crc1, crc2);
    }

    // Write EPC without any filter
    void write_tag_no_filter(byte *new_epc, byte *password)
    {
        byte reader_write_command[] = {
            0x15,
            0xff,
            0x04,
            0x06,

            password[0],
            password[1],
            password[2],
            password[3],

            new_epc[0],
            new_epc[1],
            new_epc[2],
            new_epc[3],
            new_epc[4],
            new_epc[5],
            new_epc[6],
            new_epc[7],
            new_epc[8],
            new_epc[9],
            new_epc[10],
            new_epc[11],
        };

        uint16_t crcValue = uiCrc16Cal(reader_write_command, sizeof(reader_write_command));
        byte crc1 = crcValue & 0xFF;
        byte crc2 = (crcValue >> 8) & 0xFF;
        write_bytes(reader_write_command, sizeof(reader_write_command), crc1, crc2);
    }

    // Change password command parser
    void change_password(String cmd)
    {
        myserial.write("Received: " + cmd);
        cmd.replace("#password:", "");

        int idx1 = cmd.indexOf('|');
        int idx2 = cmd.indexOf('|', idx1 + 1);

        if (idx1 == -1 || idx2 == -1)
        {
            myserial.write("Invalid format: Missing separators '|'");
            return;
        }

        String param1 = cmd.substring(0, idx1);
        String param2 = cmd.substring(idx1 + 1, idx2);
        String param3 = cmd.substring(idx2 + 1);

        if (param1.length() != 24 || param2.length() != 8 || param3.length() != 8)
        {
            myserial.write("Invalid parameter length");
            return;
        }

        String allParams[] = {param1, param2, param3};
        for (int i = 0; i < 3; i++)
        {
            for (char c : allParams[i])
            {
                if (!isHexadecimalDigit(c))
                {
                    myserial.write("Invalid hex digit in parameter " + String(i + 1));
                    return;
                }
            }
        }

        // Convert hex strings to byte arrays
        byte password_data[3][12]; // 24 hex chars = 12 bytes
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                String byteStr = allParams[i].substring(j * 2, j * 2 + 2);
                password_data[i][j] = (byte)strtol(byteStr.c_str(), nullptr, 16);
            }
        }
    }
};
