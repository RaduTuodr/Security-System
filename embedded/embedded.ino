/*****************************************************
   TTP229 Keypad (2-wire) + Laser Tripwire System
   Continuous "1 2 3 4" or "4 3 2 1" detection
*****************************************************/

/* --- Pin Definitions for the TTP229 (2-wire) --- */
#define SCL_PIN 10
#define SDO_PIN 11

/* --- Pin Definitions for Laser Tripwire --- */
const int laserPin   = 12;  // Laser diode
const int sensorPin  = 2;   // Laser sensor input
const int ledPin     = 13;  // LED output
const int buzzerPin  = 4;   // Buzzer output (active-LOW)

/* --- Codes to Enable/Disable the System --- */
const byte ENABLE_CODE[4]  = {1, 2, 3, 4};
const byte DISABLE_CODE[4] = {4, 3, 2, 1};

/* --- Global Variables --- */
bool machineEnabled = false;  
byte enteredKeys[4] = {0,0,0,0}; // Rolling buffer of the last 4 keys pressed

void setup() {
  Serial.begin(9600);

  // Configure pins
  pinMode(laserPin, OUTPUT);
  pinMode(sensorPin, INPUT); // or INPUT_PULLUP if needed
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(SCL_PIN, OUTPUT);
  pinMode(SDO_PIN, INPUT);

  // On startup, keep everything off (disabled)
  digitalWrite(laserPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, HIGH); // Buzzer OFF (inverted logic)

  Serial.println("TTP229 + Laser Tripwire (Inverted Buzzer)");
  Serial.println("Enter 1 2 3 4 to ENABLE, 4 3 2 1 to DISABLE (anywhere in sequence)");
}

void loop() {
  // 1) Check Keypad Input for Enable/Disable
  handleKeypadInput();

  // 2) If Enabled, run laser tripwire
  if (machineEnabled) {
    runLaserTripwire();
  } else {
    // Keep system off if disabled
    digitalWrite(laserPin, LOW);
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, HIGH); // Buzzer OFF (inverted)
  }
}

/*******************************************************
   Read the TTP229 in 2-wire mode.
   Returns: 1..16 if a key is pressed, or 0 if none.
 *******************************************************/
byte readKeypad() {
  byte keyNumber = 0;
  
  // We pulse SCL 16 times, reading SDO each time
  for (byte count = 1; count <= 16; count++) {
    digitalWrite(SCL_PIN, LOW);
    delayMicroseconds(10);

    if (!digitalRead(SDO_PIN)) {
      keyNumber = count;
    }

    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(10);
  }
  return keyNumber;
}

/*******************************************************
   Poll for a key press.
   If pressed, shift in the new key -> check codes.
 *******************************************************/
void handleKeypadInput() {
  byte currentKey = readKeypad();

  // If TTP229 returns 9..16, map to 1..8 (change if your keypad is wired differently)
  if (currentKey >= 9 && currentKey <= 16) {
    currentKey -= 8;
  }

  // If a key was actually pressed
  if (currentKey != 0) {
    // For debugging
    Serial.print("Key Pressed:");
    Serial.println(currentKey);

    // 1) Shift left the old keys to make room
    for (byte i = 0; i < 3; i++) {
      enteredKeys[i] = enteredKeys[i+1];
    }
    // 2) Store the new key in the last position
    enteredKeys[3] = currentKey;

    // 3) Check if the last 4 keys match either code
    if (checkCode(enteredKeys, ENABLE_CODE)) {
      machineEnabled = true;
      Serial.println("MACHINE ENABLED!");
    }
    else if (checkCode(enteredKeys, DISABLE_CODE)) {
      machineEnabled = false;
      Serial.println("MACHINE DISABLED!");
    }

    // Small delay to avoid multiple reads of the same press
    delay(200);
  }
}

/*******************************************************
   Laser tripwire logic:
   - Laser ON
   - If sensorPin is LOW => beam is broken => LED/Buzzer ON
 *******************************************************/
void runLaserTripwire() {
  digitalWrite(laserPin, HIGH);
  
  int sensorState = digitalRead(sensorPin);
  if (sensorState == LOW) {
    // Beam broken
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, LOW);   // Buzzer ON (inverted)
    Serial.println("Beam Broken!");
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, HIGH);  // Buzzer OFF (inverted)
  }

  delay(50); // small debounce
}

/*******************************************************
   Compare two 4-byte arrays to see if they match
 *******************************************************/
bool checkCode(const byte entered[4], const byte correct[4]) {
  for (byte i = 0; i < 4; i++) {
    if (entered[i] != correct[i]) {
      return false;
    }
  }
  return true;
}
