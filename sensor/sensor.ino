#include <BLEDevice.h>

#define bleServerName "ESP32_Server" //nome do servidor BLE que iremos conectar

//GPIO dos Botões
#define BOTAO1 18 
#define BOTAO2 19

static BLEUUID bmeServiceUUID("19B10000-E8F2-537E-4F6C-D104768A1214"); //UID do serviço BLE que iremos acessar
static BLEUUID CharacteristicUUID("19B10001-E8F2-537E-4F6C-D104768A1214"); // UID da caracteristica BLE que iremos alterar

static boolean doConnect = false; // estado que indica que o servidor foi achado
static boolean connected = false;

static BLEAddress* pServerAddress;

static BLERemoteCharacteristic* Characteristic;

bool enviar=false; //variavel que controla o envio
String mensagem; // variavel da mensagem que sera enviada

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks { //classe personalizada de callback de retorno quando achar algum dispositivo BLE
  void onResult(BLEAdvertisedDevice advertisedDevice) { // metodo OnResult que sobreescreve o original
    if (advertisedDevice.getName() == bleServerName) { //se o dispositivo tiver o mesmo nome do servidor procurado
      advertisedDevice.getScan()->stop(); //para o scan
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // pega o endereço do dispositivo
      doConnect = true; // pronto para se conectar
    }
  }
};

void funcao_ISR_btn1() //função ISR para quando o botão 1 for apertado
{
  mensagem="botao1";
  mostrar=true;
}

void funcao_ISR_btn2() //função ISR para quando o botão 2 for apertado
{
  mensagem="botao2";
  mostrar=true;
}

void scan_BLE() { // função para scanear dispositivos BLE
  BLEScan* pBLEScan = BLEDevice::getScan(); //seta o scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //configura a classe de callback personalizada
  pBLEScan->setActiveScan(true); //ativa o scan
  pBLEScan->start(30); // inicia o scan por 30 segundos
}

void setup() {
  //Pinos dos botões como entrada
  pinMode(BOTAO1, INPUT);
  pinMode(BOTAO2, INPUT);
  //ativa a interrupção  dos botões
  attachInterrupt(BOTAO1, funcao_ISR_btn1, RISING);
  attachInterrupt(BOTAO2, funcao_ISR_btn2, RISING);
  BLEDevice::init(""); //inicializa o dispositivo BLE
  delay(500); //aguarda configuração
  scan_BLE(); // faz o scan BLE
}

// Conectar ao servidor BLE com o nome, Serviço e Características especificados
bool conectarAoServidor(BLEAddress pAddress, String novoValor) {
  BLEClient* pClient = BLEDevice::createClient();

  // Conectar ao Servidor BLE remoto.
  if (!pClient->connect(pAddress)) {
    return false;
  }

  // Obter uma referência ao serviço desejado no servidor BLE remoto.
  BLERemoteService* pRemoteService = pClient->getService(bmeServiceUUID);
  if (pRemoteService == nullptr) {
    return false;
  }

  // Obter uma referência às características no serviço do servidor BLE remoto.
  Characteristic = pRemoteService->getCharacteristic(CharacteristicUUID);

  if (Characteristic == nullptr) {
    return false;
  }

  Characteristic->writeValue(novoValor.c_str(), novoValor.length()); //envia a informação para o servidor BLE

   // Desconectar após a escrita
  pClient->disconnect();

  return true;
}



void loop() {
  
  if(doConnect){ // se estiver pronto para conectar
    if(mostrar){ // se o botão for apertado
        if(conectarAoServidor(*pServerAddress,mensagem)){}
        mostrar=false;
    }
  }
  delay(1000); //aguarda 1 segun
}