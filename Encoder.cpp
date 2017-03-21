#include "Encoder.h"
#define NOT_AN_INTERRUPT 50

RotaryEncoder *Encoder;

RotaryEncoder::RotaryEncoder(uint8_t button_pin, uint8_t encA_int_pin, uint8_t encB_pin)
{
	initEncoder(encA_int_pin, encB_pin);
	initButton(button_pin);
}

void Encoder_Init(uint8_t button_pin, uint8_t encA_int_pin, uint8_t encB_pin)
{
	Encoder = new RotaryEncoder(button_pin, encA_int_pin, encB_pin);
}

void RotaryEncoder::initEncoder(uint8_t encA_int_pin, uint8_t encB_pin) 
{
	pinMode(encA_int_pin, INPUT_PULLUP);
	pinMode(encB_pin, INPUT_PULLUP);

	bMask = digitalPinToBitMask(encB_pin);
	bReg = portInputRegister(digitalPinToPort(encB_pin));
	encoderInterrupt = digitalPinToInterrupt(encA_int_pin);
}

void RotaryEncoder::initButton(uint8_t button_pin) 
{
	buttonPin = button_pin;
	pinMode(buttonPin, INPUT_PULLUP);
}

void ISR_encoder_handler() 
{
	Encoder->IntHandler();
}

void RotaryEncoder::EncoderOn() {
	attachInterrupt(encoderInterrupt, ISR_encoder_handler, FALLING);
	encoderCounter = 0;
	encoderIsOn = true;
}

void RotaryEncoder::EncoderOff() {
	detachInterrupt(encoderInterrupt);
	encoderCounter = 0;
	encoderIsOn = false;
}

char RotaryEncoder::CheckEncoder() {
	if (encoderIsOn) {
		cli();
		char retval = encoderCounter;
		encoderCounter = 0;
		sei();
		return retval;
	} else {
		return 0;
	}
}

/*
const char asciiSet[] PROGMEM = {'a','b','c','d','e','f','g','h','i',
		'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G',
		'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X',
		'Y','Z','.','\?','/','\\','*','-','_','+'     // INSERT MORE HERE, KEEP SPACE LAST
		, ' '};

const int sizeOfAsciiSet = sizeof(asciiSet) / sizeof(asciiSet[0]);



int findCharIndexInAsciiSet(char aVal) {
	for (int i = 0; i < sizeOfAsciiSet; i++){
		char chr = pgm_read_byte_near(asciiSet + i);
		if(chr == aVal){
			return i;
		}
	}
	return 0;  // return 0 by default.
}

void useRotaryEncoderASCII(char& var) {
	int cre = checkRotaryEncoder();
	int chr = findCharIndexInAsciiSet(var);

	chr += cre;
	chr %= sizeOfAsciiSet;
	while(chr < 0){   // fix it if it went negative.  Use while just in case it went way negative.
		chr += sizeOfAsciiSet;
	}

	var = pgm_read_byte_near(asciiSet + chr);
}
*/

boolean RotaryEncoder::PeekEncoder() 
{
	return encoderCounter != 0;
}

boolean RotaryEncoder::IsEncoderOn()
{
	return encoderIsOn;
}

void RotaryEncoder::PollButton() 
{
	static boolean buttonState = HIGH;
	static boolean lastState = HIGH;
	static unsigned long pressTime = 0;
	if (buttonIsOn) 
	{
		boolean curState = digitalRead(buttonPin);
		if (curState != lastState) 
		{
			pressTime = millis();
		}
		if ((millis() - pressTime) > BUTTON_DEBOUNCE) //button has been in present state long enough
		{  
			if ((buttonState == LOW) && (curState == HIGH)) // button was pressed and is being released
			{
				if (!killPress) //Prevents registering a press on the release from a long press
				{
					buttonPressed = true;    // register a button press
				}
				killPress = false;
			}
			// buttonState is the fully debounced state of the button for the rest of the program to use
			buttonState = curState;
		}

		if ((buttonState == LOW) && (millis() - pressTime > BUTTON_LONG_PRESS)) 
		{
			buttonLongPressed = true;
			killPress = true;    //  Force to skip next press when button is released after long press
		}
		lastState = curState;
	}
}

void RotaryEncoder::IntHandler()
{
	//  My encoder has a full cycle of 4 transitions per click.
	//  I only want one increment or decrement per click
	//  So we are only looking at one transition
	//  We're only using the falling edge on the interrupt pin.
	//  So we know ENCODER_INTERRUPT_PIN is LOW.
	//  If ENCODER_B_PIN is HIGH then they're different so decrement
	//  if(digitalRead(ENCODER_B_PIN) == HIGH) {
	if (*bReg & bMask) {
		encoderCounter--;
	} else {
		encoderCounter++;
	}
}

/* Clears button press states. */
void RotaryEncoder::clearButton() 
{
	buttonPressed = false;
	buttonLongPressed = false;
}

/* Returns whether the button was pressed for a long duration. */
boolean RotaryEncoder::CheckButtonLongPress() 
{
	boolean retval = buttonLongPressed;
	buttonLongPressed = false;
	return retval;
}

/* Returns whether the button was pressed. */
boolean RotaryEncoder::CheckButton() 
{
	boolean retval = buttonPressed;
	buttonPressed = false;
	return retval;
}

/* Activate button press recognition. */
void RotaryEncoder::ButtonOn() 
{
	buttonIsOn = true;
	clearButton();
}

/* Deactivate button press recognition. */
void RotaryEncoder::ButtonOff() 
{
	buttonIsOn = false;
	clearButton();
}

/* Returns whether button press recognition is active. */
boolean RotaryEncoder::IsButtonOn() 
{
	return buttonIsOn;
}