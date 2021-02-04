// for temp sens
#include <DHT.h>
#include <Adafruit_Sensor.h>

// for SD reader
#include <SPI.h>
#include <SD.h>

#define DHTPIN 2
#define DHTTYPE DHT22

// this will be needed to keep track of the time and decide when to log data and turn on/off the LED
// while still being able to check for the pressed button
unsigned long previousMillis4Interval = 0; // to keep track of time
unsigned long previousMillis4onoff = 0; // to keep track of time
//unsigned long previousMillis4off = 0; // to keep track of time
long ontime = 59000;               // milliseconds of on-time
long interval = 60000;             // interval for writing data
long offtime = 61000;              // milliseconds of off-time

bool on = false;
bool interv = false;

// DHT SENSOR: Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);

// SD MOUDLE: change this to match your SD shield or module, the CS pin of the SD module
const int chipSelect = 10;

// PUSH BUTTON on the proto board
const int buttonPin = 1;
const int debounceDelay = 10; // milliseconds to wait until stable

// LEDs on the proto board
const int ledPinRed = 8; // the number of the output pin
const int ledPinButton = 7; // the number of the output pin

// USEFUL VARIABLES: initialise ID, will use it later to print it to file
int id = 1;
bool flag = false;
File myFile; // file to write in


// DEBOUNCE FUNCTION
boolean debounce(int pin)
{
 boolean state;
 boolean previousState;
 previousState = digitalRead(pin); // store switch state
 for(int counter=0; counter < debounceDelay; counter++)
 {
 delay(1); // wait for 1 millisecond
 state = digitalRead(pin); // read the pin
 if( state != previousState)
 {
 counter = 0; // reset the counter if the state changes
 previousState = state; // and save the current state
 }
 }
 // here when the switch state has been stable longer than the debounce period
 return state;
}



void setup() 
{
  // LED on the proto board
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinButton, OUTPUT);

  // PUSH BUTTON on the proto board as an input
  pinMode(buttonPin, INPUT);
  
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  // Setup DHT sensor:
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

  // if the file opened okay, write to it:
  if (myFile) {
  Serial.print("Initialising data file...");
  // write the headers every time the Arudino starts (ID, Temp, Hum, Flag)
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

void loop() 
{

  
  // READ THE BUTTON
  if(debounce(buttonPin))
   {
   digitalWrite(ledPinButton, LOW);
   } else {
    digitalWrite(ledPinButton, HIGH);
    flag = true;
   }

  // this is the current time
  unsigned long currentMillis = millis();
  //Serial.println(currentMillis - previousMillis4onoff);
  
  // Read the humidity (in %)
  float h = dht.readHumidity();
  // Read the temperature (in Celsius)
  float t = dht.readTemperature();
  
  // Check if everything's good with the reading from the DHT22:
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if((currentMillis - previousMillis4onoff >= interval) && (interv == false))
  {    
    interv = true;
    //ledState = HIGH;  // turn it on
    Serial.println("sono nel interval e scrivo...");

    // write on the txt the temperature and humidity taken from the sensor every minute
    myFile = SD.open("data.txt", FILE_WRITE);
    
    // if the file opened okay, write to it:
    if (myFile) {
    //Serial.print("Writing to file...");

      // now check if flag == true, that is, if the button was pressed to signal an event
      // if there was an event, write 1 in the "flag" column
      // else write 0
      if (flag == true){
        myFile.print(id);myFile.print(",");myFile.print(t);myFile.print(",");myFile.print(h);myFile.print(",");myFile.println(1);  
        Serial.print(id);Serial.print(",");Serial.print(t);Serial.print(",");Serial.print(h);Serial.print(",");Serial.println(1); 
        flag = false;
        } 
      else {
        myFile.print(id);myFile.print(",");myFile.print(t);myFile.print(",");myFile.print(h);myFile.print(",");myFile.println(0);    
        Serial.print(id);Serial.print(",");Serial.print(t);Serial.print(",");Serial.print(h);Serial.print(",");Serial.println(0);
        }
  
    // close the file:
    myFile.close();

    // increase ID
    id ++;

    //previousMillis4onoff = currentMillis;   // Remember the time
    
    // turn the red led off
    //digitalWrite(8, LOW);
    
    //digitalWrite(ledPin, ledState);    // Update the actual LED
    }
 }
 
  if ((currentMillis - previousMillis4onoff >= ontime) && (on == false))
  {
   on = true;
   Serial.println("sono nel ONON");
   digitalWrite(ledPinRed, HIGH);
    //  // Debug on serial
    //  Serial.print("Humidity: ");
    //  Serial.print(h);
    //  Serial.print("%\n");
    //  Serial.print("Temperature: ");
    //  Serial.print(t);
    //  Serial.print("\n");
    //ledState = LOW;  // Turn it off

    // turn the red led on
    //digitalWrite(8, HIGH);
    
    //previousMillis4onoff = currentMillis;  // Remember the time
    //digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    
  if ((currentMillis - previousMillis4onoff >= offtime)){
  Serial.println("sono nel OFFOFF");
  digitalWrite(ledPinRed, LOW);
  previousMillis4onoff = currentMillis;  // Remember the time
  interv = false;
  on = false;
  }
  
}
