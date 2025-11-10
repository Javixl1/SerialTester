String input;
int ledOnboard = 15;              // Pin 15 llamado LedOnboard en tu ESP32-C6

void setup() {
    Serial.begin(115200);         // Se declara la velocidad de comunicacion
    pinMode(ledOnboard, OUTPUT);  // Configura pine 15 como salida
    Serial.setTimeout(1000);      // Tiempo de espera para lectura serial
}

void loop() {
  if (Serial.available()) {
    input = Serial.readStringUntil('\r');  // Cambia a '\n' si tu sistema usa salto de línea
    input.trim();

    if (input.startsWith("q") && input.indexOf('=') > 1) {
      int igualPos = input.indexOf('=');
      String qNumStr = input.substring(1, igualPos);
      String valStr = input.substring(igualPos + 1);

      int qNum = qNumStr.toInt();
      int val = valStr.toInt();

      if (qNum >= 1 && qNum <= 4 && (val == 0 || val == 1)) {
        int pin = ledOnboard + qNum;
        digitalWrite(ledOnboard, val);
        Serial.println("OK:");
      } else {
        Serial.println("ERROR:" + qNum + val);
      }
    } else {
      Serial.println("Valor no Reconocido:");
    }
  }
}
