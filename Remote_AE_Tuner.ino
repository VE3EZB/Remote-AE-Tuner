/* Coded by VE3EZB - Steve */
// These are the pins that corespond to the "Data" pins on the NodeMCU 1.0(ESP12E Module)
//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3;
//static const uint8_t D10  = 1;
#include <ArduinoJson.h>
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "index.h"

#define ccwesPin 12 // define CCW end stop pin as pin 12 (D6)
#define cwesPin 14  // define CW end stop pin as pin 14 (D5)


 
// set steps per revolution for you motor / Nema 17 is 200
const int stepsPerRevolution = 200;
 
// setup client switch logic
bool stopFlag = false;
//bool stopButtonPressed = false;

// initialize the stepper library on pins D1 through D4
Stepper myStepper(stepsPerRevolution, D1, D2, D3, D4);

int currentPosition = 0; // used to count the number of step the motor has moved and report it as a position

//initialize and declare global variable

const char* ssid = "your_ssid";       // Edit for your SSID
const char* password = "your_password";   // Edit for your password
String mDir;
String Limit;


// Start the webserver
ESP8266WebServer server(80);    

// send webpage (index.h) to client-side (browser) when requested
void handleRoot() {
    String s = webpage;  
    server.send(200, "text/html", s);
    
}
// Declare functions to be used
void saveMotorPosition(int currentPosition) {
  // Break down the integer into bytes
  byte byte1 = (currentPosition >> 24) & 0xFF;
  byte byte2 = (currentPosition >> 16) & 0xFF;
  byte byte3 = (currentPosition >> 8) & 0xFF;
  byte byte4 = currentPosition & 0xFF;

  // Write each byte to EEPROM
  EEPROM.write(0, byte1);
  EEPROM.write(1, byte2);
  EEPROM.write(2, byte3);
  EEPROM.write(3, byte4);

  EEPROM.commit();
  Serial.println("Saving position: " + String(currentPosition));
}

int getMotorPosition() {
  // Read each byte from EEPROM
  byte byte1 = EEPROM.read(0);
  byte byte2 = EEPROM.read(1);
  byte byte3 = EEPROM.read(2);
  byte byte4 = EEPROM.read(3);

  // Combine the bytes back into an integer
  int currentPosition = ((byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4);
  Serial.println("Retrieved position: " + String(currentPosition));
  return currentPosition;
}

void updatePosition() {
    String jsonResponse = "{\"position\":" + String(currentPosition) + "}";
    server.send(200, "application/json", jsonResponse);
}

// Handle STOP button
void handleButtonSTOP() {
    stopFlag = true;
    
    myStepper.step(0); // dirStep causes the motor to Stop
    mDir = "STOPPED - don't forget to turn off power if the antenna is tuned.";
    
    bool cwEndStopActivated = (digitalRead(cwesPin) == LOW);
    bool ccwEndStopActivated = (digitalRead(ccwesPin) == LOW);
    
    if (cwEndStopActivated) {
        mDir = "MOTOR STOPPED";
        currentPosition = 0;
        saveMotorPosition(currentPosition);
    } else if (ccwEndStopActivated) {
        mDir = "MOTOR STOPPED";
        saveMotorPosition(currentPosition);
    } else {
        // Neither end stop is activated, so save the motor position
        updatePosition();
        saveMotorPosition(currentPosition);
    }


    // Send end stop information to the client
    String jsonResponse = "{\"mDir\":\"" + mDir + "\"," +
                    "\"Limit\":\"" + Limit + "\"," +
                    "\"cwEndStopActivated\":" + (cwEndStopActivated ? "true" : "false") + "," +
                    "\"ccwEndStopActivated\":" + (ccwEndStopActivated ? "true" : "false") + 
                    "}";
    server.send(200, "application/json", jsonResponse);
}

// Handle CCW button
void handleButtonCCW() {
  stopFlag = false; // reset the flag

  while (digitalRead(ccwesPin) == HIGH && !stopFlag) {  // if CCW endstop not activated AND the STOP button not pressed    
          
    mDir = " ";
    Limit = " ";
    myStepper.step(1); // move motor CCW
    currentPosition += 1; // counts the number of steps 
    delay(10);  // Add a small delay for stability
    updatePosition();

    // Handle any incoming client requests
    server.handleClient();

    if (digitalRead(ccwesPin) == LOW)
        break;
  }

  // Send end stop information to the client
 String jsonResponse = "{\"ccwEndStop\":\"" + String(digitalRead(ccwesPin)) + "\"}";
    server.send(200, "application/json", jsonResponse);
        
    handleButtonSTOP(); // call function to stop motor
}

// Handle CW button
void handleButtonCW() {
  stopFlag = false; // reset the flag

  while (digitalRead(cwesPin) == HIGH && !stopFlag) {  // if CW endstop not activated AND the STOP button not pressed    
      
    mDir = " ";
    Limit = " ";
    myStepper.step(-1); // move motor CW
    currentPosition -= 1; // counts the number of steps 
    delay(10);  // Add a small delay for stability
    updatePosition();

    // Handle any incoming client requests
    server.handleClient();

    if (digitalRead(cwesPin) == LOW)
        break;
  }

  // Send end stop information to the client
  String jsonResponse = "{\"cwEndStop\":\"" + String(digitalRead(cwesPin)) + "\"}";
    server.send(200, "application/json", jsonResponse);
        
    handleButtonSTOP(); // call function to stop motor
}

// Add a function to send initial end stop information to the client
void sendInitialEndStopStatus() {
    bool cwEndStopActivated = (digitalRead(cwesPin) == LOW);
    bool ccwEndStopActivated = (digitalRead(ccwesPin) == LOW);

      // Use String constructor to concatenate values
      String jsonResponse = String("{\"cwEndStopActivated\":") + (cwEndStopActivated ? "true" : "false") + ","
                            "\"ccwEndStopActivated\":" + (ccwEndStopActivated ? "true" : "false") + "}";
    server.send(200, "application/json", jsonResponse);
}

// Handle Reset button
void handleButtonReset() {
        Serial.println("Reset Button Pressed");
        mDir = "Server Soft-Rebooting ... Please wait.";

        // Send all information to the client in a single JSON response
       String jsonResponse = "{\"mDir\":\"" + String(mDir) + "\"}";
    server.send(200, "application/json", jsonResponse);
        
    delay(6000);
        
    ESP.restart();
}

void setup() { 

  EEPROM.begin(64); // Initialize 64 bytes of EEPROM

/*//  use the code below to initialize a new board
  for (int i = 0; i < 64; i++) { // This will write 0 to all 64 bytes of the EEPROM
  EEPROM.write(i, 0);
}
EEPROM.commit(); // This is necessary for ESP8266 and ESP32 boards to store data
//  use the code above to initialize a new board */


  delay(2000);
  int savedPosition = getMotorPosition();
  Serial.println("Saved Position: " + String(savedPosition));
  currentPosition = savedPosition;
  Serial.println("Current Position: " + String(currentPosition));

  pinMode(ccwesPin, INPUT_PULLUP);  // Set ccwesPin as input
  pinMode(cwesPin, INPUT_PULLUP);   // Set cwesPin as input
 

// initialize serial monitor
  delay (3000);
  Serial.begin(9600); 
  delay(1000);
  Serial.println(" ");
  Serial.println("Remote Antenna Tuner");
  Serial.println("developed by Steve VE3EZB");

// start WiFi service
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
// Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print("Connecting...");
  }
// Notify that server WiFi is connected
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//multicast DNS so you don't require the IP address to connect
  if (MDNS.begin("remote_tuner")) { // "remote_tuner" can be changed to anything you want
    Serial.println("MDNS responder started");
    Serial.println("access via http://remote_tuner");
  }

// start the server
    server.begin();

// Add a call to send initial end stop status when the server starts
    sendInitialEndStopStatus();

// setup server to handle requests
server.on("/", handleRoot);
server.on("/buttonCCW", HTTP_GET, handleButtonCCW);
server.on("/buttonCW", HTTP_GET, handleButtonCW);
server.on("/buttonSTOP", HTTP_GET, handleButtonSTOP);
server.on("/buttonReset", HTTP_GET, handleButtonReset);
server.on("/position", HTTP_GET, updatePosition);
server.on("/indicator", HTTP_GET, []() {
  String jsonResponse = String("{") +
                        String("\"cwEndStopActivated\": ") + String(digitalRead(cwesPin) == LOW ? "true" : "false") +
                        String(",\"ccwEndStopActivated\": ") + String(digitalRead(ccwesPin) == LOW ? "true" : "false") + "}";
  server.send(200, "application/json", jsonResponse);
});


server.on("/motor", HTTP_GET, []() {
  String jsonResponse = String("{\"mDir\": \"") + String(mDir) + String("\", \"Limit\": \"") + String(Limit) + String("\"}");
server.send(200, "application/json", jsonResponse.c_str()); // <-- Use c_str() to pass a const char*
});
 
server.onNotFound([]() -> void {
  if (server.hasArg("value")) {
    String buttonValueStr = server.arg("value");  // Get the value of the button press from the request URL
    int buttonValue = server.arg("value").toInt();  // Convert the string to an integer

    if (isnan(buttonValue)) {
      server.send(400, "text/plain", "400: Invalid Request - Value is not a number");
      return;
    }

    switch (buttonValue) {
      case 1:
        handleButtonCCW();
        break;
      case 2:
        handleButtonCW();
        break;
      case 3:
        handleButtonSTOP();
        break;
      case 4:
        handleButtonReset();
        break;
    }
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "text/plain", "400: Invalid Request");
  }
});

}

void loop() {
    server.handleClient();
    MDNS.update();
    
}
    
