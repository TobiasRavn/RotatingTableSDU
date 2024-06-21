#include <Arduino.h>
#include <Controllino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <AccelStepper.h>

#define STEPS_PER_REV 200
#define MICROSTEPS 4
#define GEAR_RATIO 2
#define STEPS_PER_DEGREE (double(STEPS_PER_REV * MICROSTEPS * GEAR_RATIO) / 360)
#define MAX_SPEED 200
#define ACCELERATION 100
#define HOME_SPEED 50

#define HOMEING_PIN CONTROLLINO_DI0

#define ENABLE_PIN CONTROLLINO_D0
#define DIR_PIN CONTROLLINO_D1
#define PULL_PIN CONTROLLINO_D2

AccelStepper stepper(AccelStepper::DRIVER, PULL_PIN, DIR_PIN);

// Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//IPAddress ip(10, 126, 69, 220); // Controllino IP - change it if needed (use static IP on your PC - tested with 10.126.69.223)
IPAddress ip(192,168,0,122);
EthernetServer server(80);

// Variables
bool power = false;
bool homed = false;

// Functions
double goToAngle(double targetAngle);
double home(double homeAngle);

void setup() {
  // Setup Stepper Motor
  stepper.setEnablePin(ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setAcceleration(ACCELERATION);
  stepper.disableOutputs();

  // Homeing setup
  pinMode(HOMEING_PIN, INPUT_PULLUP);

  // Ethernet setup
  Ethernet.begin(mac, ip);
  server.begin();

  // Serial setup
  // Serial.begin(9600);
  // Serial.println("Starting Stepper Motor Controller");
  // Serial.print("IP Address: ");
  // Serial.println(Ethernet.localIP());

  // Serial.println("Steps per degree: " + String(STEPS_PER_DEGREE));


}

void loop() {
  // Listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // Serial.println("New client");
    while (client.connected()) {
      if (client.available()) {
        String msg = client.readString();
        // Serial.println("Received message: " + msg);
        // If the message is go to angle
        if (msg.startsWith("goToAngle:")) {
          if (!power) {
            client.print("notPoweredOn");
          } else if (!homed) {
            client.print("notHomed");
          } else {
          double angle = msg.substring(10).toDouble();
          double actualAngle = goToAngle(angle);
          // Serial.println("Moved to " + String(actualAngle) + " degrees");
          client.print("actualAngle:" + String(actualAngle));
          }
        } else if (msg.startsWith("home:")) {
          if (!power) {
            client.print("notPoweredOn");
          } else {
          double homeAngle = msg.substring(5).toDouble();
          double actualHomeAngle = home(homeAngle);
          homed = true;
          // Serial.println("Homed to " + String(actualHomeAngle) + " degrees");
          client.print("homeAngle:" + String(actualHomeAngle));
          }
        } else if (msg.startsWith("getAngle")) {
          if (!power) {
            client.print("notPoweredOn");
            break;
          } else if (!homed) {
            client.print("notHomed");
            break;
          } else {
          double currentAngle = stepper.currentPosition() / STEPS_PER_DEGREE;
          // Serial.println("Current angle " + String(currentAngle) + " degrees");
          client.print("currentAngle:" + String(currentAngle));
          }
        } else if (msg.startsWith("powerOn")) {
          stepper.enableOutputs();
          power = true;
          // Serial.println("Powered on");
          client.print("poweredOn");
        } else if (msg.startsWith("powerOff")) {
          stepper.disableOutputs();
          power = false;
          homed = false;
          // Serial.println("Powered off");
          client.print("poweredOff");
        }
        // Serial.println("Next message");
      }
    }
    client.stop();
    // Serial.println("Client disconnected");
  }
}

double home(double homeAngle) {
  stepper.setMaxSpeed(HOME_SPEED);
  // If the homeing pin is already pressed, move the motor until it is not pressed
  if (digitalRead(HOMEING_PIN) == HIGH) {
    stepper.move(round(-25 * STEPS_PER_DEGREE));
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
  // Move the motor until the homeing pin is pressed
  stepper.moveTo(1000000);
  while (digitalRead(HOMEING_PIN) == LOW) {
    stepper.run();
  }
  stepper.stop();
  stepper.setCurrentPosition(homeAngle * STEPS_PER_DEGREE);
  stepper.setMaxSpeed(MAX_SPEED);
  return double(stepper.currentPosition()) / STEPS_PER_DEGREE;
}

double goToAngle(double targetAngle) {
  double currentAngle = stepper.currentPosition() / STEPS_PER_DEGREE;
  // Calculate the shortest path to the target angle
  double angleDiff = targetAngle - currentAngle;
  if (angleDiff > 180) {
    angleDiff -= 360;
  } else if (angleDiff < -180) {
    angleDiff += 360;
  }
  long targetPosition = stepper.currentPosition() + round(angleDiff * STEPS_PER_DEGREE);
  stepper.moveTo(targetPosition);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  // Return the current angle from 0 to 360
  double actualAngle = stepper.currentPosition() / STEPS_PER_DEGREE;
  while (actualAngle < 0) {
    actualAngle += 360;
  }
  while (actualAngle >= 360) {
    actualAngle -= 360;
  }
  return actualAngle;
}