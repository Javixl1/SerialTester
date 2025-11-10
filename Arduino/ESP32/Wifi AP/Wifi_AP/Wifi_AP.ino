#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

const int botonOnboard = 9;
const int ledOnboard = 15;

bool estadoLED = false;
bool estadoAnteriorBoton = HIGH;
bool primeraConfiguracion = false;

Preferences prefs;
WebServer server(80);

int baudRate = 115200;
String delimiter = "\n";
String ssid = "";
String pass = "";

void handleRoot() {
  String html;

  if (primeraConfiguracion) {
    html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Configuración Wi-Fi</title></head>
      <body>
        <h1>Primera configuración Wi-Fi</h1>
        <form action="/wifi" method="POST">
          SSID: <input name="ssid" type="text"><br>
          Contraseña: <input name="pass" type="password"><br>
          <input type="submit" value="Guardar Wi-Fi">
        </form>
      </body>
      </html>
    )rawliteral";
  } else {
    html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Panel de configuración</title></head>
      <body>
        <h1>Configuración Serial</h1>
        <form action="/config" method="POST">
          Baud Rate: <input name="baud" type="number" value=")rawliteral" + String(baudRate) + R"rawliteral("><br>
          Delimitador: <input name="delim" type="text" value=")rawliteral" + delimiter + R"rawliteral("><br>
          <input type="submit" value="Guardar configuración serial">
        </form>

        <h1>Configuración Wi-Fi</h1>
        <form action="/wifi" method="POST">
          SSID: <input name="ssid" type="text"><br>
          Contraseña: <input name="pass" type="password"><br>
          <input type="submit" value="Actualizar Wi-Fi">
        </form>
      </body>
      </html>
    )rawliteral";
  }

  server.send(200, "text/html", html);
}

void handleConfig() {
  if (server.hasArg("baud")) {
    baudRate = server.arg("baud").toInt();
    prefs.putInt("baud", baudRate);
  }
  if (server.hasArg("delim")) {
    delimiter = server.arg("delim");
    prefs.putString("delim", delimiter);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleWiFi() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    ssid = server.arg("ssid");
    pass = server.arg("pass");
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);

    String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Wi-Fi guardado</title></head>
      <body>
        <h1>Wi-Fi guardado correctamente</h1>
        <form action="/reboot" method="POST">
          <input type="submit" value="Reiniciar dispositivo">
        </form>
      </body>
      </html>
    )rawliteral";

    server.send(200, "text/html", html);
  } else {
    server.send(400, "text/html", "<h1>Error: faltan datos</h1>");
  }
}

void handleReboot() {
  server.send(200, "text/html", "<h1>Reiniciando...</h1>");
  delay(1000);
  ESP.restart();
}

void setup() {
  prefs.begin("config", false);

  // Recuperar configuración serial
  baudRate = prefs.getInt("baud", 115200);
  delimiter = prefs.getString("delim", "\n");
  Serial.begin(baudRate);
  delay(100);
  Serial.println("Configuración serial aplicada:");
  Serial.println("Baud rate: " + String(baudRate));
  Serial.println("Delimitador: " + delimiter);

  // Recuperar estado del LED
  estadoLED = prefs.getBool("led", false);
  pinMode(ledOnboard, OUTPUT);
  digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);

  // Recuperar Wi-Fi
  ssid = prefs.getString("ssid", "");
  pass = prefs.getString("pass", "");

  if (ssid == "") {
    primeraConfiguracion = true;
    WiFi.softAP("ESP32C6_Config", "12345678");
    Serial.println("Modo AP iniciado: ESP32C6_Config");
    Serial.println("Conéctate y accede a http://192.168.4.1");
  } else {
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("Conectando a Wi-Fi...");
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
      delay(500);
      Serial.print(".");
      intentos++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConectado a Wi-Fi: " + WiFi.localIP().toString());
    } else {
      Serial.println("\nNo se pudo conectar a Wi-Fi");
    }
  }

  pinMode(botonOnboard, INPUT_PULLUP);

  // Servidor web
  server.on("/", handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  server.on("/wifi", HTTP_POST, handleWiFi);
  server.on("/reboot", HTTP_POST, handleReboot);
  server.begin();
}

void loop() {
  server.handleClient();

  bool estadoBoton = digitalRead(botonOnboard);
  if (estadoAnteriorBoton == HIGH && estadoBoton == LOW) {
    estadoLED = !estadoLED;
    digitalWrite(ledOnboard, estadoLED ? HIGH : LOW);
    prefs.putBool("led", estadoLED);
    Serial.println(estadoLED ? "LED ENCENDIDO (guardado)" : "LED APAGADO (guardado)");
    delay(50);
  }
  estadoAnteriorBoton = estadoBoton;
  delay(10);
}
