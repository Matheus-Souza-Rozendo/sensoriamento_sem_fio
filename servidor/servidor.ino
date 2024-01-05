#include <WiFi.h>

const char *rede = "VIVOFIBRA-9AE8"; //rede wifi
const char *senha="KEtLXmgwwJ"; //senha da rede

TaskHandle_t Task1;
TaskHandle_t Task2; 

QueueHandle_t fila;
int tamanho_da_fila = 50;

SemaphoreHandle_t semaforo_serial;
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
    for (int i = 0; i < tamanho_da_fila; i++) {
      int item = i;
      xQueueSendToFront(fila, &item, portMAX_DELAY);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
  }
}

void tarefa2(void *parameter) {
  while (1) {
    int elemento;
    for (int i = 0; i < tamanho_da_fila; i++) {
      xQueueReceive(fila, &elemento, portMAX_DELAY);

      xSemaphoreTake(semaforo_serial, portMAX_DELAY);
      Serial.print(elemento);
      Serial.print(',');
      xSemaphoreGive(semaforo_serial);

      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelete(NULL);
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  fila = xQueueCreate(tamanho_da_fila, sizeof(int));
  if (fila == NULL) {
    Serial.println("Erro ao criar a fila");
  } else {
    Serial.println("Fila criada com sucesso");

    semaforo_serial = xSemaphoreCreateMutex();

    // Primeiro, crie a tarefa2 para garantir que ela espere pela tarefa1
    xTaskCreatePinnedToCore(tarefa2, "Task2", 10000, NULL, 1, &Task2, 1);

    // Em seguida, crie a tarefa1 para que ela comece a adicionar itens à fila
    xTaskCreatePinnedToCore(tarefa1, "Task1", 10000, NULL, 1, &Task1, 0);
  }
}

void loop() {
  // Não faz nada no loop principal, pois estamos usando FreeRTOS
}