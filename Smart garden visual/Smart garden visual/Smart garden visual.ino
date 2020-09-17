/*
 Name:		Smart_garden_visual.ino
 Created:	9/13/2020 7:07:33 PM
 Author:	Malthe Holm Sennels. hest
*/
#include <LiquidCrystal.h>
#define ch1 5
#define	ch2	4
#define	ch3	3

LiquidCrystal lcd(13, 12, 8, 9, 10, 11);

long cmddata[100];
bool executed[100];
int Tnow = 0;
int	dagnow = 0;
String menu1[] = { "Alarmer", "Data", "indstillinger", "test 1", "test 2", "test 3" };


void setup() {
	lcd.begin(16, 2);
	pinMode(ch1, OUTPUT);
	pinMode(ch2, OUTPUT);
	pinMode(ch3, OUTPUT);
	Serial.begin(9600);
	Serial.println("progam start");
	lcd.print("smartgarden");
	lcd.setCursor(0, 1);
	lcd.print("V. 1.01")
}


void loop() {
	//unsigned long test = 0x1A8AC42A;
	lcdwrite(cursor, menu1);
	delay(3000);
	cursor++;
	if (cursor > 4) { cursor = 0; }

}

void lcdwrite(int i, String menu[]) {
	lcd.clear();
	i = constrain(i, 0, 4);
	lcd.setCursor(0, 0);
	lcd.print(menu1[i]);
	lcd.setCursor(0, 1);
	lcd.print(menu1[i + 1]);

}



unsigned long readcmd(long d, long c, long b, long a) {
	unsigned long data;
	data = a + (b << 8) + (c << 16) + (d << 24);
	return data;
}



void writearray(unsigned long k) {
	int x = 0;
	while(cmddata[x] != 0){
		x++;
	}
	cmddata[x] = k;
}

unsigned long subbyte(unsigned long input, int from, int length){			//subbyte(long, from, to); inkluderer 1 from og to (start 0)
	unsigned long check = 1;
	unsigned long output = 0;
	check = check << (32-from);
	int z = 0;
	while (z <= length) {
		output ^= (check & input);
		check = check >> 1;
		z++;
	}
	z = 0;
	while (z < 32-(from + length)) {
			output = output >> 1;
			z++;
	}
	return output;
}

void command(unsigned long k) {
	bool start = false;

	if(subbyte(k, 1, 3) == 1){
		int channel = subbyte(k, 5, 1); //finder den channel der skal bruges
		if (subbyte(k, 7, 0) == 1) {
			//case tid start
			int Tstart = subbyte(k, 8, 10);
			int	dag = subbyte(k, 19, 2);
			/*if (dag == dagnow || dag == 0) {
				if (Tstart > Tnow) {
					start = true;
				}
			}*/
			if(subbyte(k, 22, 0) == 1) {
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
		else {

		}
	}

}


void action() {


}