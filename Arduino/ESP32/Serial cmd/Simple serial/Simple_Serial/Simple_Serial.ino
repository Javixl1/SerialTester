// To activate this option you need enable USB CDC option in tool menu >> USB CDCD on Boot
// before use it

int contador = 0;
const int LED_onboard = 15;

void setup() {
  Serial.begin(115200);
  pinMode(LED_onboard, OUTPUT);
}

void loop() {
  Serial.print("Contador: ");
  Serial.println(contador++);
  digitalWrite(LED_onboard, HIGH);
  delay(1000);
  digitalWrite(LED_onboard, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
