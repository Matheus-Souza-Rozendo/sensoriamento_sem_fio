#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoBLE.h>

const char *rede = "SUA-REDE"; //rede wifi
const char *senha="SUA-SENHA"; //senha da rede

TaskHandle_t Task1; //tarefa 1
TaskHandle_t Task2; //tarefa 2

QueueHandle_t fila; // fila
int tamanho_da_fila = 5; //tamanho da fila

String endpointAPI = "www.exemplo.com/envio-dados"; //URL da API

BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214"); // UUID do serviço
BLEStringCharacteristic customCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLENotify, 20);// UUID da característica

void conectar_wifi(){
    WiFi.begin(rede,senha); //iniciando o wifi
    while(WiFi.status()!=WL_CONNECTED){ //aguardando a conexão
      delay(500);
    }
}

void tarefa1(void *parameter) {
  while (1) {
      BLEDevice central = BLE.central(); //inicializa uma central BLE
      if (central) { // Se a central existir
        while (central.connected()) { // se conecta a central BLE
          if (customCharacteristic.written()) { // verifica se a caracteristica foi reescrita ( RECEBEU INFO)
            String valorRecebido = customCharacteristic.value(); // pega o valor
            xQueueSendToBack(fila, &valorRecebido, portMAX_DELAY); //adiciona na fila
            delay(1000); // Aguarda antes de aceitar uma nova mensagem
          }
        }
     }
    delay(500);//aguarda 0.5s
  }
}

void tarefa2(void *parameter) {
  while (1) {
    if (uxQueueMessagesWaiting(fila) != 0){ //se tiver alguma coisa da fila
      if(WiFi.status()==WL_CONNECTED){ // se tiver conectado ao WIFI
        HTTPClient http;
        String dado;
        xQueueReceive(fila, &dado, portMAX_DELAY); //retira o elemento da fila
        String url = endpointAPI+"?botao="+dado; // monta a URL para a requisição do tipo GET
        http.begin(url); // inicia a conexão HTPP
        int httpCode = http.GET(); // realiza o metodo GET
      }else{
        conectar_wifi(); // tenta reconectar ao WIFI
      }
    }
    delay(1000); // aguarda 1 segundo
    }
}

void setup() {
  delay(1000); //aguarda configuração
  fila = xQueueCreate(tamanho_da_fila, sizeof(String)); //cria a fila
  if (fila == NULL) {} else {
    conectar_wifi(); //conecta ao wifi
    delay(5000); //aguarda conexão
    if (!BLE.begin()) { //inicializa BLE
      ESP.restart();  //ESP restart se deu errado
    }
    BLE.setLocalName("ESP32_Server"); // coloca um nome no dispositivo
    BLE.setAdvertisedService(customService); // cria um serviço no servidor com o UID definido

    customService.addCharacteristic(customCharacteristic); // adiciona uma caracteristica ao serviço
    BLE.addService(customService); // adiciona o serviço criado ao BLE

    BLE.advertise(); // torna o dispositivo visivel

    // Primeiro, crie a tarefa2 para garantir que ela espere pela tarefa1
    xTaskCreatePinnedToCore(tarefa2, "Task2", 10000, NULL, 1, &Task2, 1);
    // Em seguida, crie a tarefa1 para que ela comece a adicionar itens à fila
    xTaskCreatePinnedToCore(tarefa1, "Task1", 10000, NULL, 1, &Task1, 0);
  }
}

void loop(){}