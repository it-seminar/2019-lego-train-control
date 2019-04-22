#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ABYSS";
const char* password = "****";

#define MOTOR_PWM_PIN D1
#define MOTOR_DIR_PIN D3
#define HALL_SENSOR_A_PIN D5
#define HALL_SENSOR_B_PIN D6
#define WIFI_RESET_PIN D7
#define LED_PIN LED_BUILTIN

#define TRAIN_INFORMATION 2
#define TRAIN_CONTROL 3
#define TRAIN_BRAKE 4

#define I_AM_A_TRAIN 1
#define I_AM_A_LEFT_SWITCH 2
#define I_AM_A_RIGHT_SWITCH 3
#define I_AM_A_SEMAPHORE 4


WiFiUDP Udp;
unsigned int localUdpPort = 4711;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char outgoingPacket[255];
char replyA[] = "At hall A";
char replyB[] = "At hall B";
char reply[] = "Hello there!";

int currentSpeed = 0;
int currentDirection = 0;
const int minSpeedEquivalent = 512;
const int maxSpeedEquivalent = 800;
const int maxSpeed = 200;

bool onMagnet = false;
unsigned long onMagnetTime = 0;
int deltaCount = 0;
IPAddress remoteIP = IPAddress(0,0,0,0);
uint16_t remotePort = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(HALL_SENSOR_A_PIN, INPUT_PULLUP);
  pinMode(HALL_SENSOR_B_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_A_PIN), onHallA, FALLING);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_B_PIN), onHallB, FALLING);

  Serial.begin(115200);
  Serial.println();
  digitalWrite(LED_PIN, HIGH);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  digitalWrite(LED_PIN, LOW);
  }


void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    remoteIP = Udp.remoteIP();
    remotePort = Udp.remotePort();
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 1)
    {
      if (incomingPacket[1] == TRAIN_CONTROL) {
        int speed = readInt(incomingPacket, 2);
        Serial.printf("Speed: %d\n", speed);
        applySpeed(speed);
        }
      else if (incomingPacket[1] == TRAIN_BRAKE) {
        applySpeed(0);
        sendTrainInformation(0, 0, 0, 1);
        }
      else {
        incomingPacket[len] = 0;
        Serial.printf("UDP packet contents: %s\n", incomingPacket);
        }

      }

    // send back a reply, to the IP address and port we got the packet from
    }
  }

void broadcastExistence() {
  
  }

int readInt(char buffer[], int index) {
  int result = 0;
  for (int i = index + 3; i >= index; i--) {
    result <<= 8;
    result |= buffer[i];
    }
  return result;
  }

int writeInt(char buffer[], int index, int value) {
  for (int i = index; i < index + 4; i++) {
    buffer[i] = value & 255;
    value >>= 8;
    }
  return index + 4;
  }

void applySpeed(int speed) {
  int d = (maxSpeedEquivalent - minSpeedEquivalent)/maxSpeed;
  int speedEquivalent = minSpeedEquivalent + speed*d;
  if (speed == 0) speedEquivalent = 0;
  digitalWrite(MOTOR_DIR_PIN, currentDirection);
  analogWrite(MOTOR_PWM_PIN, speedEquivalent);
  }

void onHallA() {
  digitalWrite(LED_PIN, HIGH);
  if (onMagnet) return;
  onMagnet = true;
  onMagnetTime = millis();
  }

void onHallB() {
  digitalWrite(LED_PIN, LOW);
  if (remotePort == 0 || !onMagnet) return;
  onMagnet = false;
  onMagnetTime = millis() - onMagnetTime;
  sendTrainInformation((5000/onMagnetTime), 0, 0, 1);
  }

void sendTrainInformation(int speed, int trackId, int distanceToLight, int light) {
  outgoingPacket[0] = 15;
  outgoingPacket[1] = TRAIN_INFORMATION;
  writeInt(outgoingPacket, 2, speed);
  writeInt(outgoingPacket, 6, trackId);
  writeInt(outgoingPacket, 10, distanceToLight);
  outgoingPacket[14] = light;
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write(outgoingPacket, 15);
  Udp.endPacket();
  }
