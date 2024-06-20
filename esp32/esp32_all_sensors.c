#include <ESP32Servo.h>

Servo myservo;
int servoPin = 12;
unsigned long lastInputTime = 0;

int buttonPin = 22;
bool pressed = false;

int led = 13;
int digitalPin = 18;
int lastDigitalVal = LOW;
unsigned long lastCheck = 0;
float wheelCircumference = 1.75;
float distancePerMagnet = wheelCircumference / 3;
const long interval = 1000;
int magnetCount = 0;

#define CLK 25
#define DT 33
#define SW 32

int counter = 0;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
float currentSpeedKMH = 0;  // Global variable to store the current speed
float previousSpeedKMH = 0; // Variable to store the previous speed
int lastPrintedCounter = 0; // Variable to store the last printed counter value

void setup()
{
    myservo.attach(servoPin);
    Serial.begin(115200);

    pinMode(led, OUTPUT);
    pinMode(digitalPin, INPUT);
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);
    pinMode(buttonPin, INPUT_PULLUP);

    lastStateCLK = digitalRead(CLK);
}

void loop()
{
    handleMagneticSensor();
    handleRotaryEncoder(currentSpeedKMH);
    handleServoControl();
    handleSerialInput(); // Check for incoming serial data and handle it
}

void handleMagneticSensor()
{
    int digitalVal = digitalRead(digitalPin);

    if (digitalVal == HIGH && lastDigitalVal == LOW)
    {
        magnetCount++;
        digitalWrite(led, HIGH);
    }
    else
    {
        digitalWrite(led, LOW);
    }

    lastDigitalVal = digitalVal;

    unsigned long currentTime = millis();
    if (currentTime - lastCheck >= interval)
    {
        float distance = magnetCount * distancePerMagnet;
        float timeSeconds = (currentTime - lastCheck) / 1000.0;
        float speedMS = distance / timeSeconds;
        currentSpeedKMH = speedMS * 3.6; // Update the global variable

        // Check if the current speed is less than the previous speed
        if (currentSpeedKMH < previousSpeedKMH)
        {
            currentSpeedKMH = 0; // Set the speed to 0 if it's less than the previous speed
        }

        if (currentSpeedKMH != previousSpeedKMH)
        {
            Serial.print(currentSpeedKMH);
            Serial.print("/");
            Serial.println(counter);
            previousSpeedKMH = currentSpeedKMH; // Update the previous speed
        }

        magnetCount = 0;
        lastCheck = currentTime;
    }
}

void handleRotaryEncoder(float speedKMH)
{
    // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);

    // If the current state is different from the last state, a pulse has occurred
    if (currentStateCLK != lastStateCLK)
    {
        // If the DT state is different than the CLK state, then the encoder is rotating CCW
        if (digitalRead(DT) != currentStateCLK)
        {
            counter--;
            if (counter < -3)
                counter = -3; // Limit counter to -3
        }
        else
        { // The encoder is rotating CW
            counter++;
            if (counter > 3)
                counter = 3; // Limit counter to 3
        }
        delay(20);

        // Print only if the counter has changed
        if (counter != lastPrintedCounter)
        {
            Serial.print(0);
            Serial.print("/");
            Serial.println(counter);
            lastPrintedCounter = counter; // Update the last printed counter value
        }
    }

    // Update the last state of CLK
    lastStateCLK = currentStateCLK;
}

void handleServoControl()
{
    bool currentState = digitalRead(buttonPin);
    if (currentState == pressed)
    {
        Serial.println("Hello");
        while (digitalRead(buttonPin) == pressed)
        {
        }
    }
}

void handleSerialInput()
{
    if (Serial.available() > 0)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastInputTime > 200)
        { // Simple debounce check
            // Read the incoming string
            String incomingString = Serial.readString();

            // Print the received string to the serial monitor for debugging
            Serial.println("---------" + incomingString + "----------------");

            // Process the incoming string and perform actions based on it
            if (incomingString == "###" || incomingString == "######" || incomingString == "#####")
            {
                myservo.write(80); // map 270 degrees to 180 degrees servo range
                                   // delay(1000);
            }
            if (incomingString == "&&&")
            {
                myservo.write(20);
                // delay(1000);
            }

            lastInputTime = currentTime; // Reset debounce timer
        }
        while (Serial.available() > 0)
            Serial.read(); // Clear any remaining input
    }
}