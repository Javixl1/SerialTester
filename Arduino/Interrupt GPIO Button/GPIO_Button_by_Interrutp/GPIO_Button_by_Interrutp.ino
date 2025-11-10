// Define the pin numbers for the button and LED
//const int buttonPin = D1;
//const int ledPin = D10;

// THIS CODE HAS NOT TESTED YET

// Define a structure to hold button-related data
struct Button {
    const uint8_t PIN; // Pin number for the button
    uint32_t numberKeyPresses; // Counter for the number of button presses
    bool pressed; // Flag to indicate if the button is currently pressed
};

// Create an instance of the Button structure for the button
Button my_button = {buttonPin, 0, false};

// Interrupt Service Routine (ISR) to handle button presses
void ARDUINO_ISR_ATTR isr(void* arg) {
    Button* s = static_cast<Button*>(arg); // Cast the argument to a Button pointer
    s->numberKeyPresses += 1; // Increment the number of button presses
    s->pressed = true; // Set the pressed flag
}

void setup() {
    Serial.begin(115200);
    pinMode(my_button.PIN, INPUT_PULLUP); // Set the button pin as input with internal pull-up resistor
    attachInterruptArg(my_button.PIN, isr, &my_button, FALLING); // Attach the ISR to the button pin, triggered on falling edge
}

void loop() {
    if (my_button.pressed) { // Check if the button is pressed
        Serial.printf("Button 1 has been pressed %lu times\n", my_button.numberKeyPresses); // Print the number of button presses
        my_button.pressed = false; // Reset the pressed flag
    }

    static uint32_t lastMillis = 0; // Variable to store the last time the interrupt was detached
    if (millis() - lastMillis > 10000) { // Check if 10 seconds have elapsed
        lastMillis = millis(); // Update the last detach time
        detachInterrupt(my_button.PIN); // Detach the interrupt from the button pin
    }
}
