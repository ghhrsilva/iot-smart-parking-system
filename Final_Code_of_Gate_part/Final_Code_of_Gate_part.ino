#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Servo.h>

#define TRIG1 12  // D6 -> GPIO12 (Entrance sensor TRIG)
#define ECHO1 13  // D7 -> GPIO13 (Entrance sensor ECHO)
#define TRIG2 14  // D5 -> GPIO14 (Exit sensor TRIG)
#define ECHO2 15  // D8 -> GPIO15 (Exit sensor ECHO)
#define SERVO_PIN 16 // D0 -> GPIO16 (Servo motor)
#define GREEN_LED 2  // D4 -> GPIO2 (Green LED for gate open)
#define BLUE_LED 0   // D3 -> GPIO0 (Blue LED for gate closed)

LiquidCrystal_PCF8574 lcd(0x27);
Servo gateServo;
bool gateOpen = false;
unsigned long lastVehicleTime = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(4, 5); // SDA = GPIO4 (D2), SCL = GPIO5 (D1)
    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.setCursor(0, 0);
    lcd.print("                "); // Clear first line
    lcd.setCursor(0, 1);
    lcd.print("System Ready");

    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);

    gateServo.attach(SERVO_PIN);
    gateServo.write(0); // Ensure gate starts fully closed
    digitalWrite(BLUE_LED, HIGH); // Blue LED ON (Gate closed)
    digitalWrite(GREEN_LED, LOW);
    delay(500);
    closeGate();
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 1);
}

void loop() {
    static unsigned long lastScrollTime = 0;
    static int scrollIndex = 0;
    String message = "- Welcome to SPOTFINDERS Parking System -";

    long distance1 = getDistance(TRIG1, ECHO1);
    long distance2 = getDistance(TRIG2, ECHO2);

    Serial.print("Entrance: "); Serial.print(distance1);
    Serial.print(" cm | Exit: "); Serial.println(distance2);

    if (distance1 < 14 || distance2 < 14) {  // Vehicle detected
        lastVehicleTime = millis(); // Reset vehicle timer
        openGate();
    } else if (millis() - lastVehicleTime > 2000) { // No vehicle for 2 seconds
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
    } else if (distance1 >= 14 && distance2 >= 14) {  // Vehicle has passed both sensors
        closeGate();
    }

    // Scroll welcome message on first line continuously
    if (millis() - lastScrollTime > 300) {
        lastScrollTime = millis();
        lcd.setCursor(0, 0);
        lcd.print(message.substring(scrollIndex, scrollIndex + 16));
        scrollIndex++;
        if (scrollIndex > message.length() - 16) {
            scrollIndex = 0;
        }
    }

    delay(500);
}

void openGate() {
    if (!gateOpen) {
        gateServo.write(180);

        lcd.setCursor(0, 1);
        lcd.print("Gate Opening...");  // Show "Gate Opening..."
        Serial.println("Gate Opening...");
        digitalWrite(BLUE_LED, LOW); // Turn OFF Blue LED
        digitalWrite(GREEN_LED, HIGH); // Turn OFF Green LED

        gateOpen = true;
    }
}

void closeGate() {
    if (gateOpen) {
        gateServo.write(0);

        lcd.setCursor(0, 1);
        lcd.print("Gate Closing...");  // Show "Gate Closing..."
        Serial.println("Gate Closing...");
        digitalWrite(BLUE_LED, HIGH); // Blue LED ON
        digitalWrite(GREEN_LED, LOW);
        delay(1000);  // Wait 2 seconds for closing effect

        lcd.setCursor(0, 1);
        lcd.print("Gate Closed...  ");  // Change to "Gate Closed..."
        delay(2000);  // Wait 2 seconds to show "Gate Closed..."

        lcd.setCursor(0, 1);
        lcd.print("                ");  // Clear the second line

        gateOpen = false;
    }
    digitalWrite(BLUE_LED, LOW); // Blue LED ON

}

long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}