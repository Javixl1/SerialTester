#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

const int botonOnboard = 9;
const int ledOnboard = 15;

bool estadoLED = false;
bool estadoAnteriorBoton = HIGH;

Preferences prefs;
WebServer server(80);

int baudRate = 115200;
String delimiter = "\n";
String ssid = "";
String pass = "";

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Configuración ESP32-C6</title>
      <style>
        body { font-family: Arial; background: #f0f0f0; padding: 20px; }
        h1 { color: #333; }
        form { background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px #ccc; margin-bottom: 20px; }
        input[type="text"], input[type="number"], input[type="password"] {
          width: 100%; padding: 8px; margin: 10px 0;
        }
        input[type="submit"] {
          background: #007BFF; color: white; padding: 10px 20px;
          border: none; border-radius: 4px; cursor: pointer;
        }
        input[type="submit"]:hover { background: #0056b3; }
      </style>
    </head>
    <body>
      <h1>Configuración Serial</h1>
      <form action="/config" method="POST">
        <label>Baud Rate:</label>
        <input name="baud" type="number" value=")rawliteral" + String(baudRate) + R"rawliteral("><br>
        <label>Delimitador:</label>
        <input name="delim" type="text" value=")rawliteral" + delimiter + R"rawliteral("><br>
        <input type="submit" value="Guardar configuración serial">
      </form>

      <h1>Configuración Wi-Fi</h1>
      <form action="/wifi" method="POST">
        <label>SSID:</label>
        <input name="ssid" type="text" value=")rawliteral" + ssid + R"rawliteral("><br>
        <label>Contraseña:</label>
        <input name="pass" type="password" value=""><br>
        <input type="submit" value="Guardar Wi-Fi">
      </form>
    </body>
    </html>
  )rawliteral";

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
    server.send(200, "text/html", "<h1>Wi-Fi guardado</h1><p>Reinicia para aplicar cambios.</p>");
  } else {
    server.send(400, "text/html", "<h1>Error: faltan datos</h1>");
  }
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

  // Recuperar y conectar a Wi-Fi
  ssid = prefs.getString("ssid", "");
  pass = prefs.getString("pass", "");
  if (ssid != "") {
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

