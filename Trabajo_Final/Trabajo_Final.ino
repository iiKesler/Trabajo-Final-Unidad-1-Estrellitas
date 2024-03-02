#include <Arduino.h>

enum class Task1States {
    INIT,
    CONFIG,
    WAIT_DATA
};

static auto task1State = Task1States::INIT;
static char password[5];
static uint8_t dataCounter = 0;
static char result[2];

void defuse() {
    switch (task1State) {
        case Task1States::INIT: {
            Serial.begin (115200);
            task1State = Task1States::CONFIG;
            break;
        }

        // The bomb starts at 5 seconds, "S" is + 1 second, "B" is - 1 second, "L" exits configuration mode
        case  Task1States::CONFIG:{
            uint32_t defuseTime = 5;
            while (true) {
                if (Serial.available()) {
                    const char c = Serial.read();
                    if (c == 'L') {
                        task1State = Task1States::WAIT_DATA;
                        break;
                    }
                    if (c == 'S') {
                        defuseTime++;
                    }
                    if (c == 'B') {
                        defuseTime--;
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
                    task1State = Task1States::WAIT_DATA;
                    dataCounter = 0;
                } else if (task1State == Task1States::WAIT_DATA) {
                    password[dataCounter++] = c;
                    if (dataCounter == 4) {
                        password[dataCounter] = '\0'; // null terminate the password
                        if (strcmp(password, "1234") == 0) {
                            Serial.println("YOU SAVED THE WORLD");
                        } else {
                            task1State = Task1States::CONFIG;
                        }
                        dataCounter = 0;
                    }
                } else {
                    task1State = Task1States::CONFIG;
                }
            }
            break;
        }

        default: {
            break;
        }
    }
}

void setup () {
// write your initialization code here
}

void loop () {
// write your code here
}