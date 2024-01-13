#include <BLEDevice.h>

#define bleServerName "ESP32_Server"

#define BOTAO1 18
#define BOTAO2 19

static BLEUUID bmeServiceUUID("19B10000-E8F2-537E-4F6C-D104768A1214");
static BLEUUID CharacteristicUUID("19B10001-E8F2-537E-4F6C-D104768A1214");

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress* pServerAddress;

static BLERemoteCharacteristic* Characteristic;

bool mostrar=false;
String mensagem;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {
      advertisedDevice.getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
      Serial.println("Dispositivo Encontrado");
    }
  }
};

void funcao_ISR_btn1()
{
  mensagem="botao1";
  mostrar=true;
}

void funcao_ISR_btn2()
{
  mensagem="botao2";
  mostrar=true;
}

void scan_BLE() {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void setup() {
  Serial.begin(9600);
  delay(500);
  pinMode(BOTAO1, INPUT);
  pinMode(BOTAO2, INPUT);
  attachInterrupt(BOTAO1, funcao_ISR_btn1, RISING);
  attachInterrupt(BOTAO2, funcao_ISR_btn2, RISING);
  Serial.println("Funcionando...");
  BLEDevice::init("");
  delay(500);
  scan_BLE();
}

// Conectar ao servidor BLE com o nome, Serviço e Características especificados
bool conectarAoServidor(BLEAddress pAddress, String novoValor) {
  BLEClient* pClient = BLEDevice::createClient();

  // Conectar ao Servidor BLE remoto.
  if (!pClient->connect(pAddress)) {
    Serial.println("Falha ao conectar ao servidor");
    return false;
  }

  Serial.println("Conectado ao servidor");

  // Obter uma referência ao serviço desejado no servidor BLE remoto.
  BLERemoteService* pRemoteService = pClient->getService(bmeServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Falha ao encontrar nosso UUID de serviço: ");
    Serial.println(bmeServiceUUID.toString().c_str());
    return false;
  }

  // Obter uma referência às características no serviço do servidor BLE remoto.
  Characteristic = pRemoteService->getCharacteristic(CharacteristicUUID);

  if (Characteristic == nullptr) {
    Serial.print("Falha ao encontrar nosso UUID de característica");
    return false;
  }

  Serial.println("Encontradas nossas características");

  
  Characteristic->writeValue(novoValor.c_str(), novoValor.length());

   // Desconectar após a escrita
  pClient->disconnect();
  Serial.println("Desconectado do servidor");

  return true;
}



void loop() {
  
  if(doConnect){
    if(mostrar){
        if(conectarAoServidor(*pServerAddress,mensagem)){
          Serial.println("Enviado Com Sucesso");
       }
       mostrar=false;
    }
  }
  delay(1000);
}