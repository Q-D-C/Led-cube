#pragma config FOSC=INTOSC, WDTE=OFF, PWRTE=OFF, MCLRE=OFF, CP=OFF, \
 CPD=OFF, BOREN=ON, CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF
#pragma config WRT=OFF, PLLEN=OFF, STVREN=OFF, BORV=HI, LVP=OFF

#include <xc.h>

typedef int state;

enum {
	LOW,
	HIGH
};

#define LATCH_pin	LATCbits.LATC4
#define CLOCK_pin	LATCbits.LATC5
#define DATA_pin	LATCbits.LATC6
#define RESET_pin	LATCbits.LATC7

#define LAYER1		LATBbits.LATB4
#define LAYER2		LATBbits.LATB5
#define LAYER3		LATBbits.LATB6
#define LAYER4		LATBbits.LATB7

#define SPEED		100

#define _XTAL_FREQ 500000 // used in delay

unsigned int layer[4] = {0, 0, 0, 0};

void SetLayers(unsigned int value)
{
	layer[0] = value;
	layer[1] = value;
	layer[2] = value;
	layer[3] = value;
}

void ClearLayers()
{
	SetLayers(0);
}

void turnOffLayers(void)
{
	LAYER1 = LOW;
	LAYER2 = LOW;
	LAYER3 = LOW;
	LAYER4 = LOW;
}

void turnOnLayer(int x)
{
	if (x == 0) {
		LAYER1 = HIGH;
	}
	if (x == 1) {
		LAYER2 = HIGH;
	}
	if (x == 2) {
		LAYER3 = HIGH;
	}
	if (x == 3) {
		LAYER4 = HIGH;
	}
}

void turnOnAllLayers()
{
	LAYER1 = HIGH;
	LAYER2 = HIGH;
	LAYER3 = HIGH;
	LAYER4 = HIGH;
}

void clock_signal(void)
{
	CLOCK_pin = 1;
	__delay_us(500);
	CLOCK_pin = 0;
	__delay_us(500);
}

void latch_enable(void)
{
	LATCH_pin = 1;
	__delay_us(500);
	LATCH_pin = 0;
}

void send_data(unsigned int data_out)
{
	int i;
	for (i = 0; i < 8; i++) {
		DATA_pin = (data_out >> i) & (0x01);
		clock_signal();
	}
	latch_enable(); // Data finally submitted
}

void empty()
{
	RESET_pin = LOW;
	turnOnAllLayers();
	__delay_ms(10);
	turnOffLayers();
	RESET_pin = HIGH;
}


void voor()
{
	ClearLayers();
	layer[0] = 0b0000000010001000;
	layer[1] = 0b0000000001000100;
	layer[2] = 0b0000000000100010;
	layer[3] = 0b0000000000010001;
	send_data(layer[0]);
	turnOnAllLayers();
	__delay_ms(SPEED);
	send_data(layer[1]);
	__delay_ms(SPEED);
	send_data(layer[2]);
	__delay_ms(SPEED);
	send_data(layer[3]);
	__delay_ms(SPEED);
}

void achter()
{
	ClearLayers();
	layer[0] = 0b0000000010001000;
	layer[1] = 0b0000000001000100;
	layer[2] = 0b0000000000100010;
	layer[3] = 0b0000000000010001;
	send_data(layer[3]);
	turnOnAllLayers();
	__delay_ms(SPEED);
	send_data(layer[2]);
	__delay_ms(SPEED);
	send_data(layer[1]);
	__delay_ms(SPEED);
	send_data(layer[0]);
	__delay_ms(SPEED);
}

void op()
{
	layer[0] = 0b1111111111111111;
	send_data(layer[0]);
	turnOnLayer(0);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(1);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(2);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(3);
	__delay_ms(SPEED);
}

void neer()
{
	layer[0] = 0b1111111111111111;
	send_data(layer[0]);
	turnOnLayer(3);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(2);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(1);
	__delay_ms(SPEED);
	turnOffLayers();
	turnOnLayer(0);
	__delay_ms(SPEED);
}

void init(void)
{
	OSCCON = 0b00111000; //500kHz clock speed
	TRISC4 = 0; // Set  DATA_pin as output pin
	TRISC5 = 0; // Set  CLOCK_pin as output pin
	TRISC6 = 0; // Set LATCH_pin as output pin
	TRISC7 = 0; // Set RESET_pin as output pin
	TRISB4 = 0; // Set LAYER1 as output
	TRISB5 = 0; // Set LAYER1 as output
	TRISB6 = 0; // Set LAYER1 as output
	TRISB7 = 0; // Set LAYER1 as output
	RESET_pin = HIGH;
	//boot();
}

void main(void)
{
	init();
	while (1) {
		empty();
		for (int i = 0; i < 5; i++) {
			op();
			__delay_ms(100);
			neer();
			__delay_ms(100);
		}
		empty();
		for (int i = 0; i < 5; i++) {
			voor();
			__delay_ms(100);
			achter();
			__delay_ms(100);
		}
	}
	return;
}