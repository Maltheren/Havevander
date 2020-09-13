/*
 Name:		Smart_garden_visual.ino
 Created:	9/13/2020 7:07:33 PM
 Author:	armin
*/
#define ch1 5
#define	ch2	4
#define	ch3	3

long cmddata[100];
void setup() {
	pinMode(ch1, OUTPUT);
	pinMode(ch2, OUTPUT);
	pinMode(ch3, OUTPUT);
	Serial.begin(9600);
	
}


void loop() {
	unsigned long test = B11011110;
	Serial.println(subbyte(test, 26, 4),BIN);
	delay(10000);
}


unsigned long readcmd(long d, long c, long b, long a) {
	unsigned long data;
	data = a + (b << 8) + (c << 16) + (d << 24);
	return data;
}

unsigned long write(long k) {
	int x = 0;
	while(cmddata[x] != 0){
		x++;
	}
	cmddata[x] = k;
}

unsigned long subbyte(unsigned long k, int i, int length){			//subbyte(long, from, to);
	Serial.println(k, BIN);
	unsigned long check = 1;
	unsigned long output = 0;
	Serial.print("before:   ");
	Serial.println(check, BIN);
	check <<= (32-i);
	Serial.print("after:   ");
	Serial.println(check, BIN);
	int z = 0;
	Serial.println("first loop");
	while (z <= length) {
		Serial.println(check, BIN);
		check >> 1;
		output ^= (check & k);
		z++;
		delay(1000);
	}
	while ((output & 1) != 1) {
		output >> 1;

		return output;
	}
}




void command(long k) {
	long input = k;
	long check = 0x80000000;
}


void action() {


}