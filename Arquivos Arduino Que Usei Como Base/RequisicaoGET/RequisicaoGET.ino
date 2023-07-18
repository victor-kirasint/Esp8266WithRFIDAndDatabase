#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Irmandade";
const char* password = "85979278";

const char* url = "http://192.168.0.123:9010/teste";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//Inicia o monitor serial na velocidade 115200, mas vai variar dependendo do seu esp, sendo 9600 a velocidade mais comum

  WiFi.begin(ssid, password);//Incia a conexao Wifi
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado ao WiFi!");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url); //IInsira a URL do servidor que vai receber a requisição e a variavel client se refere a ao objeto WifiClient Obs.: a variavel URL pode ser declarada como constante do tipo caractere dessa forma: const char* = “url”;


    int httpCode = http.GET();// Faz a requisição GET ao servidor
    if (httpCode > 0) {
      String payload = http.getString();// atribui a variavel payload o resultado da requisiçào
      Serial.println(httpCode);//escreve no monitor serial o codigo de resposta do servidor
      Serial.println(payload);// escreve no monitor serial a resposta da requisição
    } else {
      Serial.println("Erro na requisição HTTP");
    }
    

    http.end();

}
