/*
 Name:		Smart_garden_visual.ino
 Created:	9/13/2020 7:07:33 PM
 Author:	Malthe Holm Sennels. hest
*/

#include <DHT_U.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define ch1 5
#define	ch2	4
#define	ch3	3
#define in1	A0
#define in2	A1
#define in3 A2
#define minfugt	225
#define maxfugt	600
#define en1	6
#define en2 7
#define sw A3
#define EEPROMSIZE 512;
#define DHT_Pin	A6

DHT dht = DHT(DHT_Pin, DHT11);

LiquidCrystal lcd(13, 12, 8, 9, 10, 11);

unsigned long cmddata[100];

bool executed[100];
int Tnow = 0;
int	dagnow = 0;
bool enstate;
bool enlast;
bool state2;
String menu1[] = { "Alarmer", "Data", "indstillinger", "test 1", "test 2", "test 3" };
int8_t cursor = 0;
void setup() {
	lcd.begin(16, 2);
	Serial.begin(9600);
	Serial.println("progam start");
	//lcd.print("smartgarden");
	lcd.setCursor(0, 1);
	//lcd.print("V. 0.91");
	pinMode(ch1, OUTPUT);
	pinMode(ch2, OUTPUT);
	pinMode(ch3, OUTPUT);
	pinMode(en1, INPUT);
	pinMode(en2, INPUT);
	pinMode(sw, INPUT_PULLUP);
	enlast = digitalRead(en1);
	Serial.println("startup succes");
	lcdwrite(cursor);

}


void loop() {
	if (Serial.available() > 3) {
		Serialinterprit();
	}


	int input = encoderRead();
	if (input == 1) {
		cursor++;
		if (cursor > 4) { cursor = 4; };
		lcdwrite(cursor);


	}
	if (input == 2) {
		cursor--;
		if (cursor < 0) { cursor = 0; }
		lcdwrite(cursor);

	}
	if (input == 3) {
		if (cursor == 1) {
			Data();
		}
		if (cursor == 0) {
			alarm();
		}
		lcdwrite(cursor);
	}
	//lcdwrite(cursor);
	input = 0;
}


void Serialinterprit() {
	unsigned long command;
	while (Serial.available() != 0) {
		command << 8;
		command |= Serial.read();
	}
	Serial.println();
}

void Data() {
	lcd.clear();
	int dta1 = map(analogRead(in1), maxfugt, minfugt, 0, 100);
	int dta2 = map(analogRead(in2), maxfugt, minfugt, 0, 100);
	int dta3 = map(analogRead(in3), maxfugt, minfugt, 0, 100);
	int air = dht.readHumidity();
	int temp = dht.readTemperature();
	lcd.setCursor(0, 0);
	lcd.print(dta1);
	lcd.print("%");
	lcd.setCursor(6, 0);
	lcd.print(dta2);
	lcd.print("%");
	lcd.setCursor(12, 0);
	lcd.print(dta3);
	lcd.print("%");
	lcd.setCursor(1, 1);
	lcd.print("ch1");
	lcd.setCursor(7, 1);
	lcd.print("ch2");
	lcd.setCursor(13, 1);
	lcd.print("ch3");
	while (encoderRead() != 3) { delay(10); }
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Luftfugtighed:");
	lcd.setCursor(0, 1);
	lcd.print(air);
	lcd.print("%");
	while (encoderRead() != 3) { delay(10); }
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Temperatur:");
	lcd.setCursor(0, 1);
	lcd.print(temp);
	lcd.print(" C");
	while (encoderRead() != 3) { delay(10); }
}

void alarm() {

	int8_t cursor2 = 0;
	lcd.setCursor(0, 0);
	lcd.clear();
	lcd.println("Ny alarm       ");
	lcd.setCursor(0, 1);
	lcd.print("Alarmliste");
	uint8_t i = 0;
	lcd.setCursor(15, 0);
	lcd.println("<");
	while (i != 3) {
		i = encoderRead();
		if (i == 1) {
			cursor2 = 0;
			lcd.setCursor(15, 1);
			lcd.println(" ");
			lcd.setCursor(15, 0);
			lcd.println("<");
		}
		if (i == 2) {
			cursor2 = 1;
			lcd.setCursor(15, 0);
			lcd.println(" ");
			lcd.setCursor(15, 1);
			lcd.println("<");
		}
	}
	debounce();
	if (cursor2 == 0) {					// ny alarm <<<<<<<<<
		unsigned long command = 0x10000000;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Bestem Kanal:");
		lcd.setCursor(0, 1);
		lcd.print("- kanal nr.");
		cursor2 = 1;
		i = 0;
		while (i != 3) {					//vælg kanal
			i = encoderRead();
			if (i == 1) {
				cursor2++;
				if (cursor2 > 3) { cursor2 = 1; }
			}
			if (i == 2) {
				cursor2--;
				if (cursor2 < 1) { cursor2 = 3; }
			}
			if (i != 0) {
				lcd.setCursor(13, 1);
				lcd.print(cursor2);
			}

		}
		unsigned long modifier = cursor2;
		command |= modifier << 26;
		Serial.println(command, BIN);
		debounce();
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Startbetingelse:");
		i = 0;
		bool state = 0;
		while (i != 3) {
			i = encoderRead();
			if (i == 1) {
				state = 1;
				lcd.setCursor(0, 1);
				lcd.print("            ");
				lcd.setCursor(0, 1);
				lcd.print("- Tidsstying");
			}
			if (i == 2) {
				state = 0;
				lcd.setCursor(0, 1);
				lcd.print("            ");
				lcd.setCursor(0, 1);
				lcd.print("- Fugtighed");
			}
		}
		debounce();
		if (state == 1) {				//if tid start
			modifier = 1;
			command |= modifier << 25;
			Serial.println(command, BIN);
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("starttidspunkt");
			lcd.setCursor(0, 1);
			lcd.print("00 : 00");
			i = 0;
			int hour = 0;
			int minute = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 2) {
					hour++;
					if (hour > 23) {
						hour = 0;
					}
					lcd.setCursor(0, 1);
					if (hour < 10) {
						lcd.print("0");
					}
					lcd.print(hour);
				}
				if (i == 1) {
					hour--;
					if (hour < 0) {
						hour = 23;
					}
					lcd.setCursor(0, 1);
					if (hour < 10) {
						lcd.print("0");
					}
					lcd.print(hour);
				}
			}
			debounce();
			i = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 2) {
					minute++;
					if (minute > 59) {
						minute = 0;
					}
					lcd.setCursor(5, 1);
					if (minute < 10) {
						lcd.print("0");
					}
					lcd.print(minute);
				}
				if (i == 1) {
					minute--;
					if (minute < 0) {
						minute = 59;
					}
					lcd.setCursor(5, 1);
					if (minute < 10) {
						lcd.print("0");
					}
					lcd.print(minute);
				}

			}
			modifier = (hour * 60 + minute);
			command |= modifier << 14;
			Serial.println(command, BIN);
			debounce();
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Ugedag: ");
			String dage[] = { "Hver dag", "Mandag  ", "Tirsdag ", "Onsdag  ", "Torsdag  ", "Fredag   ", "L0rdag   ", "S0ndag   ", " " };
			cursor2 = 0;
			i = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 1) {
					cursor2++;
					Serial.println(cursor2);
					if (cursor2 > 7) {
						cursor2 = 0;
					}
					modifier = cursor2;
					lcd.setCursor(0, 1);
					lcd.print(dage[cursor2]);
				}
				if (i == 2) {
					cursor2--;
					if (cursor2 < 0) {
						cursor2 = 7;
					}
					modifier = cursor2;
					lcd.setCursor(0, 1);
					lcd.print(dage[cursor2]);
				}
			}
			command |= modifier << 11;
			Serial.println(command, BIN);
			debounce();
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Stopbetingelse:");
			i = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 1) {
					lcd.setCursor(0, 1);
					lcd.print("tid: ");
					state = 1;
				}
				if (i == 2) {
					lcd.setCursor(0, 1);
					lcd.print("fugt:");
					state = 0;
				}
			}
			debounce();
			i = 0;
			int value = 0;

			if (state == 1) {	//stopbetingelse: tid.
				modifier = 1;
				command |= modifier << 10;
				lcd.setCursor(10, 1);


				while (i != 3) {
					i = encoderRead();
					if (i == 1) {
						value++;
						if (value > 1020) {
							value = 1020;
						}

						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("s");

					}
					if (i == 2) {
						value--;
						if (value < 1) {
							value = 1;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("s");
					}
				}
				debounce();
				command |= value;
				//FUCKING NICE 
			}
			else {				//stopbetingelse: fugtighed.
				value = 5;
				modifier = 1;
				command |= modifier << 10;
				lcd.setCursor(5, 1);
				lcd.print("5%");
				while (i != 3) {
					i = encoderRead();
					if (i == 1) {
						value++;
						if (value > 100) {
							value = 100;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("%");

					}
					if (i == 2) {
						value--;
						if (value < 5) {
							value = 5;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("%");
					}
				}

				debounce();
				command |= map(value, 0, 100, 0, 1023);

			}

		}
		else if (i == 2) { // case fugtighed <<<<<<<<<<<<<
			modifier = 0;
			command |= modifier << 25;
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Startfugtighed:");
			lcd.setCursor(6, 1);
			lcd.print("%");
			i = 0;
			int value1 = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 1) {
					value1++;
					if (value1 > 80) {
						value1 = 80;
					}
					lcd.setCursor(0, 1);
					lcd.write(value1);

				}
				if (i == 2) {
					value1--;
					if (value1 < 10) {
						value1 = 10;
					}
				}

			}
			modifier = map(value1, 0, 100, 0, 1023);
			command |= modifier << 15;		/// ========================================
			debounce();

			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Stopbetingelse:");
			i = 0;
			while (i != 3) {
				i = encoderRead();
				if (i == 1) {
					lcd.setCursor(0, 1);
					lcd.print("tid: ");
					state = 1;
				}
				if (i == 2) {
					lcd.setCursor(0, 1);
					lcd.print("fugt:");
					state = 0;
				}
			}
			debounce();
			i = 0;
			int value = 0;

			if (state == 1) {	//stopbetingelse: tid.
				modifier = 1;
				command |= modifier << 10;
				lcd.setCursor(10, 1);


				while (i != 3) {
					i = encoderRead();
					if (i == 1) {
						value++;
						if (value > 1020) {
							value = 1020;
						}

						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("s");

					}
					if (i == 2) {
						value--;
						if (value < 1) {
							value = 1;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("s");
					}
				}
				debounce();
				command |= value;
				//FUCKING NICE 
			}
			else {				//stopbetingelse: fugtighed.
				value = value1 + 10;
				modifier = 0;
				command |= modifier << 10;
				lcd.setCursor(5, 1);
				lcd.print(value1 + 10);
				while (i != 3) {
					i = encoderRead();
					if (i == 1) {
						value++;
						if (value > 90) {
							value = 100;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("%");

					}
					if (i == 2) {
						value--;
						if (value < value1 + 10) {
							value = value1 + 10;
						}
						lcd.setCursor(5, 1);
						lcd.print("      ");
						lcd.setCursor(5, 1);
						lcd.print(value);
						lcd.print("%");
					}
				}
				modifier = map(value, 0, 100, 0, 1023);
				command |= modifier;
				debounce();
			}
		}

		Serial.println(command, BIN);
		AddCmd(command);
	}
	else {				//liste over alarmer
		cursor2 = 2;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Alarmer:");
		i = 0;
		while (i != 3) {

			i = encoderRead();
			if (i == 1) {
				cursor2++;
				if (cursor2 > 50) {
					cursor2 = 50;
					lcd.setCursor(0, 0);
					lcd.print(EEPROM[cursor2] - 1);
					lcd.setCursor(0, 1);
					lcd.print(EEPROM[cursor2]);

				}
			}
			if (i == 2) {
				cursor2--;
				if (cursor2 < 2) {
					cursor2 = 2;
				}
				lcd.setCursor(0, 0);
				if (cursor2 == 2) {
					lcd.print("Alarmer:         ");
				}
				else {
					lcd.print(label(cursor));
				}
				lcd.setCursor(0, 1);
				lcd.print(label(cursor));
			}

		}
	}
}


String label(int input) {
	String output;
	unsigned long constructer;
	if (subbyte(EEPROM[input], 1, 3) == 1) {
		constructer = construct(EEPROM[input], EEPROM[input + 1], EEPROM[input + 2], EEPROM[input + 3]);
		output = "C";
		output += subbyte(constructer, 5, 1);
		if (subbyte(constructer, 7, 0) == 1) {
			output += "T";
			int eftermidnat = subbyte(constructer, 8, 10);
			int hour = eftermidnat / 60;
			if (hour < 10) {
				output += "0" + hour;
			}
			else {
				output += hour;
			}
			output += ":";
			int minute = (eftermidnat % 60) * 60;
			if (minute < 10) {
				output += "0" + minute;
			}
			else {
				output += minute;
			}
			output += "D";
			String dage[] = { "h", "m", "t" , "o", "T", "f", "l", "s" };
			output += dage[subbyte(constructer, 19, 2)];
			if (subbyte(constructer, 22, 0) == 1) {   //stop = tid
				output += "T";
				output += subbyte(constructer, 23, 9);
				output += "s";
			}
			else {						//stop fugtighed
				output += "F";
				output += map(subbyte(constructer, 23, 9), 0, 1024, 0, 100);
				output += "%";
			}
		}
		else {
			output += "F";
			output += map(subbyte(constructer, 8, 9), 0, 1024, 0, 100);
			output += "%";

			if (subbyte(constructer, 22, 0) == 1) {   //stop = tid
				output += "T";
				output += subbyte(constructer, 23, 9);
				output += "s";
			}
			else {						//stop fugtighed
				output += "F";
				output += map(subbyte(constructer, 23, 9), 0, 1024, 0, 100);
				output += "%";
			}

		}
		return output;
	}
	else {
		return "ingen Kommando";
	}
}

uint8_t encoderRead() {

	enstate = digitalRead(en1);
	state2 = digitalRead(en2);
	if (enstate == LOW) {
		if (state2 != LOW) {
			while (digitalRead(en1) != HIGH || digitalRead(en2) != HIGH) { delay(10); }
			enlast = enstate;
			return 1;
		}
		else {
			while (digitalRead(en1) != HIGH || digitalRead(en2) != HIGH) { delay(10); }
			enlast = enstate;
			return 2;
		}
	}
	if (digitalRead(sw) == LOW) {
		while (digitalRead(sw) == LOW) { delay(10); }
		return 3;
	}
	return 0;
}


void AddCmd(unsigned long input) {

	int k = arraylength();

	EEPROM.update(k, subbyte(input, 1, 7));
	EEPROM.update(k + 1, subbyte(input, 9, 7));
	EEPROM.update(k + 2, subbyte(input, 17, 7));
	EEPROM.update(k + 3, subbyte(input, 25, 7));
}

void readcmd(int x) {			//omskriv
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	uint8_t d = 0;

	for (int j = 0; j != EEPROM.read(1); j++) {
		a = EEPROM.read(x);
		x++;
		b = EEPROM.read(x);
		x++;
		c = EEPROM.read(x);
		x++;
		d = EEPROM.read(x);
		cmddata[j] = construct(a, b, c, d);
		x++;
	}

}

void lcdwrite(uint8_t i) {
	lcd.clear();
	i = constrain(i, 0, 4);
	lcd.setCursor(0, 0);
	lcd.print(menu1[i]);
	lcd.setCursor(15, 0);
	lcd.print("<");
	lcd.setCursor(0, 1);
	lcd.print(menu1[i + 1]);

}

void debounce() {
	while (encoderRead() == 3) { delay(100); }
}

unsigned long construct(uint8_t d, uint8_t c, uint8_t b, uint8_t a) {
	unsigned long data;
	data = a + (b << 8) + (c << 16) + (d << 24);
	return data;
}

int arraylength() {
	int x = 1;
	while (EEPROM[2] != B11111111) {
		x += 4;
	}
	return x;
}

unsigned long subbyte(unsigned long input, int from, int length) {			//subbyte(long, from, to); inkluderer 1 from og to (start 0)
	unsigned long check = 1;
	unsigned long output = 0;
	check = check << (32 - from);
	int z = 0;
	while (z <= length) {
		output ^= (check & input);
		check = check >> 1;
		z++;
	}
	z = 0;
	while (z < 32 - (from + length)) {
		output = output >> 1;
		z++;
	}
	return output;
}

void commandinterprit(unsigned long k) {
	bool start = false;

	if (subbyte(k, 1, 3) == 1) {
		int channel = subbyte(k, 5, 1); //finder den channel der skal bruges
		if (subbyte(k, 7, 0) == 1) {			//case tid start <<<<

			int Tstart = subbyte(k, 8, 10);
			int	dag = subbyte(k, 19, 2);
			if (dag == dagnow || dag == 0) {
				if (Tstart > Tnow) {
					start = true;
				}									//Rework <<================================
			}

			if (subbyte(k, 22, 0) == 1) {
				//case tid stop

				int Tstop = subbyte(k, 23, 9);
				Serial.print("channel:  ");
				Serial.println(channel);
				Serial.print("Tstart:  ");
				Serial.println(Tstart);
				Serial.print("Dag:  ");
				Serial.println(dag);
				Serial.print("Længde:  ");
				Serial.println(Tstop);
			}
			else {
				//fugt stop

			}
		}
		else {				// case fugtigtighed






		}
	}

}

// Læser sensor med 0 - 100%
int ReadSensor(int channel) {
	int input;
	if (channel == 1) {
		input = analogRead(in1);
	}
	else if (channel == 2) {
		input = analogRead(in2);
	}
	else if (channel == 3) {
		input = analogRead(in3);
	}
	else {
		return -1;
	}
	return(map(input, minfugt, maxfugt, 100, 0));
}
void action() {


}