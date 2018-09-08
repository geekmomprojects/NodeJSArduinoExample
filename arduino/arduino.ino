
/* 
 *  Sketch that sends potentiometer values out through the serial port, and turns an LED on/off depending
 *  on values read in through the serial port.
 */

// LED Light Pin
const int ledPin = 6;

// Potentiometer Pin
const int potPin = A0;

// simple variables used to store the LED status
String ledStatus = "off";

// Store the most recent value of the potentiometer
int lastPotVal = 0;

// String to hold all the messages coming from the nodejs server
String inputString = "";

// whether the string received form nodejs is complete
// a string is considered complete by the carriage return '\r'
boolean stringComplete = false;

/**
 *
 * arduino board setup
 *
 */

void setup()
{
  // set the Baud Rate
  Serial.begin(115200);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // Initialize potentiometer pin as input
  pinMode(potPin, INPUT);
}

/**
 *
 * Default arduino loop function
 * it runs over and over again
 *
 */

void loop()
{
  // read the button state and send a message to the nodejs server
  listenPotChanges();
  // update the status of the led light connected to the arduino board
  updateLedStatus();
}


/**
 *  Send an update of the potentiomter value if it changes by more than the threshold (we use
 *  a threshold so as not to get swamped with readings due to random noise fluctuations.
 */
void listenPotChanges() {
  const int THRESHOLD = 10;
  int potVal = analogRead(potPin);
  if (abs(potVal - lastPotVal) > THRESHOLD) {
    Serial.print("Pot:");
    Serial.println(potVal);
    delay(50);  // Delay so we don't swamp serial port
    lastPotVal = potVal;
  }
}

/**
 *
 * Update the status of the led light reading the messages dispatched by the nodejs server
 *
 */

void updateLedStatus() {
  // detect whether the string has been completely received
  if (stringComplete) {
    // set and store the current led status
    if (inputString == "on\r") {
      ledStatus = "on";
    }
    if (inputString == "off\r") {
      ledStatus = "off";
    }
    // send the light status to the nodejs server
    Serial.println(ledStatus);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  // turn on or off the led according to the latest light status stored
  digitalWrite(ledPin, ledStatus == "on" ? HIGH : LOW);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
 void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a "\n" we detect the end of the string
    if (inChar == '\r') {
      stringComplete = true;
    }
  }
}
