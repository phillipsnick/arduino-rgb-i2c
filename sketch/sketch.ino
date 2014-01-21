#include <Wire.h>

#define SLAVE_ADDRESS 0x04

// RGB Strip 1
int rPin1 = 3;
int gPin1 = 5;
int bPin1 = 6;

// RGB Strip 2
int rPin2 = 9;
int gPin2 = 10;
int bPin2 = 11;

// Set initial color
int redVal1 = 255;
int grnVal1 = 255; 
int bluVal1 = 255;
int redVal2 = 255;
int grnVal2 = 255; 
int bluVal2 = 255;

int wait = 3;      // 10ms internal crossFade delay; increase for slower fades

// Initialize color variables
int prevR1 = 255;
int prevG1 = 255;
int prevB1 = 255;
int prevR2 = 255;
int prevG2 = 255;
int prevB2 = 255;

// storage for values from i2c
int number = 0;

int debug = 0;

void setup() {
  if (debug == 1) {
    Serial.begin(9600);
  }
  
  // setup all pins required
  pinMode(rPin1, OUTPUT);
  pinMode(gPin1, OUTPUT);  
  pinMode(bPin1, OUTPUT);
  pinMode(rPin2, OUTPUT);
  pinMode(gPin2, OUTPUT);  
  pinMode(bPin2, OUTPUT);

  // default strips to off  
  analogWrite(rPin1, redVal1);
  analogWrite(gPin1, grnVal1);
  analogWrite(bPin1, bluVal1);
  analogWrite(rPin2, redVal2);
  analogWrite(gPin2, grnVal2);
  analogWrite(bPin2, bluVal2);
  
  // initialize i2c as slave and define callback
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  
  if (debug == 1) {
    Serial.println("Ready!");
  }
}

void loop() {
  delay(50);
}

void receiveData(int count) {
  // we must receive 4 values
  // 1st - method to use
  // 2nd - RED value
  // 3rd - GREEN value
  // 4th - BLUE value
  if (count != 4) {
    if (debug == 1) {
      Serial.println("We need 4 values");
    }
    
    return;
  }
  
  // position we are in the array
  int pos = 1;
  
  // values we will be setting
  int method;
  int redVal;
  int greenVal;
  int blueVal;
  
  while(Wire.available() > 0) {
    number = Wire.read();
    
    if (pos == 1) {
      method = number;
    }
    
    if (pos == 2) {
      redVal = number;
    }
    
    if (pos == 3) {
      greenVal = number;
    }
    
    if (pos == 4) {
      blueVal = number;
    }
    
    pos
    ++;
  }    
    
  if (debug == 1) {
    Serial.print("Method number: ");
    Serial.println(method);
    Serial.print("Red: ");
    Serial.println(redVal);
    Serial.print("Green: ");
    Serial.println(greenVal);
    Serial.print("Blue: ");
    Serial.println(blueVal);
  }
  
  switch(method) {
    // Strip 1 SET
    case 1:
      setColour(1, redVal, greenVal, blueVal);
      break;
      
    case 2:
      fadeTo(1, redVal, greenVal, blueVal);
      break;
      
    case 3:
      setColour(2, redVal, greenVal, blueVal);
      break;
      
    case 4:
      fadeTo(2, redVal, greenVal, blueVal);
      break;
  }
}

/* 
* The red rises from 0 to 10 in ten steps, the green from 
* 0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
* 
* In the real program, the color percentages are converted to 
* 0-255 values, and there are 1020 steps (255*4).
* 
* To figure out how big a step there should be between one up- or
* down-tick of one of the LED values, we call calculateStep(), 
* which calculates the absolute gap between the start and end values, 
* and then divides that gap by 1020 to determine the size of the step  
* between adjustments in the value.
*/
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1. 
*  (R, G, and B are each calculated separately.)
*/
int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* fadeTo() fade to an RGB value
*  It loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/
void fadeTo(int strip, int r, int g, int b) {
  int stepR;
  int stepG;
  int stepB;
  
  switch(strip) {
    case 1:
      stepR = calculateStep(prevR1, r);
      stepG = calculateStep(prevG1, g); 
      stepB = calculateStep(prevB1, b);
      break;
      
    case 2:
      stepR = calculateStep(prevR2, r);
      stepG = calculateStep(prevG2, g); 
      stepB = calculateStep(prevB2, b);
      break;
  }

  for (int i = 0; i <= 1020; i++) {
    switch(strip) {
      case 1:
        redVal1 = calculateVal(stepR, redVal1, i);
        grnVal1 = calculateVal(stepG, grnVal1, i);
        bluVal1 = calculateVal(stepB, bluVal1, i);
        setColour(strip, redVal1, grnVal1, bluVal1);
        break;
        
      case 2:
        redVal2 = calculateVal(stepR, redVal2, i);
        grnVal2 = calculateVal(stepG, grnVal2, i);
        bluVal2 = calculateVal(stepB, bluVal2, i);
        setColour(strip, redVal2, grnVal2, bluVal2);
    }
    
    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  
  // Update current values for next loop
  switch (strip) {
    case 1:
      prevR1 = redVal1; 
      prevG1 = grnVal1; 
      prevB1 = bluVal1;
      break;
      
    case 2:
      prevR2 = redVal2; 
      prevG2 = grnVal2; 
      prevB2 = bluVal2;
      break;
  }
}

void setColour(int strip, int r, int g, int b) {
  switch(strip) {
    case 1:
      analogWrite(rPin1, r);
      analogWrite(gPin1, g);
      analogWrite(bPin1, b);
      break;
      
    case 2:
      analogWrite(rPin2, r);
      analogWrite(gPin2, g);
      analogWrite(bPin2, b);
      break;
  }
}
