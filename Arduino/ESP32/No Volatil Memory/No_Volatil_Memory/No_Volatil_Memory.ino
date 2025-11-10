#include <Preferences.h>

const int botonOnboard = 9;
const int ledOnboard = 15;

bool estadoLED = false;
bool estadoAnteriorBoton = HIGH;

Preferences memoria;  // Objeto para acceder a NVS

void setup() {
  Serial.begin(115200);
  pinMode(botonOnboard, INPUT_PULLUP);
  pinMode(ledOnboard, OUTPUT);

  // Abre espacio en NVS con nombre "config" en modo lectura/escritura
  memoria.begin("config", false);

  // Recupera el estado almacenado (por defecto false si no existe)
  estadoLED = memoria.getBool("led", false);
  digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);
  Serial.println(estadoLED ? "LED ENCENDIDO (recuperado)" : "LED APAGADO (recuperado)");
}

void loop() {
  bool estadoBoton = digitalRead(botonOnboard);

  if (estadoAnteriorBoton == HIGH && estadoBoton == LOW) {
    estadoLED = !estadoLED;
    digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);
    memoria.putBool("led", estadoLED);  // Guarda el nuevo estado
    Serial.println(estadoLED ? "LED ENCENDIDO (guardado)" : "LED APAGADO (guardado)");
    delay(50);  // Debounce
  }

  estadoAnteriorBoton = estadoBoton;
  delay(10);
}
