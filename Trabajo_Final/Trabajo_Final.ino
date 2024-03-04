#include <Arduino.h>

enum class Task1States {
  INIT,
  CONFIG,
  WAIT_DATA
};

static auto task1State = Task1States::INIT;
static char password[5];
static uint8_t dataCounter = 0;
static uint32_t defuseTime = 5;
static constexpr uint32_t INTERVAL = 1000;
static constexpr uint32_t INTERVAL2 = 2000;
static uint32_t lastTime = 0;
static uint8_t result[2];

void defuse() {
  switch (task1State) {
    case Task1States::INIT: {
      Serial.begin(115200);
      delay(2000);
      Serial.print("AHORA ENTRANDO AL MODO CONFIGURACIÓN");
      delay(2000);
      task1State = Task1States::CONFIG;
      break;
    }
    case Task1States::CONFIG: {
      defuseTime = 5;
      Serial.println("'S' para aumentar 1 segundo, 'B' para bajar 1 segundo, 'L' para salir de la configuración. ");
      Serial.println("Segundos por defecto: 5");
      while (true) {
        if (Serial.available()) {
          const char c = Serial.read();
          if (c == 'L') {
            Serial.println("Ahora Inicia la cuenta regresiva ");
            delay(2000);
            task1State = Task1States::WAIT_DATA;
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
              Serial.println("Bajó 1 segundo ");;
            }             
          }
          Serial.println(defuseTime);
        }
      }
      break;
    }
    case Task1States::WAIT_DATA: {
      do {                    
        if (Serial.available()>0){
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
              task1State = Task1States::CONFIG;
              break;
            }
            else{
              Serial.println("Respuesta incorrecta");
              task1State = Task1States::WAIT_DATA;
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
        bool is = true;
        do{    
          if ((currentTime2 - lastTime) >= INTERVAL2) {
            is = false;
            delay(2000);
            task1State = Task1States::CONFIG;
            break;
          }  
        }while(is == true);          
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