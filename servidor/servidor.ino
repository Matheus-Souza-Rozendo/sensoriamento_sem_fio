#include <WiFi.h>

const char *rede = "VIVOFIBRA-9AE8"; //rede wifi
const char *senha="KEtLXmgwwJ"; //senha da rede

void conectar_wifi(){
    Serial.print("Conectando-se a: ");
    Serial.println(rede);
    WiFi.begin(rede,senha); //iniciando o wifi
    while(WiFi.status()!=WL_CONNECTED){ //aguardando a conexão
      delay(500);
      Serial.print(".");
    }
}

void setup() {
  Serial.begin(9600); //inicializando a comunicação serial
  delay(5000); //aguardando o fim da configuração
  conectar_wifi(); //chamando o procedimento para conectar ao wifi
  delay(5000); // aguardando o fim da configuração
}

void loop() {
  if(WiFi.status()==WL_CONNECTED){ // se estiver conectado
    Serial.println("Conectado a internet"); //envia a mensagem de conectado
  }else{
    Serial.println("Desconectado"); // envia a mensagem de desconexão
    conectar_wifi();// tenta a conexão novamente
  }
  delay(3000);  //aguarda 3 segundos para iniciar novamente o loop
}
