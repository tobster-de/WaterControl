#ifndef Encoder_h
#define Encoder_h

#include "Configuration.h"

class RotaryEncoder 
{
private:
	uint8_t buttonPin;
	uint8_t encoderInterrupt;

	byte bMask;

	volatile byte *bReg;
	volatile char encoderCounter;

	boolean encoderIsOn = false;
	boolean buttonPressed = false;
	boolean buttonLongPressed = false;
	boolean killPress = false;
	boolean buttonIsOn = false;

	void initButton(uint8_t button_pin);
	void initEncoder(uint8_t encA_int_pin, uint8_t encB_pin);

	void clearButton();

public:
	RotaryEncoder(uint8_t button_pin, uint8_t encA_int_pin, uint8_t encB_pin);

	void EncoderOn();
	void EncoderOff();
	boolean IsEncoderOn();
	boolean PeekEncoder();
	char CheckEncoder();

	void ButtonOn();
	void ButtonOff();
	boolean IsButtonOn();
	boolean CheckButtonLongPress();
	boolean CheckButton();
	void PollButton();

	void IntHandler();
};

extern RotaryEncoder *Encoder;

void Encoder_Init(uint8_t address, uint8_t columns, uint8_t rows);

//void ISR_encoder_handler();

/*
// call with mult == 0 allows only one step per call no matter how fast the encoder is turning
template<class T>
void useRotaryEncoder(T& var, int mult = 1)
{
  if(mult){
    var += (checkRotaryEncoder() * mult);
  } 
  else {
    int chek = checkRotaryEncoder();
    if (chek > 0) var += 1;
    if (chek < 0) var -= 1;
  }
}

template<class T>
void useRotaryEncoder(T& var, T mini, T maxi, int mult = 1)
{
  if(mult){
    var += (checkRotaryEncoder() * mult);
  } 
  else {
    int chek = checkRotaryEncoder();
    if (chek > 0) var += 1;
    if (chek < 0) var -= 1;
  }
  if (var > maxi){
    var = mini;
  } else if (var < mini) {
    var = maxi;
  }
  
}
*/

#endif
