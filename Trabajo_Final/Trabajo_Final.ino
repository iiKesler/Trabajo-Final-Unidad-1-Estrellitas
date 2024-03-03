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

static void compareData(const char *pass, uint8_t size, uint8_t *res);

void defuse() {
  switch (task1State){
    case Task1States::INIT:{
      Serial.begin(115200);
      task1State = Task1States::CONFIG;
      break;
    }
    case Task1States::CONFIG:{
      defuseTime=5;
      Serial.println("S para subir tiempo, B para bajar tiempo, L para iniciar la cuenta regresiva");
      while (Serial.available()){
        const char c = Serial.read();
        if (c == 'L'){
          Serial.println("Ahora sigue la cuenta regresiva ");
          task1State = Task1States::WAIT_DATA;
          break;
        }
        if (c == 'S' && defuseTime < 40){
          defuseTime++;
          Serial.println("Subio ");
        }
        if (c == 'B' && defuseTime > 1){
          defuseTime--;
          Serial.println("Bajo ");
        }
        Serial.println(defuseTime);
      }
      break;
    }
    case Task1States::WAIT_DATA:{
      do{
        if (Serial.available()>0){
          password[dataCounter]=Serial.read();
          dataCounter++;
          if(dataCounter==5){
            compareData(password, dataCounter,result);
            dataCounter=0;
            Serial.print("result: ");
            Serial.print(result[1]);
            if(result[1]==1){
              Serial.print("Salvaste el mundo ");
              task1State = Task1States::CONFIG;
              break;
            }
            else{
              Serial.print("Respuesta incorrecta ");
              task1State = Task1States::WAIT_DATA;
              break;
            }
          }
        }
        const uint32_t currentTime = millis();
        if ((currentTime - lastTime) >= INTERVAL) {
          Serial.println("Tiempo restante ");
          lastTime = currentTime;
          defuseTime--;
          Serial.println(defuseTime);
        }
      } while (defuseTime > 0 );

      if(defuseTime==0){
        Serial.print("RADIACIÓN NUCLEAR ACTIVA ");
        const uint32_t currentTime2 = millis();
        do{
          if ((currentTime2 - lastTime) >= INTERVAL2){
            task1State = Task1States::CONFIG;
            break;
          }
        }while(defuseTime != 0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

static void compareData(const char *pass, const uint8_t size, uint8_t *res){
  constexpr char compare[5]={'C','1','2','3','4'};
  bool same=true;

  for(int i=0;i<size;i++){
    if (pass[i]!=compare[i]){
      same=false;
      break;
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