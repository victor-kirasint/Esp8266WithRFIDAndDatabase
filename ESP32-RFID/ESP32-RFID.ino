#include <MFRC522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#ifndef STASSID
#define STASSID "portateste"
#define STAPSK "12345678"
#endif


const char* ssid = STASSID;
const char* password = STAPSK;


#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

#define pinVerde     12
#define pinVermelho  32

//esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
//código de status de retorno da autenticação
MFRC522::StatusCode status;

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
    Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

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




  // Inicia a serial
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus

  pinMode(pinVerde, OUTPUT);
  pinMode(pinVermelho, OUTPUT);
  
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

    // Dump debug info about the card; PICC_HaltA() is automatically called
//  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  //chama o menu e recupera a opção desejada
  int opcao = menu();
   // verifica se ainda está com o cartão/tag
//  if ( ! mfrc522.PICC_IsNewCardPresent()) 
//  {
//    return;
//  }
  
  if(opcao == 0) 
    leituraDados();
  else if(opcao == 1) 
    gravarDados();
  else {
    Serial.println(F("Opção Incorreta!"));
    return;
  }
 
  // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
  mfrc522.PICC_HaltA(); 
  // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
  mfrc522.PCD_StopCrypto1();  
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
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }

  //faz a leitura dos dados do bloco
  status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }
  else{
      digitalWrite(pinVerde, HIGH);
      delay(1000);
      digitalWrite(pinVerde, LOW);
  }

  Serial.print(F("\nDados bloco ["));
  Serial.print(bloco);Serial.print(F("]: "));

  //imprime os dados lidos
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
      Serial.write(buffer[i]);
  }
  Serial.println(" ");
}

//faz a gravação dos dados no cartão/tag
void gravarDados()
{
  //imprime os detalhes tecnicos do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  // aguarda 30 segundos para entrada de dados via Serial
  Serial.setTimeout(30000L) ;     
  Serial.println(F("Insira os dados a serem gravados com o caractere '#' ao final\n[máximo de 16 caracteres]:"));

  //Prepara a chave - todas as chaves estão configuradas para FFFFFFFFFFFFh (Padrão de fábrica).
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //buffer para armazenamento dos dados que iremos gravar
  byte buffer[MAX_SIZE_BLOCK] = "";
  byte bloco; //bloco que desejamos realizar a operação
  byte tamanhoDados; //tamanho dos dados que vamos operar (em bytes)

  //recupera no buffer os dados que o usuário inserir pela serial
  //serão todos os dados anteriores ao caractere '#'
  tamanhoDados = Serial.readBytesUntil('#', (char*)buffer, MAX_SIZE_BLOCK);
  //espaços que sobrarem do buffer são preenchidos com espaço em branco
  for(byte i=tamanhoDados; i < MAX_SIZE_BLOCK; i++)
  {
    buffer[i] = ' ';
  }
 
  bloco = 1; //bloco definido para operação
  String str = (char*)buffer; //transforma os dados em string para imprimir
  Serial.println(str);

  //Authenticate é um comando para autenticação para habilitar uma comuinicação segura
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    bloco, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }
  //else Serial.println(F("PCD_Authenticate() success: "));
 
  //Grava no bloco
  status = mfrc522.MIFARE_Write(bloco, buffer, MAX_SIZE_BLOCK);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(pinVermelho, HIGH);
    delay(1000);
    digitalWrite(pinVermelho, LOW);
    return;
  }
  else{
    Serial.println(F("MIFARE_Write() success: "));
    digitalWrite(pinVerde, HIGH);
    delay(1000);
    digitalWrite(pinVerde, LOW);
  }
 
}

//menu para escolha da operação
int menu()
{
  Serial.println(F("\nEscolha uma opção:"));
  Serial.println(F("0 - Leitura de Dados"));
  Serial.println(F("1 - Gravação de Dados\n"));

  //fica aguardando enquanto o usuário nao enviar algum dado
  while(!Serial.available()){};

  //recupera a opção escolhida
  int op = (int)Serial.read();
  //remove os proximos dados (como o 'enter ou \n' por exemplo) que vão por acidente
  while(Serial.available()) {
    if(Serial.read() == '\n') break; 
    Serial.read();
  }
  return (op-48);//do valor lido, subtraimos o 48 que é o ZERO da tabela ascii
}

