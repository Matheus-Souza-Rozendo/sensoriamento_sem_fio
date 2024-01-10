#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoBLE.h>

const char *rede = "VIVOFIBRA-9AE8"; //rede wifi
const char *senha="KEtLXmgwwJ"; //senha da rede

TaskHandle_t Task1;
TaskHandle_t Task2; 

QueueHandle_t fila;
int tamanho_da_fila = 5;

SemaphoreHandle_t semaforo_serial;

String endpointAPI = "https://3684nhytni.execute-api.sa-east-1.amazonaws.com/beta/botao";

BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214"); // UUID do serviço
BLEStringCharacteristic customCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLENotify, 20);// UUID da característica

void conectar_wifi(){
    Serial.print("Conectando-se a: ");
    Serial.println(rede);
    WiFi.begin(rede,senha); //iniciando o wifi
    while(WiFi.status()!=WL_CONNECTED){ //aguardando a conexão
      delay(500);
      Serial.print(".");
    }
}

void tarefa1(void *parameter) {
  while (1) {
      BLEDevice central = BLE.central();
      if (central) {
        xSemaphoreTake(semaforo_serial, portMAX_DELAY);
        Serial.print("Conectado a ");
        Serial.println(central.address());
        xSemaphoreGive(semaforo_serial);
        while (central.connected()) {
          if (customCharacteristic.written()) {
            String valorRecebido = customCharacteristic.value();
            xQueueSendToBack(fila, &valorRecebido, portMAX_DELAY);
            delay(1000); // Aguarda antes de aceitar uma nova mensagem
          }
        }
        xSemaphoreTake(semaforo_serial, portMAX_DELAY);
        Serial.println("Conexão perdida.");
        xSemaphoreGive(semaforo_serial);
     }
    delay(500);
  }
}

void tarefa2(void *parameter) {
  while (1) {
    if (uxQueueMessagesWaiting(fila) != 0){
      if(WiFi.status()==WL_CONNECTED){
        HTTPClient http;
        String dado;
        xQueueReceive(fila, &dado, portMAX_DELAY);
        String url = endpointAPI+"?botao="+dado;
        http.begin(url);
        int httpCode = http.GET();
        if(httpCode<=0){
          xSemaphoreTake(semaforo_serial, portMAX_DELAY);
          Serial.println("Erro na requisição");
          xSemaphoreGive(semaforo_serial);
        }
      }else{
        xSemaphoreTake(semaforo_serial, portMAX_DELAY);
        conectar_wifi();
        xSemaphoreGive(semaforo_serial);
      }
    }
    delay(1000);
    }
}

void setup() {
  Serial.begin(9600); // iniciando a comunicação serial
  delay(1000); //aguarda configuração
  fila = xQueueCreate(tamanho_da_fila, sizeof(String)); //cria a fila
  if (fila == NULL) { //houve erro ao criar a fila
    Serial.println("Erro ao criar a fila"); 
  } else {
    Serial.println("Fila criada com sucesso");
    semaforo_serial = xSemaphoreCreateMutex(); // criar  um semaforo para a porta serial
    conectar_wifi(); //conecta ao wifi
    delay(5000); //aguarda conexão
    if (!BLE.begin()) { //inicializa BLE
      Serial.println("Erro ao iniciar o BLE!");
      while (1); //fica preso aqui caso de erro (MODIFICAR DEPOIS)
    }
    BLE.setLocalName("ESP32_Server"); // coloca um nome no dispositivo
    BLE.setAdvertisedService(customService); // cria um serviço no servidor com o UID definido

    customService.addCharacteristic(customCharacteristic); // adiciona uma caracteristica ao serviço
    BLE.addService(customService); // adiciona o serviço criado ao BLE

    BLE.advertise(); // torna o dispositivo visivel

    Serial.println("Servidor BLE iniciado. Aguardando conexão..."); // aguardando pareamento
    // Primeiro, crie a tarefa2 para garantir que ela espere pela tarefa1
    xTaskCreatePinnedToCore(tarefa2, "Task2", 10000, NULL, 1, &Task2, 1);
    // Em seguida, crie a tarefa1 para que ela comece a adicionar itens à fila
    xTaskCreatePinnedToCore(tarefa1, "Task1", 10000, NULL, 1, &Task1, 0);
  }
}

void loop(){}