#include <WiFi.h>
#include <HTTPClient.h>


const char *rede = "VIVOFIBRA-9AE8"; //rede wifi
const char *senha="KEtLXmgwwJ"; //senha da rede

TaskHandle_t Task1;
TaskHandle_t Task2; 

QueueHandle_t fila;
int tamanho_da_fila = 5;

SemaphoreHandle_t semaforo_serial;

String endpointAPI = "https://3684nhytni.execute-api.sa-east-1.amazonaws.com/beta/botao";

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
    if (Serial.available() > 0) {
      //ler string
      xSemaphoreTake(semaforo_serial, portMAX_DELAY);
      String mensagemRecebida = Serial.readStringUntil('\n');
      xSemaphoreGive(semaforo_serial);

      // coloca a informação na fila
      xQueueSendToBack(fila, &mensagemRecebida, portMAX_DELAY);

      // Aguarde um pouco antes de verificar novamente para evitar leituras repetitivas
      delay(1000);
    }
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
  }
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  fila = xQueueCreate(tamanho_da_fila, sizeof(String));
  if (fila == NULL) {
    Serial.println("Erro ao criar a fila");
  } else {
    Serial.println("Fila criada com sucesso");
    semaforo_serial = xSemaphoreCreateMutex();
    conectar_wifi();
    delay(5000);
    // Primeiro, crie a tarefa2 para garantir que ela espere pela tarefa1
    xTaskCreatePinnedToCore(tarefa2, "Task2", 10000, NULL, 1, &Task2, 1);
    // Em seguida, crie a tarefa1 para que ela comece a adicionar itens à fila
    xTaskCreatePinnedToCore(tarefa1, "Task1", 10000, NULL, 1, &Task1, 0);
  }
}

void loop() {
  // Não faz nada no loop principal, pois estamos usando FreeRTOS
}