/*
 Name:		Smart_garden_visual.ino
 Created:	9/13/2020 7:07:33 PM
 Author:	Malthe Holm Sennels. hest
*/
#define ch1 5
#define	ch2	4
#define	ch3	3

long cmddata[100];
bool executed[100];
int  Tnow = 0;
int	dagnow = 0;

void setup() {
	pinMode(ch1, OUTPUT);
	pinMode(ch2, OUTPUT);
	pinMode(ch3, OUTPUT);
	Serial.begin(9600);
	Serial.println("progam start");
}


void loop() {
	unsigned long test = 0x18000000;
	Serial.println(test, BIN);
	command(test);
	delay(1000);

}


unsigned long readcmd(long d, long c, long b, long a) {
	unsigned long data;
	data = a + (b << 8) + (c << 16) + (d << 24);
	return data;
}



void write(unsigned long k) {
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
			if (dag == dagnow || dag == 0) {
				if (Tstart > Tnow) {
					start = true;
				}
			}
			if(subbyte(k, 22, 0) == 1) {
			//case tid stop

			int Tstop = subbyte(k, 23, 9);
			

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