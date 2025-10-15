#include "vars.h"

class TAG_COMMANDS
{
public:
	void add_tag(String current_epc, String current_tid, int current_ant, int current_rssi)
	{
		if (!read_on)
			return;

		if (antena[current_ant - 1].rssi < current_rssi)
			return;

		if (ignore_read && last_packs_read[0].indexOf(current_epc) != -1)
			return;

		if (always_send)
			display_current_tag(String(current_epc), String(current_tid), String(current_ant), String(current_rssi));

		for (int i = 0; i < max_tags; i++)
		{
			if (current_epc == tags[i].epc || current_tid == tags[i].tid)
				return;
			if (tags[i].epc == "" || tags[i].tid == "")
				break;
		}

		tags[current_tag].epc = current_epc;
		tags[current_tag].tid = current_tid;
		tags[current_tag].ant_number = current_ant;
		tags[current_tag].rssi = current_rssi;

		tags_table += "[\"" + String(current_tag + 1) + "\"" + "," + "\"" + String((current_epc.substring(0, current_epc.length() / 2)) + "<br>" + (current_epc.substring(current_epc.length() / 2))) + "\"" + "," + "\"" + String((current_tid.substring(0, current_tid.length() / 2)) + "<br>" + (current_tid.substring(current_tid.length() / 2))) + "\"" + "," + "\"" + String(current_ant) + "\"" + "," + "\"" + String(current_rssi) + "\"],";

		if (!always_send)
			display_current_tag(String(tags[current_tag].epc), String(tags[current_tag].tid), String(tags[current_tag].ant_number), String(tags[current_tag].rssi));

		current_tag_num();

		if (buzzer_on)
			pins.buzzer_time = millis();
	}

	void tag_data_display()
	{
		String tags_to_send = "#tags:";
		for (int i = 0; i < max_tags; i++)
		{
			if (tags[i].epc == "")
				break;
			tags_to_send += "@" + tags[i].epc;
		}
		myserial.write(tags_to_send);

		clear_tags();
	}

	void tag_data_display_all()
	{
		String tags_to_send = "#tags:";
		for (int i = 0; i < max_tags; i++)
		{
			if (tags[i].epc == "")
				break;
			tags_to_send += "@" + tags[i].epc + "|" + tags[i].tid + "|" + tags[i].ant_number + "|" + tags[i].rssi;
		}
		myserial.write(tags_to_send);
		clear_tags();
	}

	void clear_tags()
	{
		for (int i = 0; i < max_tags; i++)
		{
			tags[i].epc = "";
			tags[i].tid = "";
			tags[i].ant_number = 0;
			tags[i].rssi = 0;
		}
		current_tag = 0;
		tags_table = "";
		myserial.write("#TAGS_CLEARED");
	}

private:
	void current_tag_num()
	{
		(current_tag < max_tags - 1) ? current_tag++ : current_tag = 0;
		if (current_tag == 0)
			tags_table = "";
	}

	void display_current_tag(String epc, String tid, String ant, String rssi)
	{
		if (decode_gtin)
			epc = epcToGtin(epc);
		if (epc == "")
			return;
		if (!simple_send)
			myserial.write("#T+@" + epc + "|" + tid + "|" + ant + "|" + rssi);
		else
			myserial.write(epc, true);
	}

	// Calculate GTIN check digit (mod 10)
	int calcCheckDigit(const String &number)
	{
		int sum = 0;
		bool multiplyBy3 = true;
		for (int i = number.length() - 1; i >= 0; i--)
		{
			int n = number[i] - '0';
			sum += multiplyBy3 ? n * 3 : n;
			multiplyBy3 = !multiplyBy3;
		}
		return (10 - (sum % 10)) % 10;
	}

	// Convert EPC (SGTIN-96) → GTIN-14
	String epcToGtin(const String &epcHex)
	{
		if (epcHex.length() != 24)
			return "";

		// Convert hex string → 12 bytes
		uint8_t epc[12];
		for (int i = 0; i < 12; i++)
		{
			String byteStr = epcHex.substring(i * 2, i * 2 + 2);
			epc[i] = (uint8_t)strtoul(byteStr.c_str(), nullptr, 16);
		}

		// ✅ Check header for SGTIN-96 (0x30)
		if (epc[0] != 0x30)
			return "";

		// Build 96-bit string
		String bits = "";
		for (int i = 0; i < 12; i++)
		{
			for (int b = 7; b >= 0; b--)
			{
				bits += ((epc[i] >> b) & 1) ? '1' : '0';
			}
		}

		// Skip header(8) + filter(3) + partition(3) = 14 bits
		int pos = 14;

		// Partition 5 → Company Prefix = 7 digits (24 bits), Item Reference = 6 digits (20 bits)
		int cpBits = 24;
		int irBits = 20;
		int cpDigits = 7;
		int irDigits = 6;

		// Extract company prefix
		String cpBin = bits.substring(pos, pos + cpBits);
		pos += cpBits;
		uint64_t company = strtoull(cpBin.c_str(), nullptr, 2);

		// Extract item reference
		String irBin = bits.substring(pos, pos + irBits);
		uint64_t item = strtoull(irBin.c_str(), nullptr, 2);

		// Pad company + item
		char buf[32];
		snprintf(buf, sizeof(buf), "%0*llu%0*llu",
				 cpDigits, (unsigned long long)company,
				 irDigits, (unsigned long long)item);

		String full = String(buf);

		// Safety: must have correct length
		if (full.length() != (cpDigits + irDigits))
			return "";

		// Split item reference → indicator + remaining
		String indicator = full.substring(cpDigits, cpDigits + 1);
		String itemRest = full.substring(cpDigits + 1);
		String companyStr = full.substring(0, cpDigits);

		// Construct GTIN-13 without check digit
		String base = indicator + companyStr + itemRest;

		// Must be 13 digits
		if (base.length() != 13)
			return "";

		// Reject all zeros
		if (base == "0000000000000")
			return "";

		// Add check digit
		int check = calcCheckDigit(base);
		String gtin = base + String(check);

		// Final validation
		if (gtin.length() != 14)
			return "";
		if (gtin == "00000000000000")
			return "";

		if (gtin[0] == '0')
			gtin = gtin.substring(1);
		return gtin;
	}
};
