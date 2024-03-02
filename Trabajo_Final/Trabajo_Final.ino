#include <Arduino.h>
void defuse() {
enum class Task1States {
    INIT,
    CONFIG,
    WAIT_DATA
};

static auto task1State = Task1States::INIT;
static char password[5];
static uint8_t dataCounter = 0;
static char result[2];
static uint8_t numbersPassword = 0; // Falso


    switch (task1State) {
        case Task1States::INIT: {
            Serial.begin (115200);
            task1State = Task1States::CONFIG;
            break;
        }

        // The bomb starts at 5 seconds, "S" is + 1 second, "B" is - 1 second, "L" exits configuration mode
        case  Task1States::CONFIG:
        {
          Serial.print("S para subir");

            uint32_t defuseTime = 5;

            while (true) {
                if (Serial.available()) {
                    const char c = Serial.read();
                    if (c == 'L') {
                      Serial.print("Ahora sigue la cuenta regresiva");
                        task1State = Task1States::WAIT_DATA;
                        break;
                    }

                    if (c == 'S') {
                        defuseTime++;
                        Serial.print("Subio");
                    }
                    if (c == 'B') {
                        defuseTime--;
                        Serial.print("Bajo");
                    }
                    Serial.println(defuseTime);
                }
            }
        }

        // The password to defuse the bomb is "C1234" and it needs to be that exact number
        case Task1States::WAIT_DATA: {
            if (Serial.available()) {
                const char c = Serial.read();
                if (c == 'C') {
                    dataCounter = 0;
                    numbersPassword = 1;

                    password[dataCounter++] = c;
                    if (dataCounter == 4 && numbersPassword = 1) {
                        password[dataCounter] = '\0'; // null terminate the password
                        if (strcmp(password, "1234") == 0) {
                            Serial.println("YOU SAVED THE WORLD");
                            task1State = Task1States::CONFIG;
                        } 
                        else {
                            Serial.print("contrasena incorrecta");
                            task1State = Task1States::CONFIG;
                        }
                        dataCounter = 0;
                    } 
                }
            break;
          }
        }
        default: {
            break;
        }
    }
}

void setup () {
  defuse();
}

void loop () 
{
  defuse();
}