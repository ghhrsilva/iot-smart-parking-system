#include <Adafruit_NeoPixel.h>

// **Slot 1 (Left Side) Ultrasonic Sensors**
#define TRIG1 12  
#define ECHO1 14  
#define TRIG2 4   
#define ECHO2 19  

// **Slot 2 (Right Side) Ultrasonic Sensors**
#define TRIG3 27  
#define ECHO3 26  
#define TRIG4 25  
#define ECHO4 33  

// **NeoPixel LED Configuration**
#define LED_PIN  15  
#define NUM_LEDS 6   // 3 LEDs per slot

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);

    pinMode(TRIG3, OUTPUT);
    pinMode(ECHO3, INPUT);
    pinMode(TRIG4, OUTPUT);
    pinMode(ECHO4, INPUT);

    pixels.begin();
    delay(10);
}

// **Read Distance with Timeout**
long readDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 25000); // Timeout to prevent long wait
    if (duration == 0) return 100; // Assume no detection
    return duration * 0.0343 / 2; // Convert to cm
}

// **Update LEDs for Slot 1 (Left Side)**
void updateParkingSlot1(long frontDistance, long sideDistance) {
    // **Default: All LEDs Green (Correctly Parked)**
    pixels.setPixelColor(0, pixels.Color(0, 50, 0)); // Left LED Green
    pixels.setPixelColor(1, pixels.Color(0, 50, 0)); // Middle LED Green
    pixels.setPixelColor(2, pixels.Color(0, 50, 0)); // Right LED Green

    // **Check if a vehicle is present (front sensor detects it)**
    if (frontDistance < 15) {  
        if (sideDistance > 6) {  
            pixels.setPixelColor(2, pixels.Color(50, 0, 0)); // Left LED Red
        }
        if (sideDistance < 3) {  
            pixels.setPixelColor(0, pixels.Color(50, 0, 0)); // Right LED Red
        }
        if (frontDistance < 5) {  
            pixels.setPixelColor(1, pixels.Color(50, 0, 0)); // Middle LED Red
        }
    }

    pixels.show(); // ✅ Moved inside the function**
}


// **Update LEDs for Slot 2 (Right Side)**
void updateParkingSlot2(long frontDistance, long sideDistance) {
    pixels.setPixelColor(3, pixels.Color(0, 255, 0)); // Left LED Green
    pixels.setPixelColor(4, pixels.Color(0, 255, 0)); // Middle LED Green
    pixels.setPixelColor(5, pixels.Color(0, 255, 0)); // Right LED Green

    // **Left Line Crossed → Left LED Red**
    if (sideDistance < 3) {  
        pixels.setPixelColor(3, pixels.Color(255, 0, 0)); // Left LED Red
    }

    // **Right Line Crossed → Right LED Red**
    if (frontDistance < 15 && sideDistance > 6) {  
        pixels.setPixelColor(5, pixels.Color(255, 0, 0)); // Right LED Red
    }

    // **Too Far → Middle LED Red**
    if (frontDistance <= 4) {  
        pixels.setPixelColor(4, pixels.Color(255, 0, 0)); // Middle LED Red
    }
    pixels.show();
}

void loop() {
    long frontDistance1 = readDistance(TRIG1, ECHO1);  
    long sideDistance1 = readDistance(TRIG2, ECHO2);   

    long frontDistance2 = readDistance(TRIG3, ECHO3);  
    long sideDistance2 = readDistance(TRIG4, ECHO4);   

    Serial.print("Slot 1 - Front: ");
    Serial.print(frontDistance1);
    Serial.print(" cm, Side: ");
    Serial.print(sideDistance1);
    Serial.print(" cm || Slot 2 - Front: ");
    Serial.print(frontDistance2);
    Serial.print(" cm, Side: ");
    Serial.println(sideDistance2);

    updateParkingSlot1(frontDistance1, sideDistance1);
    updateParkingSlot2(frontDistance2, sideDistance2);

    pixels.show();
    delay(200); // Faster response
}