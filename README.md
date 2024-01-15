# Projeto de Comunicação ESP-32

## Descrição

Este projeto implementa um sistema de comunicação entre dois dispositivos ESP-32: um servidor e um sensor. O servidor recebe informações via BLE do sensor, armazena-as em uma fila e, posteriormente, envia para uma API na AWS via Wi-Fi. O sensor possui uma interrupção de botão que inicia a comunicação BLE com o servidor, enviando o nome do botão pressionado.

## Requisitos de Hardware

- 2 ESP32 DEVKITV1
- 2 Push Buttons
- 2 Resistores 10K ohm

## Bibliotecas Utilizadas

### No Código do Servidor:

- WiFi.h
- HTTPClient.h
- ArduinoBLE.h

### No Código do Sensor:

- BLEDevice.h

## Funcionalidades do Servidor

### Tarefas do Servidor

1. **Tarefa de Leitura BLE:**
   - Lê informações via BLE do sensor.
   - Armazena informações em uma fila para processamento posterior.

2. **Tarefa de Envio via Wi-Fi:**
   - Envia, via Wi-Fi, informações retiradas da fila para uma API na AWS.

### Comunicação entre Tarefas

- As tarefas se comunicam por meio de uma fila.

### Uso de Coprocessadores

- Cada tarefa utiliza um coprocessador diferente para execução simultânea.

## Funcionalidades do Sensor

### Funcionamento do Botão

- Interrupção de botão habilitada.
- A cada pressionamento, a interrupção é desabilitada, e inicia a comunicação BLE com o servidor, enviando o nome do botão pressionado.

## Instalação e Uso

1. Clone este repositório: `git clone https://github.com/Matheus-Souza-Rozendo/sensoriamento_sem_fio`
2. Instale o pacote de desenvolvimento do ESP32 para a Arduino IDE. Veja [Tutorial](https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html)

## Licença

Este projeto é licenciado sob a [Licença MIT](LICENSE).

