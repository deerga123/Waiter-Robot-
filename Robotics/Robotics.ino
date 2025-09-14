
// IR Sensors
const int irLeft = 3;
const int irRight = 2;

// Ultrasonic
const int trigPin = A0;
const int echoPin = A1;

// Motors
const int rightDir = 4;
const int rightPWM = 5;
const int leftPWM  = 6;
const int leftDir  = 7;

// Colour Sensor 
const int S0 = 9, S1 = 10, S2 = 12, S3 = 11;
const int colorOut = 13, colorLED = 8;

// Buttons & Tray Switch
const int button1 = A2;
const int button2 = A3;
const int button3 = A4;
const int traySwitch = A5;

// === Variables ===
String targetTable = "";
bool orderReady = false;

// === Functions ===

// Motor control
void forward(int speed = 150) {
  digitalWrite(rightDir, HIGH);
  digitalWrite(leftDir, HIGH);
  analogWrite(rightPWM, speed);
  analogWrite(leftPWM, speed);
}

void stopMotors() {
  analogWrite(rightPWM, 0);
  analogWrite(leftPWM, 0);
}

// Ultrasonic distance
float getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // timeout
  float distance = duration * 0.034 / 2;
  return distance;
}

// Detect colour 
String detectColor() {
  unsigned long r, g, b;

  // RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  r = pulseIn(colorOut, LOW);

  // GREEN
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  g = pulseIn(colorOut, LOW);

  // BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  b = pulseIn(colorOut, LOW);

  if (r < g && r < b) return "RED";
  if (g < r && g < b) return "GREEN";
  if (b < r && b < g) return "BLUE";
  return "UNKNOWN";
}

void setup() {
  Serial.begin(9600);

  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(rightDir, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(leftPWM, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(colorOut, INPUT);
  pinMode(colorLED, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(traySwitch, INPUT_PULLUP);

 
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(colorLED, HIGH);

  stopMotors();
}


void loop() {
  // Kitchen buttons
  if (digitalRead(button1) == LOW) { targetTable = "RED"; orderReady = true; }
  if (digitalRead(button2) == LOW) { targetTable = "GREEN"; orderReady = true; }
  if (digitalRead(button3) == LOW) { targetTable = "BLUE"; orderReady = true; }


  if (orderReady) {
    if (digitalRead(traySwitch) == HIGH) forward();
    else stopMotors();
  }

  // IR obstacle detection
  if (digitalRead(irLeft) == LOW || digitalRead(irRight) == LOW) stopMotors();

  // Ultrasonic detection
  if (getDistanceCM() < 5) stopMotors();

  // Color table detection
  if (orderReady) {
    String currentColor = detectColor();
    if (currentColor == targetTable) {
      stopMotors();
      Serial.println("Place tray -> Switch ON to confirm");
      while (digitalRead(traySwitch) == LOW) delay(50);

      Serial.println("Tray placed -> Returning to kitchen");
      forward();
    }
  }
}
