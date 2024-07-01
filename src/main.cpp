#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Define pin numbers
const int ledPin = 2;             // GPIO2 for LED controlled by potentiometer (yellow2 LED)
const int potentiometerPin = 34;  // GPIO34 for potentiometer (Analog input)
const int toggleLedPin = 16;      // GPIO16 for the toggle LED
const int buttonPin = 33;         // GPIO33 for the toggle LED button (yellow1 LED)
const int redLedPin = 26;         // GPIO26 for the red LED
const int redButtonPin = 18;      // GPIO18 for the red LED button

// Wi-Fi credentials
const char* ssid = "Meru";
const char* password = "6235deba0b5949c7";

// Create an instance of the server
WebServer server(80);

// Variables to store potentiometer value and time
int previousMappedValue = -1;
bool redLedState = false;          // State of the red LED
bool potentiometerActive = true;   // Flag to indicate if potentiometer control is active

void handleRoot() {
  server.send(200, "text/plain", "ESP32 is running");
}

void handleToggleRedLed() {
  redLedState = !redLedState;
  digitalWrite(redLedPin, redLedState ? HIGH : LOW);
  server.send(200, "text/plain", "Red LED toggled");
}

void handleToggleYellowLed() {
  digitalWrite(toggleLedPin, !digitalRead(toggleLedPin));
  server.send(200, "text/plain", "Yellow LED toggled");
}

void handleSetPotentiometerValue() {
  if (server.hasArg("value")) {
    int value = server.arg("value").toInt();
    int ledBrightness = map(value, 0, 100, 0, 255);
    analogWrite(ledPin, ledBrightness);
    server.send(200, "text/plain", "Potentiometer value set");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  // Initialize LED pins as outputs
  pinMode(ledPin, OUTPUT);
  pinMode(toggleLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // Initialize button pins as inputs with internal pull-up resistors
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redButtonPin, INPUT_PULLUP);

  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Adjust potentiometer to control LED brightness. Press button to toggle LEDs.");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to Wi-Fi. IP address: ");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/", handleRoot);
  server.on("/toggleRedLed", handleToggleRedLed);
  server.on("/toggleYellowLed", handleToggleYellowLed);
  server.on("/setPotentiometerValue", handleSetPotentiometerValue);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void resetSystem() {
  // Turn off all LEDs
  digitalWrite(ledPin, LOW);
  digitalWrite(toggleLedPin, LOW);
  digitalWrite(redLedPin, LOW);

  // Reset flags and variables
  previousMappedValue = -1;
  redLedState = false;
  potentiometerActive = true;
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Check if the red LED button is pressed
  if (digitalRead(redButtonPin) == LOW) {
    // Toggle the red LED state
    redLedState = !redLedState;
    digitalWrite(redLedPin, redLedState ? HIGH : LOW);

    if (redLedState) {
      // Disable potentiometer control and toggle LED (yellow1 LED)
      potentiometerActive = false;
      digitalWrite(toggleLedPin, LOW);
      Serial.println("Red LED turned ON. Potentiometer and toggle LED control disabled.");
    } else {
      // Reset the system if red LED is turned off
      resetSystem();
      Serial.println("Red LED turned OFF. System reset.");
    }

    delay(200); // Debounce delay
  }

  // Check if the toggle LED button is pressed
  if (digitalRead(buttonPin) == LOW && !redLedState) {
    // Toggle the toggle LED state
    digitalWrite(toggleLedPin, !digitalRead(toggleLedPin));
    delay(200); // Debounce delay
  }

  // Allow potentiometer control only if neither button is active
  if (!redLedState && potentiometerActive) {
    // Read potentiometer value
    int potValue = analogRead(potentiometerPin);

    // Reverse the potentiometer value and map it to the range 0-255 for PWM
    int reversedPotValue = 4095 - potValue;
    int ledBrightness = map(reversedPotValue, 0, 4095, 0, 255);

    // Update LED brightness based on the potentiometer value
    analogWrite(ledPin, ledBrightness);

    // Print potentiometer value if it has changed
    int mappedValue = map(reversedPotValue, 0, 4095, 0, 100);
    if (mappedValue != previousMappedValue) {
      Serial.print("Potentiometer value: ");
      Serial.println(mappedValue);
      previousMappedValue = mappedValue; // Update previous value
    }
  }

  // Delay for stability
  delay(10);  // Adjust delay as needed
}
