extern "C" {
 
#include <Arduino.h>

// the setup routine runs once when you press reset:
void setup() {                

    Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {

    char * s = "Hello world!\n";

    for (char *p=s; *p; p++) {
        Serial.write(*p);
    }

}

}
