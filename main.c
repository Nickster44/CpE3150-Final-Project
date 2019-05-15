/*
 * GccApplication2.c
 *
 * Created: 5/5/2019 7:42:30 PM
 * Author : Nicholas Dapprich
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define F_CPU 16000000UL		//Clock freq
#define pinID1 5  //Pin IDs for shifting In/Out
#define pinID2 5
#define pinID3 6
#define pinID4 7

void USART_Init(unsigned long);  // Serial functions
char USART_RxChar();
void USART_TxChar(char);
void USART_SendString(char*);
unsigned char value;

void newArray();		//Create a new array
void delay(unsigned int time_ms);		//delay function using timers
void makeNoise(unsigned int Hz, unsigned int delay_ms);	//Sound generator function using timers
void displayPattern();	//Output pattern using LEDs
void incrementMode();	// Increment the game mode by 1
void updateButtons();	// Check for button presses and debouncing

char endPatternSize = 1;			// List of User variables
char b1 = 1, b2 = 1, b3 = 1, b4 = 1, modeButton, session = 0, replay = 0, hb = 0, end = 0, hr = 0;	// Button variables
char lastb1 = 1, lastb2 = 1, lastb3 = 1 , lastb4 = 1, lastm = 1, replayL = 1, hbL = 1, endL = 1, hrL = 1;
char mode = 1, pos = 0, checkTime = 0, validate = 0, currentButton = 0;
char array1[10] = {0};		// Array for storing pattern
char array2[100] = {0};		// Array for storing endless pattern
int delayTime = 1000, overFlowCount = 0, overFlowTarget = 0, remainder1 = 0, totalTicks = 0;  // Timer variables
unsigned int timeBuffer = 0, delayBuffer = 0, ms_Counter = 0, ms_Target = 0;
char patternSize = 4, toggleSoundPin = 0, ms_Count = 0;
unsigned int score = 0, highscore = 5;

int main(void)
{
	PORTA = 0xFF; // use pull-up resistors on Port A for the input switches
	DDRA = 0x00; // Set port A to input
	DDRD = 0xFB;    // USART1 uses pins PD2 for Receiving (input) & PD3 for Transmitting (output)
	PORTD = 0xFD;    // turn all of the LEDs off (pull-up resistors on)
	PORTE = 0xBF;	//use pull-up resistors on Port E for the input switches
	DDRE = 0xBF;	//Led output, speaker output, button input

	TCCR0A = (1 << WGM01); //Set CTC bit
	OCR0A = 250;	// Value for compare to get one millisecond
	TIMSK0 = (1 << OCIE0A); // Set Compare A interrupt
	sei(); 

	TCCR0B = (1 << CS01) | (1 << CS00); // Start at 64 pre-scaler

	srand(1);  // Seed random function

	USART_Init(9600);  // Start UART port
	highscore = eeprom_read_word(0); // Retrieve highscore from EEPROM
	USART_SendString("Welcome to Repeat the Pattern! Brought to you by Nicholas, Caleb, Daniel and David.\n"); //Welcome message
	
    /* Main loop */
    while (1) 
    {
		updateButtons(); // check for button presses
		if(mode != 4) {  // check for endless mode
			endPatternSize = 1;
		}
		if(session == 0) {	//initialize session once
			pos = 0;
			if(mode != 4) {  //do not create new array if in endless mode
				newArray();
			}
			displayPattern();	// output pattern to LEDs
			session = 1;		// Only call session once
		}

		if(validate == 1) {		// validate the button press
			if(currentButton == array1[pos] && mode != 4) {  // check button value to array value
				validate = 0;			// if button pressed was correct, then pos increment and continue game
				if(pos == patternSize - 1) {
					score += patternSize * mode; // Scoring system
					USART_SendString("Level Complete!\nCurrent Score: ");
					char buff[10];		// integer to char array buffer
					itoa(score, buff, 10);
					USART_SendString(buff);
					USART_SendString("\n");
					
					if(score > highscore) {		// if set a new highscore
						USART_SendString("Congratulations on setting a new highscore!!!\n");
						highscore = score;
						eeprom_write_word(0, highscore); // Save new highscore
					}
					
					makeNoise(400, 200);	// make victory tune
					delay(300);				// Victory tune created by Nicholas as a special feature when completing a pattern
					makeNoise(500, 200);
					delay(350);
					makeNoise(600, 200);
					delay(300);
					makeNoise(700, 200);
					delay(2000);
					session = 0;			// restart session
				}
				pos++;
			}else if(mode == 4 && currentButton == array2[pos]){ //check for endless mode
				validate = 0;	
				if(pos == endPatternSize -1) {
					makeNoise(400, 200);		// Victory tune created by Nicholas as a special feature when completing a pattern
					delay(300);
					makeNoise(500, 200);
					delay(350);
					makeNoise(600, 200);
					delay(300);
					makeNoise(700, 200);
					delay(2000);
					session = 0;
					endPatternSize++; // continue endless mode session with increasing size
				}
				pos++;
			}
			
			else {
				validate = 0;	// if button pressed was not correct, restart session.
				session = 0;
				buttonPressed(5); // Light up leds in ERROR mode
			}
		}
    }
}


void buttonPressed(char button) {		// When button is pressed light LED and make noise
	
	switch (button)
	{
	case 1:
		PORTE &= ~(1 << pinID1);
		makeNoise(400, 500);
		delay(500);
		PORTE |= 1 << pinID1;
		validate = 1;
		break;
	case 2:
		PORTD &= ~(1 << pinID2);
		makeNoise(500, 500);
		delay(500);
		PORTD |= 1 << pinID2;
		validate = 1;
		break;
	case 3:
		PORTD &= ~(1 << pinID3);
		makeNoise(600, 500);
		delay(500);
		PORTD |= 1 << pinID3;
		validate = 1;
		break;
	case 4:
		PORTD &= ~(1 << pinID4);
		makeNoise(700, 500);
		delay(500);
		PORTD |= 1 << pinID4;
		validate = 1;
		break;
	case 5:
		PORTD &= ~(1 << pinID4) & ~(1 << pinID3) & ~(1 << pinID2);		// light up all LEDs when you LOSE created Nicholas to give more feedback to the players
		PORTE &= ~(1 << pinID1);
		makeNoise(300, 1000);
		delay(500);
		PORTD |= (1 << pinID4) | (1 << pinID3) | (1 << pinID2) | (1 << pinID1);
		PORTE |= (1 << pinID1);
		delay(1500);
		break;
	}
	currentButton = button;		//set current button
}

void updateButtons() {		// check for buttons being pressed
	b1 = (PINE >> (pinID1+1)) & 1;		//button 1 pressed
	if(b1 == 0 && b1 != lastb1) {
		delay(25);
		b1 = (PINE >> (pinID1+1)) & 1;
		if(b1 == 0) {
			buttonPressed(1);
		}
	}
	lastb1 = b1;
	
	b2 = (PINA >> pinID2) & 1;		//button 2 pressed
	if(b2 == 0 && b2 != lastb2) {
		delay(25);
		b2 = (PINA >> pinID2) & 1;
		if(b2 == 0) {
			buttonPressed(2);
		}
	}
	lastb2 = b2;
	
	b3 = (PINA >> pinID3) & 1;		//button 3 pressed
	if(b3 == 0 && b3 != lastb3) {
		delay(25);
		b3 = (PINA >> pinID3) & 1;
		if(b3 == 0) {
			buttonPressed(3);
		}
	}
	lastb3 = b3;
	
	b4 = (PINA >> pinID4) & 1;		//button 4 pressed
	if(b4 == 0 && b4 != lastb4) {
		delay(25);
		b4 = (PINA >> pinID4) & 1;
		if(b4 == 0) {
			buttonPressed(4);
		}
	}
	lastb4 = b4;
	
	replay = (PINA >> 2) & 1;		// replay button pressed
	if(replay == 0 && replay != replayL) {
		delay(25);
		replay = (PINA >> 2) & 1;
		if(replay == 0) {
			displayPattern();		// Replay button feature created by Daniel to give user a hint during playing
		}
	}
	replayL = replay;
	
	modeButton = (PINA >> 1) & 1;		//mode button pressed
	if(modeButton == 0 && modeButton != lastm) {
		delay(25);
		modeButton = (PINA >> 1) & 1;
		if(modeButton == 0) {
			incrementMode();
		}
	}
	lastm = modeButton;
	
	hb = PINA & 1;				//highscore button pressed
	if(hb == 0 && hb != hbL) {
		delay(25);
		hb = PINA & 1;
		if(hb == 0) {
			char buff[10];
			itoa(highscore, buff, 10);
			USART_SendString("Current highscore: ");		// Displaying current Highscore feature created by David to show user's the competition
			USART_SendString(buff);
		}
	}
	hbL = hb;
	
	hr = (PINA >> 4) & 1;		//highscore reset button pressed
	if(hr == 0 && hr != hrL) {
		delay(25);
		hr = (PINA >> 4) & 1;
		if(hr == 0) {
			highscore = 0;
			eeprom_write_word(0, 0);
			USART_SendString("Reset Highscore to 0.\n");	// Reseting Highscore feature created by David
		}
	}
	hrL = hr;
	
	end = (PINA >> 3) & 1;		//endless mode button pressed
	if(end == 0 && end != endL) {
		delay(25);
		end = (PINA >> 3) & 1;
		if(end == 0) {
			mode = 4;
			for(int i = 0; i < 100; i++) {
				char x = (rand() % 4) + 1;
				array2[i] = x;
			}
			USART_SendString("Entering endless mode!\n");	// Starts the endless mode created by Caleb and expanded throughout the program's code
			displayPattern();								// Sets game to a mode that start at patternSize = 1 then increments pattern size
		}													// Everytime they complete the current pattern.
	}
	endL = end;
}

void makeNoise(unsigned int Hz, unsigned int delay_ms) {	//generate a noise at set Freq and for set delay as length
	totalTicks = F_CPU / (Hz * 64);
	overFlowTarget = totalTicks / 255;
	remainder1 = totalTicks % 255;
	if(totalTicks > 255) {
		OCR2A = 255;		// max tick amount for 8 bit timer
	}else{
		OCR2A = totalTicks;	// remainder amount of ticks
	}
	
	TCCR2A = (1 << WGM21); //Set CTC bit
	TIMSK2 = (1 << OCIE2A); // Set Compare A interrupt
	TCCR2B = (1 << CS21) | (1 << CS20); // Start at 64 prescaler
	
	ms_Count = 1;			// boolean to check for ms delay
	ms_Target = delay_ms;
}

 ISR(TIMER2_COMPA_vect) {		// timer 2 interrupt function
	if(overFlowTarget == 0 || (OCR2A == remainder1  && overFlowCount >= overFlowTarget)) {
		PORTE ^= 0x10;		//if required ticks are completed, flip bit for speaker output
		overFlowCount = 0;
		if(totalTicks > 255) {
			OCR2A = 255;
			}else{
			OCR2A = totalTicks;		// set remaining ticks
		}	
	}else{
		overFlowCount += 1;
		if(overFlowCount == overFlowTarget) {
			OCR2A = remainder1;
		}
	}
	
	if(ms_Counter >= ms_Target){		// if delay time is reached
		TCCR2B &= ~(1 << CS21) & ~(1 << CS20); //turn timer2 interrupt off
		ms_Counter = 0;
		ms_Count = 0;
		overFlowCount = 0;
		overFlowTarget = 0;
		remainder1 = 0;
	}
}

void incrementMode() {		//increment mode and update variables
	mode += 1;
	if(mode > 3) {
		mode = 1;
	}
	switch (mode) {
		case 1:
			delayTime = 1000;
			patternSize = 4;
			session = 0;
			PORTD |= 1;		//set mode LED status
			PORTD &= ~2;
			break;
		case 2:
			delayTime = 750;
			patternSize = 7;
			session = 0;
			PORTD |= 2;		//set mode LED status
			PORTD &= ~1;
			break;
		case 3:
			delayTime = 500;
			patternSize = 10;
			session = 0;		//set mode LED status
			PORTD &= ~3;
			break;
	}
}

void displayPattern() {		//display pattern to LEDs
	if(mode == 4) {		//check if in endless mode
		for(int i = 0; i < endPatternSize; i++) {
			switch (array2[i])
			{
				case 1:
				PORTE &= ~(1 << pinID1);
				makeNoise(400, delayTime);
				delay(delayTime);
				PORTE |= 1 << pinID1;
				delay(100);
				break;
				case 2:
				PORTD &= ~(1 << pinID2);
				makeNoise(500, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID2;
				delay(100);
				break;
				case 3:
				PORTD &= ~(1 << pinID3);
				makeNoise(600, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID3;
				delay(100);
				break;
				case 4:
				PORTD &= ~(1 << pinID4);
				makeNoise(700, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID4;
				delay(100);
				break;
			}
		}
	}else {			//check if in normal mode
		for(int i = 0; i < patternSize; i++) {
			switch (array1[i])
			{
				case 1:
				PORTE &= ~(1 << pinID1);
				makeNoise(400, delayTime);
				delay(delayTime);
				PORTE |= 1 << pinID1;
				delay(100);
				break;
				case 2:
				PORTD &= ~(1 << pinID2);
				makeNoise(500, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID2;
				delay(100);
				break;
				case 3:
				PORTD &= ~(1 << pinID3);
				makeNoise(600, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID3;
				delay(100);
				break;
				case 4:
				PORTD &= ~(1 << pinID4);
				makeNoise(700, delayTime);
				delay(delayTime);
				PORTD |= 1 << pinID4;
				delay(100);
				break;
			}
		}
	}
}

void newArray() {		// create new array for pattern
	for(int i = 0; i < 10; i++) {
		char x = (rand() % 4) + 1;
		array1[i] = x;
	}
	USART_SendString("Made new pattern for level ");
	USART_TxChar(mode + 48);
	USART_SendString(".\n");
}

 void delay(unsigned int time_ms) {		//universal delay function
	checkTime = 1;
	timeBuffer = 0;
	delayBuffer = time_ms;
	while (checkTime == 1) {
		PORTD = PORTD;  //busy wait
	}
 }

 ISR(TIMER0_COMPA_vect) {		// timer0 interrupt function
	 
	if(checkTime == 1) {	// timer for delay buffer
		timeBuffer += 1;
		if(timeBuffer >= delayBuffer) {
			checkTime = 0;
		}
	}
	if(ms_Count == 1) {		// count milliseconds
		ms_Counter += 1;
	}
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
}
