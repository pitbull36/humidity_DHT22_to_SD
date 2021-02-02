// for temp sens
#include <DHT.h>
#include <Adafruit_Sensor.h>

// for SD reader
#include <SPI.h>
#include <SD.h>

#define DHTPIN 2
#define DHTTYPE DHT22

File myFile; // file to write

// Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);

// change this to match your SD shield or module, the CS pin of the SD module;
const int chipSelect = 10;

// initialise ID, will use it later to print it to file
int id = 1;

void setup() 
{
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  // Setup sensor:
  dht.begin();

  // initialise SD card
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // initialise file with headers
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
  Serial.print("Writing to file...");
  // write the headers every time the Arudino starts
  myFile.print("ID");myFile.print(",");myFile.print("Temperature");myFile.print(",");myFile.println("Humidity");

  // close the file:
  myFile.close();
  }

//  // re-open the file for reading:
//  myFile = SD.open("test.txt");
//  if (myFile) {
//    Serial.println("test.txt:");
//
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//    } else {
//      // if the file didn't open, print an error:
//      Serial.println("error opening test.txt");
//    }
    
}

void loop() 
{ 
  // Take measurements and write them onto the SD every 1 minute
  delay(60000);
  
  // Read the humidity (in %)
  float h = dht.readHumidity();
  // Read the temperature (in Celsius)
  float t = dht.readTemperature();
  
  // Check if everything's good with the reading from the DHT22:
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

//  // Debug on serial
//  Serial.print("Humidity: ");
//  Serial.print(h);
//  Serial.print("%\n");
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.print("\n");
  
  // write on the txt the temperature and humidity taken from the sensor every minute
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
  //Serial.print("Writing to file...");
  myFile.print(id);myFile.print(",");myFile.print(t);myFile.print(",");myFile.println(h);

  // close the file:
  myFile.close();

  // increase ID
  id ++;
  
  //Serial.println("done.");
  } //else {
  // if the file didn't open, print an error:
  //Serial.println("error opening the file");
  //}  
}
