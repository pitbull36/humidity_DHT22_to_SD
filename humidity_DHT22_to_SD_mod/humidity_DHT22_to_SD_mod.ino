// for temp sensor
#include <DHT.h>
#include <Adafruit_Sensor.h>

// for SD reader
#include <SPI.h>
#include <SD.h>

// DHT SENSOR: Initialize DHT sensor for normal 16mhz Arduino:
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht = DHT(DHTPIN, DHTTYPE);

// SD MODULE: change this to match the CS pin of the SD module
const int chipSelect = 10;

// PUSH BUTTON on the proto board
const int buttonPin = 1;
const int debounceDelay = 10;   // milliseconds to wait until stable

// LEDs on the proto board
const int ledPinWriting = 8;    // proto shield led to signal we are writing on the SD card
const int ledPinButton = 7;     // proto shield led 2 pin for button pressed feedback

// USEFUL VARIABLES

// these will be needed to keep track of the time and decide when to log data and turn on/off the proto shield LEDs
// while still being able to check for the pressed button of the proto shield
unsigned long previousMillis4onoff = 0;     // to keep track of time
long ontime = 59000;                        // led1 on time
long SDwriting = 60000;                     // when to log data on SD
long offtime = 61000;                       // led1 off time

bool on = false;                            // to check if the led was on
bool wrote = false;                         // to check if to write or not
bool flag = false;                          // to signal if the button was pressed (to signal an event: open windows, dehumidifier on, etc.)

File myFile;                                // my log file
int id = 1;                                 // to later increase the ID on the log file


// DEBOUNCE FUNCTION to precisely state when the button was pressed
bool debounce(int pin) {
  bool state;
  bool previousState;
  previousState = digitalRead(pin); // store switch state
  for (int counter = 0; counter < debounceDelay; counter++) {
    delay(1); // wait for 1 millisecond
    state = digitalRead(pin); // read the pin
    if (state != previousState) {
      counter = 0; // reset the counter if the state changes
      previousState = state; // and save the current state
    }
  }
  // here when the switch state has been stable longer than the debounce period
  return state;
}


void setup() {
    // LEDs on the proto board
    pinMode(ledPinWriting, OUTPUT);
    pinMode(ledPinButton, OUTPUT);

    // PUSH BUTTON on the proto board as an input
    pinMode(buttonPin, INPUT);

    // Begin serial communication at a baud rate of 9600
    Serial.begin(9600);
    
    // Setup DHT sensor
    dht.begin();

    // initialise SD card
    Serial.print("Initializing SD card...");

    if (!SD.begin()) {
      Serial.println("initialization failed!");
      return;
    }
    Serial.println("initialization done.");

    // initialise data file with headers
    myFile = SD.open("data.txt", FILE_WRITE);

    // if the file opened okay, write the headers in it
    if (myFile) {
      Serial.print("Initialising data file...");
      // write the headers in one line comma separeted, every time the Arudino starts (ID, Temp, Hum, Flag)
      myFile.print("ID");myFile.print(",");myFile.print("Temperature");myFile.print(",");myFile.print("Humidity");myFile.print(",");myFile.println("Flag");
      // close the file:
      myFile.close();
      Serial.println("Done, all good!");
    }

//  // Check if the file was written, by reading it
//  myFile = SD.open("data.txt");
//  if (myFile) {
//      Serial.println("data.txt:");
//
//    // read from the file until empty
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file
//    myFile.close();
//    } else {
//      // if the file didn't open
//      Serial.println("error opening the file");
//    }
    
}

void loop() {

  // READ THE BUTTON by calling the debounce function and turn on or off the LED1
  if (debounce(buttonPin)) {
    digitalWrite(ledPinButton, LOW);
  } else {
    digitalWrite(ledPinButton, HIGH);
    flag = true;
  }

  // start milliseconds timer
  unsigned long currentMillis = millis();

  // Read the humidity (in %) from the DHT22 sensor
  float h = dht.readHumidity();
  // Read the temperature (in Celsius) from the DHT22 sensor
  float t = dht.readTemperature();

  // Check if everything's good with the reading from the DHT22:
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // ACTION 1: check if the waiting time for the led signalling the writing event passed (ontime), and if it wan't already on turn it on
  if ((currentMillis - previousMillis4onoff >= ontime) && (on == false)) {
    // Serial.println("sono nel ONON"); // debug reasons
    on = true;
    digitalWrite(ledPinWriting, HIGH);
  }

  // ACTION 2: if we didn't log any data yet (bool wrote) after the writing waiting time passed (SDwriting) start writing data
  if ((currentMillis - previousMillis4onoff >= SDwriting) && (wrote == false)) {
    wrote = true;

    // write on the txt the temperature and humidity taken from the sensor every minute
    myFile = SD.open("data.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
      Serial.println("I'm writing"); // for debug reasons

      // now check if flag == true, that is, if the button was pressed to signal an external event
      // if there was an event, write 1 in the "flag" column along with the other information in one line
      // else write 0
      if (flag == true) {
        myFile.print(id);myFile.print(",");myFile.print(t);myFile.print(",");myFile.print(h);myFile.print(",");myFile.println(1);
        // debug on serial
        Serial.print(id);Serial.print(",");Serial.print(t);Serial.print(",");Serial.print(h);Serial.print(",");Serial.println(1);
        // reset the flag boolean
        flag = false;
      } else {
        myFile.print(id);myFile.print(",");myFile.print(t);myFile.print(",");myFile.print(h);myFile.print(",");myFile.println(0);
        // debug on serial
        Serial.print(id);Serial.print(",");Serial.print(t);Serial.print(",");Serial.print(h);Serial.print(",");Serial.println(0);
      }

      // close the file:
      myFile.close();

      // increase ID
      id++;
    }
  }

  // ACTION 3: if the waiting time to turn the led off is passed (offtime), turn off the led and reset all the boolean
  if ((currentMillis - previousMillis4onoff >= offtime)) {
    //Serial.println("sono nel OFFOFF"); // debug reasons
    digitalWrite(ledPinWriting, LOW);
    // reset the previousMillisonoff variable to count to the next interval for all of the 3 actions (led on, writing, led off)
    previousMillis4onoff = currentMillis; // Remember the time
    // reset the boolean variables for the next action
    wrote = false;
    on = false;
  }
}
