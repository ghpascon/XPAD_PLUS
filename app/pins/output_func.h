class output_func
{
public:
	long buzzer_time = millis();
	void set_outputs()
	{
		set_gpos();
		set_buzzer();
	}

	void set_gpos()
	{
		for (int i = 0; i < gpo_qtd; i++)
			gpo[i]
				? pinMode(gpo_pin[i], INPUT_PULLUP)
				: pinMode(gpo_pin[i], OUTPUT);
	}

	void set_buzzer()
	{
		const int buzzer_time_on = 100;
		digitalWrite(buzzer_pin, (millis() - buzzer_time > buzzer_time_on) ? HIGH : LOW);
	}
};