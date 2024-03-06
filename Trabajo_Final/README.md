# Documentation of the code
The code is used to create an interactive experience with the user, in which they have to defuse a nuclear bomb set up by a group of terrorists.

## State Machines
First off, the code has 3 different state machines, "INIT, CONFIG, WAIT_DATA", let's go one by one

    enum class DefuseStates {
      INIT,
      CONFIG,
      WAIT_DATA
    };
    
    static auto defuseState = DefuseStates::INIT;

### INIT
This is the beginnig of the code, where we start the serial with the microcontroler.

    case DefuseStates::INIT: {
          Serial.begin(115200);
          delay(2000);
          Serial.print("AHORA ENTRANDO AL MODO CONFIGURACIÓN");
          delay(2000);
          defuseState = DefuseStates::CONFIG;
          break;

The way we communicate between states, is by using a `switch` statement, which evaluates the variable called `defuseState` and it changes depending on which parrt of the code we're in, this `INIT`state is never used again.

### CONFIG
This is where the interaction actually begins, because the user now chooses how many seconds they want to have to defuse the bomb (between 1 and 4o seconds), by pressing certain keys and, when they're done, they press another key and it changes states, where they have to introduce the passowrd to defuse the bomb.

    case DefuseStates::CONFIG: {
          defuseTime = 5;
          Serial.println("'S' para aumentar 1 segundo, 'B' para bajar 1 segundo, 'L' para salir de la configuración. ");
          Serial.println("Segundos por defecto: 5");
          while (true) {
            if (Serial.available()) {
              const char c = Serial.read();
              if (c == 'L') {
                Serial.println("Ahora Inicia la cuenta regresiva ");
                delay(2000);
                defuseState = DefuseStates::WAIT_DATA;
                break;
              }
              if (c == 'S') {
                if(defuseTime == 40){
                  Serial.println("No se pueden agregar más de 40 segundos ");
                }
                else{
                  defuseTime++;
                  Serial.println("Subió 1 segundo ");
                }
              }
              if (c == 'B') {
                if(defuseTime == 1){
                  Serial.println("No se puede bajar a menos de 1 segundo ");
                }
                else{
                  defuseTime--;
                  Serial.println("Bajó 1 segundo ");
                }             
              }
              Serial.println(defuseTime);
            }
          }
          break;
        }
This part consists of different sets of `if` statements, where it evaluates which keys have been pressed (In this case, it can either be 'S', 'B' or 'L') and it does something depending on that, some other ifs evaluate if the maximum or minimum ammount of seconds has been reached (The default of seconds being 5) and it prints a message if that's the case, and, at the end, it prints how many seconds in total you have.

### WAIT_DATA
This is the part of the code where you introduce the password to defuse the bomb, in this case, we're looking for "C1234", a string of 5 characters.

      case DefuseStates::WAIT_DATA: {
            do {                    
              if (Serial.available() > 0){
                password[dataCounter] = Serial.read();
                dataCounter++;
                if(dataCounter == 5){
                  compareData(password, dataCounter,result);
                  dataCounter=0;
                  Serial.println("result: ");
                  Serial.println(result[1]);
                  if(result[1] == 1){
                    Serial.println("Salvaste el mundo");
                    delay(2000);
                    defuseState = DefuseStates::CONFIG;
                    break;
                  }
                  else{
                    Serial.println("Respuesta incorrecta");
                    defuseState = DefuseStates::WAIT_DATA;
                    break;
                  }          
                }
              }
            uint32_t currentTime = millis();
            if ((currentTime - lastTime) >= INTERVAL) {
              Serial.println("Tiempo restante");
              lastTime = currentTime;
              defuseTime--;                            
              Serial.println(defuseTime);
            }
          }while (defuseTime > 0 );
            if(defuseTime == 0){
              Serial.println("RADIACIÓN NUCLEAR ACTIVA");            
              uint32_t currentTime2 = millis();
              delay(2000);
              defuseState = DefuseStates::CONFIG;          
            }
            break;
          }
This is an interesting part of the code, is where the user will spend a lot of time trying to figure out what data to enter. We do this by usint a `do... while` statement, where the condition to exit this loop is the `defuseTime` variable being greater than 0. Once this condition is met, it prints a message saying you failed to defuse the bomb and after 2 seconds, it goes back to the `CONFIG` state. On the flip side, if you introduce the password correctly, it prints a message saying you succeded on stopping the bomb, and again, after 2 seconds, it goes back to the `CONFIG` state.

## Variables
This code counts with multiple variables:

    static auto defuseState = DefuseStates::INIT;
    static uint32_t defuseTime = 5;
    static char password[5];
    static uint8_t dataCounter = 0;
    static uint8_t result[2];
    static constexpr uint32_t INTERVAL = 1000;
    static uint32_t lastTime = 0;
Let's go part by part, the `defuseState` variable is used to change between the states of the machine, it is used in the `switch` statement at the beginning of the `void defuse()` function. The next variable `defuseTime` is used to have a counter on how many seconds the user has left, by default, this value is 5 and it can be changed on the `CONFIG` state. The next 3 variable are used to check the password introduced by the user, this are `password[5]`, `dataCounter` and `result[2]`, the `password[5]` is used to store whatever the user sends to the serial; `dataCounter` is used to check how many data is gettin in, and if you check the `WAIT_DATA` state, where all this variables are being used, it compares if this variable has been equal to 5 and then proceeds to check for the characters that were entered themselves are equal to the ones we need to defuse the bomb; `result[2]` is used to store a boolean result of 0 or 1, and it's just the result of the evaluation done before in the `compareData` function, if the result is 0, then it is NOT the right password, otherwise, it is.

## `compareData` Function
This is what is used to evaluate if the password sent by the user is the same one the code needs to stop the bomb, i.e, if the 2 values are the same, the bomb gets defused.

    static void compareData(const char *pass, const uint8_t size, uint8_t *res){
      constexpr char compare[5] = {'C','1','2','3','4'};
      bool same = true;
    
      for(int i = 0; i < size; i++){
        if (pass[i] == compare[i]){
          same = true;
        }
        else{
          same = false;
          i = 5;      
        }  
      }
    
      res[0] = same ? 0 : 1;
      res[1] = same ? 1 : 0;
    }
It is a simple `for` cycle, where it checks position by position of what's stored in the `compare[5]` variable, if all the characters are the same, it returns a boolean value of `true` (or 1) and it returns it via a pointer of the `res[]` variable, this one points towards the `result[2]` variable and if it's 0, then is not the correct password, but if it's 1, it IS the correct password.

## Void setup() and Void loop()
This is used to call for the first time the `Void defuse()` function and then loop it indefinitely.

## Complete code

    #include <Arduino.h>
    
    enum class DefuseStates {
      INIT,
      CONFIG,
      WAIT_DATA
    };
    
    static auto defuseState = DefuseStates::INIT;
    static uint32_t defuseTime = 5;
    static char password[5];
    static uint8_t dataCounter = 0;
    static uint8_t result[2];
    static constexpr uint32_t INTERVAL = 1000;
    static uint32_t lastTime = 0;
    
    void defuse() {
      switch (defuseState) {
        case DefuseStates::INIT: {
          Serial.begin(115200);
          delay(2000);
          Serial.print("AHORA ENTRANDO AL MODO CONFIGURACIÓN");
          delay(2000);
          defuseState = DefuseStates::CONFIG;
          break;
        }
        case DefuseStates::CONFIG: {
          defuseTime = 5;
          Serial.println("'S' para aumentar 1 segundo, 'B' para bajar 1 segundo, 'L' para salir de la configuración. ");
          Serial.println("Segundos por defecto: 5");
          while (true) {
            if (Serial.available()) {
              const char c = Serial.read();
              if (c == 'L') {
                Serial.println("Ahora Inicia la cuenta regresiva ");
                delay(2000);
                defuseState = DefuseStates::WAIT_DATA;
                break;
              }
              if (c == 'S') {
                if(defuseTime == 40){
                  Serial.println("No se pueden agregar más de 40 segundos ");
                }
                else{
                  defuseTime++;
                  Serial.println("Subió 1 segundo ");
                }
              }
              if (c == 'B') {
                if(defuseTime == 1){
                  Serial.println("No se puede bajar a menos de 1 segundo ");
                }
                else{
                  defuseTime--;
                  Serial.println("Bajó 1 segundo ");
                }             
              }
              Serial.println(defuseTime);
            }
          }
          break;
        }
        case DefuseStates::WAIT_DATA: {
          do {                    
            if (Serial.available() > 0){
              password[dataCounter] = Serial.read();
              dataCounter++;
              if(dataCounter == 5){
                compareData(password, dataCounter, result);
                dataCounter=0;
                Serial.println("result: ");
                Serial.println(result[1]);
                if(result[1] == 1){
                  Serial.println("Salvaste el mundo");
                  delay(2000);
                  defuseState = DefuseStates::CONFIG;
                  break;
                }
                else{
                  Serial.println("Respuesta incorrecta");
                  defuseState = DefuseStates::WAIT_DATA;
                  break;
                }          
              }
            }
          uint32_t currentTime = millis();
          if ((currentTime - lastTime) >= INTERVAL) {
            Serial.println("Tiempo restante");
            lastTime = currentTime;
            defuseTime--;                            
            Serial.println(defuseTime);
          }
        }while (defuseTime > 0 );
          if(defuseTime == 0){
            Serial.println("RADIACIÓN NUCLEAR ACTIVA");            
            uint32_t currentTime2 = millis();
            delay(2000);
            defuseState = DefuseStates::CONFIG;         
          }
          break;
        }
    
        default: {
          break;
        }
      }
    }
    static void compareData(const char *pass, const uint8_t size, uint8_t *res){
      constexpr char compare[5] = {'C','1','2','3','4'};
      bool same = true;
    
      for(int i = 0; i < size; i++){
        if (pass[i] == compare[i]){
          same = true;
        }
        else{
          same = false;
          i = 5;      
        }  
      }
    
      res[0] = same ? 0 : 1;
      res[1] = same ? 1 : 0;
    }
    
    
    void setup() {
      defuse();
    }
    
    void loop() {
      defuse();
    }

## State Diagram
![image](https://github.com/iiKesler/Trabajo-Final-Unidad-1-Estrellitas/assets/89699466/84dacac3-e168-490f-ad09-bda8f91f56d9)
