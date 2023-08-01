#include <MFRC522.h> //biblioteca responsá  vel pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "Irmandade";
const char* password = "85979278";

const char* url = "http://192.168.0.123:9010/teste";

#define SS_PIN    4
#define RST_PIN   0

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

const int buzzer = 15;
const int luz = 5;

Bounce debouncer = Bounce();

bool ledLigado = false;
//esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
//código de status de retorno da autenticação
MFRC522::StatusCode status;

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
  // Inicia a serial
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);//Incia a conexao Wifi
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Conectado ao WiFi!");


  pinMode(buzzer, OUTPUT);
  pinMode(luz, OUTPUT);


  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Inicia MFRC522
  mfrc522.PCD_Init();
  // Mensagens iniciais no serial monitor
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  

}

void loop() 
{
  ArduinoOTA.handle();
   // Aguarda a aproximacao do cartao
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona um dos cartoes
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  leituraDados();
 
  // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
  mfrc522.PICC_HaltA(); 
  // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
  mfrc522.PCD_StopCrypto1();

 // Atualiza o estado do botão

}

//faz a leitura dos dados do cartão/tag
void leituraDados()
{
  //imprime os detalhes tecnicos do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 

  //Prepara a chave - todas as chaves estão configuradas para FFFFFFFFFFFFh (Padrão de fábrica).
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //buffer para colocar os dados ligos
  byte buffer[SIZE_BUFFER] = {0};

  //bloco que faremos a operação
  byte bloco = 1;
  byte tamanho = SIZE_BUFFER;


  //faz a autenticação do bloco que vamos operar
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //faz a leitura dos dados do bloco
  status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

 Serial.print(F("\nDados bloco ["));
 Serial.print(bloco);Serial.print(F("]: "));

  //imprime os dados lidos
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
      Serial.write(buffer[i]);
  }
  Serial.println(" ");
   
  int dadosInt = atoi( (char*)buffer);
  char dadosChar[8] = "";
  sprintf(dadosChar, "%d",dadosInt);
  // Imprime a variável de números inteiros
    if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url); // Insira a URL do servidor que vai receber a requisição e a variavel client se refere a ao objeto WifiClient Obs.: a variavel URL pode ser declarada como constante do tipo caractere dessa forma: const char* = “url”;

    http.addHeader("Content-Type", "application/json"); // Define o cabeçalho da requisição e o tipo de dado que vai enviar

    String requestBody = "{\"id\":\""+String(dadosInt)+"\"}"; //Aqui coloca-se os dados da requisição em formato json
    
    int httpCode = http.POST(requestBody);//Realiza a requisição ao servidor
    if (httpCode > 0) {
      String payload = http.getString();//Atraves do http.getString(); atribui a variavel payload a resposta da requisição
      Serial.println(httpCode);//escreve no monitor serial o codigo de resposta do servidor
      Serial.println(payload);//escreve no monitor serial o resultado da requisição


      StaticJsonDocument<256> doc;//tamanho em bytes reservado para armazenar o arquivo json

      DeserializationError error = deserializeJson(doc, payload);//realiza a conversao do json atribuindo a doc o resultado


      if(error){
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
      }//verifica se a conversao foi bem sucedida ou nao

      JsonObject root_0 = doc[0];//define um JsonObject chamado root_0 que vai receber os dados de doc para ser usado

      int id = root_0["id"];//atribui a uma variavel os valores desejados do json
      const char* nome = root_0["nome"];

      Serial.println(id);

      if(id == 0){
        buzzerBloqueia();
        Serial.println("erro");
        
      }
      else{
        buzzerLibera();
        Serial.println("OK");
        digitalWrite(luz, HIGH);
        delay(20000);
        digitalWrite(luz, LOW);
      }
    } else {
      Serial.println("Erro na requisição HTTP");//caso a requisição dê erro ele escreve isto
      buzzerBloqueia();
    }
    http.end();
  }
}
}
void buzzerLibera() {
  // Frequência do som para liberação
  int frequencia = 1000; // Altere para a frequência desejada
  int duracao = 500; // Altere para o tempo de duração desejado
  
  // Gera o som no buzzer
  tone(buzzer, frequencia, duracao);
}

// Função para gerar o som de bloqueio
void buzzerBloqueia() {
  // Frequência do som para bloqueio
  int frequencia = 500; // Altere para a frequência desejada
  int duracao = 1000; // Altere para o tempo de duração desejado
  
  // Gera o som no buzzer
  tone(buzzer, frequencia, duracao);
}




