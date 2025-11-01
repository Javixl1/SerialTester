String input;
int pinBase = 10;  // Por ejemplo, q1 controla el pin 11, q2 el 12, etc.

void setup() {
  Serial.begin(9600);
  for (int i = 1; i <= 4; i++) {
    pinMode(pinBase + i, OUTPUT);  // Configura pines 11 a 13
  }
}

void loop() {
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();  // Elimina espacios y saltos de línea

    // Verifica si el formato es tipo "qX=Y"
    if (input.startsWith("q") && input.indexOf('=') > 1) {
      int igualPos = input.indexOf('=');
      String qNumStr = input.substring(1, igualPos);  // Extrae número después de 'q'
      String valStr = input.substring(igualPos + 1);  // Extrae valor

      int qNum = qNumStr.toInt();
      int val = valStr.toInt();

      if (qNum >= 1 && qNum <= 4 && (val == 0 || val == 1)) {
        int pin = pinBase + qNum;  // q1 → 11, q2 → 12, q3 → 13
        digitalWrite(pin, val);
        Serial.println("OK");
      } else {
        Serial.println("ERROR");
      }
    } else {
      handleSCPI(input);
    }
  }
}

      // Comandos especiales
struct Command {
  const char* code;
  const char* response;
};

Command scpiCommands[] = {
  { "*IDN?", "Identificador del sistema: Arduino Controlador" },
  { "*CLS",  "Sistema limpiado." },
  { "*RST",  "Sistema reiniciado." }
};

void handleSCPI(String input) {
  for (Command cmd : scpiCommands) {
    if (input == cmd.code) {
      Serial.print(cmd.response);
      Serial.println("");
      return;
    }
  }
  Serial.print("Comando no reconocido.");
}
