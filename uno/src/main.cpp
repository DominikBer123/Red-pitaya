#include <SoftwareSerial.h>
#include <SPI.h>
#include <Arduino.h>

// Define the software serial object
SoftwareSerial mySerial(2, 3); // RX, TX



void setup() {
  // Start the serial communication with the default hardware serial (for Serial Monitor)
  Serial.begin(9600);
  
  // Start the software serial communication
  mySerial.begin(9600);

  Serial.println("Arduino Uno Software Serial Example");

  SPI.begin(); // Initialize SPI communication
  SPI.setDataMode(SPI_MODE1); // Set SPI mode 1

  pinMode(8, INPUT); // Set pin 8 as input
}

uint8_t readDistanceSPI()
{
  // Select the slave device (assuming it's active low, adjust based on your hardware)
  digitalWrite(SS, LOW);

  // Send the first byte (0x40)
  SPI.transfer(0x40);

  // Send the second byte (0x00) and receive the response
  byte response = SPI.transfer(0x00);

  Serial.print("Response: ");
  Serial.println(response);

  // Deselect the slave device
  digitalWrite(SS, HIGH);

  return response;
}

void loop() {
  // Read from Serial Monitor and send to the software serial
  /* if (Serial.available()) {
    char data = Serial.read();
    mySerial.print(data);
  } */

  // Read from software serial and send to Serial Monitor
  /* if (mySerial.available()) {
    char data = mySerial.read();
    Serial.print(data);
  } */

  uint8_t distance = 0;

  if(digitalRead(8) == HIGH)
  {
    distance = readDistanceSPI();

    // Send to RP
    mySerial.write(distance);

  }


  //Serial.println("x");
  
}
