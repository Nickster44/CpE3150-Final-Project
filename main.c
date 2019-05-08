/*
 * GccApplication2.c
 *
 * Created: 5/5/2019 7:42:30 PM
 * Author : Nicholas Dapprich
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))

bool b1, b2, b3, b4, b5, b6, b7, b8, modeButton, session = false;
char mode = 1, pos = 0;
char array1[10] = {0};
short delayTime = 1000;
short timeBuffer = 0;
char patternSize = 4;

int main(void)
{
	PORTA = 0xFF; // use pull-up resistors on Port A for the input switches
	DDRA = 0x00; // Set port A to input
	DDRD = 0xFF; // Set port D to output
	PORTE = 0xBB; // 0xBB for Button input, led output and RX TX, I think.
	DDRE = 0xBB;

	TCCR0A = (1 << WGM01); //Set CTC bit
	OCR0A = 125;
	TIMSK0 = (1 << OCIE0A); // Set Compare A interrupt
	sei(); 

	TCCR0B = (1 << CS01) | (1 << CS00); // Start at 64 prescaler

	srand(1);

	USART_Init(9600);

    /* Replace with your application code */
    while (1) 
    {
		updateButtons()
		
		if(session == false) {
			pos = 0;
			newArray();
			displayPattern();
			displayingPattern = false;
			session = true;
		}
		if(validate == true) {
			if(currentButton == array1[pos]) {
				validate = false;			// if button pressed was correct, then pos increment and continue game
				if(pos == patternSize - 1) {
					displaySerialWinningMessage();
					session = false;
				}
				pos++;
			}else {
				validate = false;	// if button pressed was not correct, restart session.
				session = false;
			}
		}

    }
}

void buttonPressed(char saidButton) {
	currentButton = saidButton;
	saidButtonLED = HIGH;
	makeButtonPressedNoise(); // need to use timer2 for noise frequency
	validate = true;

}

void updateButtons() {
	
	if (anyOtherButton == HIGH) {
		buttonPressed(saidButton);  // Send button ID that was pressed to function to display button's LED and call noise function
	}

	if(modeButton == true) {
		incrementMode()
	}
}

void incrementMode() {
	mode++;
	if(mode == 4) {
		mode = 1;
	}
	switch (mode) {
		case 1: {
			delayTime = 1000;
			patternSize = 4;
			session = false;
		}
		case 2: {
			delayTime = 750;
			patternSize = 7;
			session = false;
		}
		case 3: {
			delayTime = 500;
			patternSize = 10;
			session = false;
		}
	}
}

void displayPattern() {
	for(char i = 0; i < patternSize; i++) {
		
	}
}

void newArray() {
	for(char i = 0; i < 10; i++) {
		char x = (rand() % 9) + 1;
		array1[i] = x;
	}
}

 void delay(short time_ms) {
	bool complete = false;
	timeBuffer = 0;
	while(!complete) {
		complete = (time_ms == timeBuffer) ? true : false;
	}
 }

 ISR(TIMER0_COMPA_vect) {
	timeBuffer++;
	//TCNT0 = 0; dont think this is needed.
 }
