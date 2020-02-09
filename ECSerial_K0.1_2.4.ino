// **********************************************************************************
// INNOVATE AUCKLAND Water Quality EC Sensor Version 2.4
// EZO-ECTM Embedded Conductivity Circuit Conductivity = μS/cm
// Adafruit featherM0 with LoRa RFM95, Dallas 18B20 sensors and sleepydog power management
// Includes 2X Dallas 18B20 digital Air and Water temperature sensors
// WD Version 2.4 Published to GitHub
// WD Version 2.3 Added MOSFET power control and grove ultrasonic sensor and library
// WD Version 2.2 Added EC K type Calibration to loop
// WD Version 2.1 Added f_ec = atof(EC); to convert char to float to resolve radio send fault
// WD Version 2.0 Installed Atlas Scientific EC K=0.1 sensor and EZO circuit.
// WD Version 1.3 Installed grove ultrasonic to improve power usage.
// WD Version 1.2 Changed to default LoRa radio spreading factor
// WD Version 1.1 Corrected water temperature and EC calculation
// WD Version 1.0 Initial Build
// **********************************************************************************

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SleepyDog.h>
#include <RH_RF95.h>
#include "Ultrasonic.h"

char* Nodestr = "STRXXX";  // Node ID
#define PWRLOOP 19         // Power down, n X 16sec power down


//************************************ Define Connections ***************************
// Settings for feather32u4 LoRa RFM95
/*
  #define RFM95_CS  8
  #define RFM95_RST 4
  #define RFM95_INT 7
  #define VBATPIN   A9 // voltage divider for battery measurement
  #define led       13
*/

// Settings for featherM0 LoRa RFM95
#define RFM95_RST    4
#define RFM95_CS     8
#define RFM95_INT    3
#define VBATPIN      A7 // voltage divider for battery measurement
#define RF95_FREQ 915.0
#define LED          13

#define SensorPWR    12
#define ONE_WIRE_BUS 6 // Dallas data wire is connected to pin 6


String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product

char sensorstring_array[30];                        //we make a char array
char *EC;                                           //char pointer used in string parsing
char *TDS;                                          //char pointer used in string parsing
char *SAL;                                          //char pointer used in string parsing
char *GRAV;                                         //char pointer used in string parsing

// wd variables told hold string values
char Tstr[10];     // Air Temp
char Wstr[10];     // Water temp
char Dstr[10];     // Distance
char ECstr[10];    // EC Level
char Bstr[10];     // Battery Voltage

char buffer[50];
byte sendLen;

float Temp1, WTemp, Bat, f_ec, Dist;
int distance;


//************************************ Load Drivers ********************************

RH_RF95 rf95(RFM95_CS, RFM95_INT);        // wd load driver instance of the radio and name it "rf95"

OneWire oneWire(ONE_WIRE_BUS);            // Setup a oneWire instance to communicate with any OneWire devices

DallasTemperature sensors(&oneWire);      // Pass our oneWire reference to Dallas Temperature.

//Ultrasonic ultrasonic(10, 11);
Ultrasonic ultrasonic(10);  //grove 3 wire


void setup() {

  //while (!Serial);
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  delay(100);
  Serial1.begin(9600);                               //set baud rate for the software serial port_1 to 9600

  Serial.println("Serial Started");

  pinMode(LED, OUTPUT);        // initialize digital pin as an output.
  pinMode(SensorPWR, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //wd initialise radio
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
 
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // wd set radio tx power and use PA false
  rf95.setTxPower(13, false);
  rf95.printRegisters();  //Print all the RFM95 register values
  /*
      // Start up the Dallas Temperature library
      sensors.begin();
  */

  inputstring.reserve(10);                            // set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           // set aside some bytes for receiving data from Atlas Scientific product
  Serial1.print("Factory");                           // Factory rest EZO-EC Atlas Scientific
  Serial1.print('\r');
  delay(1000);
  Serial1.print("K,0.1");                             // Set K level of the EZO-EC Atlas Scientific product
  Serial1.print('\r');

  digitalWrite(LED, LOW);
  delay(50);
  digitalWrite(LED, HIGH);
  delay(50);
  digitalWrite(LED, LOW);
  delay(50);
  digitalWrite(LED, HIGH);
  delay(50);
  digitalWrite(LED, LOW);
  delay(5000);  //wait for recode

}


/**********wd dtostrf function for feather M0 *************************/
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000)
static char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}
#endif


// used for serial input to the EZO-EC circuit
void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}


/***************** Main Loop *************************/
void loop() {
  // Power Up and Start Sensors
  Serial.println();
  Serial.println("Sensor Start");
  // Power Up Sensors
  digitalWrite(SensorPWR, HIGH);
  delay(500);
  sensors.begin();
  delay(1000);
  //Serial1.print("Wake");                            //send wake string to the Atlas Scientific product
  //Serial1.print('\r');
  Serial1.print("K,0.1");                             // Set K level of the EZO-EC Atlas Scientific product
  Serial1.print('\r');
  delay(1000);


  // Measure Temperatures
  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  Temp1 = sensors.getTempCByIndex(1); //index 0, first sensor, air temp
  //Temp1 = 20; //index 0, first sensor, water temp sesnor
  WTemp = sensors.getTempCByIndex(0); //index 1, second sensor, water temp
  //WTemp = 25; //index 1, second sensor, water temp

  // Measure EC level, call read EZO-EC code
  uint8_t ReadEC;
  for (ReadEC = 0; ReadEC < 100; ReadEC++) {
    ECloop();                                //then call ec sensor function
  }


  // Measure water level, read UltraSonic
  distance = ultrasonic.MeasureInCentimeters();
  //distance = 76;


  // Measure battery
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2; // we divided by 2, so multiply back
  measuredvbat *= 3.3; // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Bat = measuredvbat;


  // convert doubles to char-string, add char-string to buffer array
  dtostrf(Temp1, 1, 1, Tstr);
  dtostrf(WTemp, 1, 1, Wstr);
  dtostrf(distance, 1, 0, Dstr);
  dtostrf(f_ec, 1, 0, ECstr);
  dtostrf(Bat, 1, 1, Bstr);
  //sprintf(buffer, "%s,%s,%s,%s,%s,%s,!", Nodestr, Tstr, Wstr, Dstr, EC, Bstr);
  sprintf(buffer, "%s,%s,%s,%s,%s,%s", Nodestr, Tstr, Wstr, Dstr, ECstr, Bstr);

  // wd send buffer and send length to radio
  sendLen = strlen(buffer);

  rf95.send((uint8_t *) buffer, sendLen);
  rf95.waitPacketSent();
  rf95.sleep();


  // wd serial diagnostics
  Serial.println("  ....................  " );
  Serial.println();
  Serial.print("Temp1: "); Serial.println(Temp1);
  Serial.print("WTemp: "); Serial.println(WTemp);
  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("EC Value: "); Serial.print(EC); Serial.println(" uS/cm2");
  Serial.print("Battery: "); Serial.println(Bat);
  Serial.println();
  Serial.print("Send Data: "); Serial.println(buffer);
  Serial.print("Send Length: "); Serial.println(sendLen);
  Serial.println("  ....................  " );
  Serial.println();
  Serial.println("Going to sleep in one second...");
  delay(1000);

  // To entering low power sleep mode for loop
  Serial.println("Shutdown Loop");
  digitalWrite(SensorPWR, LOW);                         // Power Down Sensors
  Serial1.print("Sleep");                               //send sleep string to the Atlas Scientific product
  Serial1.print('\r');
  uint8_t i;
  for (i = 0; i < PWRLOOP; i++) {
    //Watchdog.sleep();  //enable watchdog only after your sensor and code is tested
    Serial.print(i);    Serial.println(", ");
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(1000); //comment out this delay to reduce power utlisation 
  }
}


void ECloop() {

  if (input_string_complete) {                        //if a string from the PC has been received in its entirety
    Serial1.print(inputstring);                      //send that string to the Atlas Scientific product
    Serial1.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (Serial1.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)Serial1.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }

  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    if (isdigit(sensorstring[0]) == false) {          //if the first character in the string is a digit
      Serial.println(sensorstring);                   //send that string to the PC's serial monitor
    }
    else                                              //if the first character in the string is NOT a digit
    {

      sensorstring.toCharArray(sensorstring_array, 30);   //convert the string to a char array
      EC = strtok(sensorstring_array, ",");               //let's pars the array at each comma
      TDS = strtok(NULL, ",");                            //let's pars the array at each comma
      SAL = strtok(NULL, ",");                            //let's pars the array at each comma
      GRAV = strtok(NULL, ",");                           //let's pars the array at each comma
      /*
            Serial.print("EC:");                                //we now print each value we parsed separately
            Serial.println(EC);                                 //this is the EC value

            Serial.print("TDS:");                               //we now print each value we parsed separately
            Serial.println(TDS);                                //this is the TDS value

            Serial.print("SAL:");                               //we now print each value we parsed separately
            Serial.println(SAL);                                //this is the salinity value

            Serial.print("GRAV:");                              //we now print each value we parsed separately
            Serial.println(GRAV);                               //this is the specific gravity
            Serial.println();                                   //this just makes the output easier to read
      */
      f_ec = atof(EC);                                    //convert the char to a float
    }
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
}
