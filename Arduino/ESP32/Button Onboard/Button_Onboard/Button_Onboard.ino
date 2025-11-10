const int botonOnboard = 9;     // GPIO del botón onboard
const int ledOnboard = 15;      // GPIO del LED onboard

bool estadoLED = false;         // Estado actual del LED
bool estadoAnteriorBoton = HIGH;  // Estado anterior del botón

void setup() {
  Serial.begin(115200);
  pinMode(botonOnboard, INPUT_PULLUP);  // Usa resistencia interna
  pinMode(ledOnboard, OUTPUT);
  digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);  // Inicializa LED
}

void loop() {
  bool estadoBoton = digitalRead(botonOnboard);

  // Detecta flanco de bajada (presión del botón)
  if (estadoAnteriorBoton == HIGH && estadoBoton == LOW) {
    estadoLED = !estadoLED;  // Invierte el estado
    digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);
    Serial.println(estadoLED ? "LED ENCENDIDO" : "LED APAGADO");
    delay(50);  // Pequeño debounce
  }

  estadoAnteriorBoton = estadoBoton;
  delay(10);  // Retardo para estabilidad
}
