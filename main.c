/*
 * GccApplication2.c
 *
 * Created: 5/5/2019 7:42:30 PM
 * Author : Nicholas Dapprich
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

#define BIT(x) (0x01 << (x))
#define pinID1 5
#define pinID2 5
#define pinID3 6
#define pinID4 7

void USART_Init(unsigned long);
char USART_RxChar();
void USART_TxChar(char);
void USART_SendString(char*);
unsigned char value;
void newArray();
void delay(unsigned int time_ms);
void makeNoise(unsigned int Hz, unsigned int delay_ms)
void displayPattern();
void incrementMode();
void updateButtons();
//void makeButtonPressedNoise(char)

char b1 = 1, b2 = 1, b3 = 1, b4 = 1, modeButton, session = 0;
char lastb1 = 1, lastb2 = 1, lastb3 = 1 , lastb4 = 1, lastm = 1;
char mode = 1, pos = 0, checkTime = 0, validate = 0, currentButton = 0;
char array1[10] = {0};
int delayTime = 1000;
unsigned int timeBuffer = 0, delayBuffer = 0;
char patternSize = 4, toggleSoundPin = 0;

int main(void)
{
	PORTA = 0xFF; // use pull-up resistors on Port A for the input switches
	DDRA = 0x00; // Set port A to input
	DDRD = 0xFB;    // USART1 uses pins PD2 for Receiving (input) & PD3 for Transmitting (output)
	PORTD = 0xFF;    // turn all of the LEDs off (pull-up resistors on)
	PORTE = 0xBF; 
	DDRE = 0xBF;

	TCCR0A = (1 << WGM01); //Set CTC bit
	OCR0A = 250;
	TIMSK0 = (1 << OCIE0A); // Set Compare A interrupt
	sei(); 

	TCCR0B = (1 << CS01) | (1 << CS00); // Start at 64 prescaler

	srand(1);

	USART_Init(9600);

    /* Replace with your application code */
    while (1) 
    {
		updateButtons();

		if(session == 0) {
			pos = 0;
			newArray();
			displayPattern();
			session = 1;
		}
		/*
		if(validate == 1) {
			if(currentButton == array1[pos]) {
				validate = 0;			// if button pressed was correct, then pos increment and continue game
				if(pos == patternSize - 1) {
					USART_SendString("Level Complete!");
					session = 0;
				}
				pos++;
			}else {
				validate = 0;	// if button pressed was not correct, restart session.
				session = 0;
			}
		}
		*/
    }
}


void buttonPressed(char button) {
	
	switch (button)
	{
	case 1:
		PORTE &= ~(1 << pinID1);
		delay(500);
		PORTE |= 1 << pinID1;
		validate = 1;
		break;
	case 2:
		PORTD &= ~(1 << pinID2);
		delay(500);
		PORTD |= 1 << pinID2;
		validate = 1;
		break;
	case 3:
		PORTD &= ~(1 << pinID3);
		delay(500);
		PORTD |= 1 << pinID3;
		validate = 1;
		break;
	case 4:
		PORTD &= ~(1 << pinID4);
		delay(500);
		PORTD |= 1 << pinID4;
		validate = 1;
		break;
	}
	
	currentButton = button;
	
	//makeButtonPressedNoise(); // need to use timer2 for noise frequency

}

void updateButtons() {
	b1 = (PINE >> (pinID1+1)) & 1;
	if(b1 == 0 && b1 != lastb1) {
		delay(25);
		b1 = (PINE >> (pinID1+1)) & 1;
		if(b1 == 0) {
			buttonPressed(1);
			makeNoise(400, 1000);
		}
	}
	lastb1 = b1;
	
	b2 = (PINA >> pinID2) & 1;
	if(b2 == 0 && b2 != lastb2) {
		delay(25);
		b2 = (PINA >> pinID2) & 1;
		if(b2 == 0) {
			buttonPressed(2);
			makeNoise(500, 1000);
		}
	}
	lastb2 = b2;
	
	b3 = (PINA >> pinID3) & 1;
	if(b3 == 0 && b3 != lastb3) {
		delay(25);
		b3 = (PINA >> pinID3) & 1;
		if(b3 == 0) {
			buttonPressed(3);
			makeNoise(600, 1000);
		}
	}
	lastb3 = b3;
	
	b4 = (PINA >> pinID4) & 1;
	if(b4 == 0 && b4 != lastb4) {
		delay(25);
		b4 = (PINA >> pinID4) & 1;
		if(b4 == 0) {
			buttonPressed(4);
			makeNoise(600, 1000);
		}
	}
	lastb4 = b4;
	
	modeButton = (PINA >> 1) & 1;
	if(modeButton == 0 && modeButton != lastm) {
		delay(25);
		modeButton = (PINA >> 1) & 1;
		if(modeButton == 0) {
			incrementMode();
		}
	}
	lastm = modeButton;
}

void makeNoise(int Hz, int delay_ms) {
	
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
			session = 0;
		}
		case 2: {
			delayTime = 750;
			patternSize = 7;
			session = 0;
		}
		case 3: {
			delayTime = 500;
			patternSize = 10;
			session = 0;
		}
	}
}

void displayPattern() {
	for(int i = 0; i < patternSize; i++) {
		switch (array1[i])
		{
		case 1:
			PORTE &= ~(1 << pinID1);
			delay(delayTime);
			PORTE |= 1 << pinID1;
			delay(100);
			break;
		case 2:
			PORTD &= ~(1 << pinID2);
			delay(delayTime);
			PORTD |= 1 << pinID2;
			delay(100);
			break;
		case 3:
			PORTD &= ~(1 << pinID3);
			delay(delayTime);
			PORTD |= 1 << pinID3;
			delay(100);
			break;
		case 4:
			PORTD &= ~(1 << pinID4);
			delay(delayTime);
			PORTD |= 1 << pinID4;
			delay(100);
			break;
		}
	}
}

void newArray() {
	for(int i = 0; i < 10; i++) {
		char x = (rand() % 4) + 1;
		array1[i] = x;
	}
	USART_SendString("Made new Array");
}

 void delay(unsigned int time_ms) {
	checkTime = 1;
	timeBuffer = 0;
	delayBuffer = time_ms;
	while (checkTime == 1) {
		PORTD = PORTD;  //busy wait
	}
	
	/*

		//char buf[10];
		//itoa(timeBuffer, buf, 10);
		//USART_SendString(buf);
		//itoa(time_ms, buf, 10);
		//USART_SendString(buf);
		//USART_SendString("Made new Array");

	*/
 }

 ISR(TIMER0_COMPA_vect) {
	 
	if(checkTime == 1) {
		timeBuffer += 1;
		if(timeBuffer >= delayBuffer) {
			checkTime = 0;
		}
	}
	TCNT0 = 0;
 }
 
 void USART_Init(unsigned long BAUDRATE)                // USART initialize function */
{
	int BAUD_PRESCALE = (((F_CPU / (BAUDRATE * 16UL))) - 1);      // Define prescale value
	UCSR1B |= (1 << RXEN) | (1 << TXEN) ;            // Enable USART transmitter and receiver
	UCSR1C |= (1 << UCSZ0) | (1 << UCSZ1);            // Write USCRC for 8 bit data and 1 stop bit, 
	UBRR1L = BAUD_PRESCALE;                            // Load UBRRL with lower 8 bit of prescale value
	UBRR1H = (BAUD_PRESCALE >> 8);                    // Load UBRRH with upper 8 bit of prescale value
}

// Data transmitting function
void USART_TxChar(char data)
{
	UDR1 = data;                                        /* Write data to be transmitting in UDR */
	while (!(UCSR1A & (1<<UDRE)));                    /* Wait until data transmit and buffer get empty */
}

// used to send a string of characters to the USART (instead of one character at a time).
void USART_SendString(char *str)
{
	int i=0;
	while (str[i]!=0)
	{
		USART_TxChar(str[i]);                        /* Send each char of string till the NULL */
		i++;
	}
	USART_TxChar(10); 
	USART_TxChar(13); 
}
