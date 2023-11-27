#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad.h>

// Constantes RFID
#define RST_PIN 15
#define SS_PIN 14
#define MISO_PIN 12
#define MOSI_PIN 13
#define SCK_PIN 2

const char *ssid = "Teclenet_Valdenir";  // Nome da rede Wi-Fi
const char *password = "ir101823";       // Senha da rede Wi-Fi
//const char *ssid = "NEM CLICA";        // Nome da rede Wi-Fi
//const char *password = "a1b2c3d4e5";   // Senha da rede Wi-Fi
//const char *ssid = "IGÃO";             // Nome da rede Wi-Fi
//const char *password = "igor1234";     // Senha da rede Wi-Fi

// Define os pinos para cada componente de cor do LED RGB
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 0

// Define os pinos para os botões
#define BTN_BIOMETRY_PIN 13
#define BTN_OFF_PIN 15

// Variáveis RFID
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

//Objeto responsável por controlar o sensor de digitais
//Utilizamos a Serial2 para comunicação e a senha padrão
//Você também pode utilizar software serial se seu dispositivo não tiver  mais HardwareSerial disponível
//Se não passar o parâmetro de senha ele também considera como senha padrão (0x0)
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial1);

// Variáveis teclado
const uint8_t ROWS = 2;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '*', '0', '#', 'D' },
};

uint8_t colPins[COLS] = { 13, 12, 14, 2 };  // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 15, 26 };         // Pins connected to R1, R2, R3, R4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Cria o objeto dos servos
Servo ServoLock;
//Servo ServoOpenSafe;

// Pino GPIO conectado ao servo
#define SERVO_PIN 16
// Pino GPIO conectado ao servo responsável por abrir a trava final
//#define SERVO_OPEN_SAFE_PIN 2

// Objeto para o servidor web assíncrono
AsyncWebServer server(80);

// Handler de tarefa do RFID
TaskHandle_t rfidTaskHandle = NULL;
// Handler de tarefa do teclado
TaskHandle_t keypadTaskHandle = NULL;
// Handler de tarefa da biometria
TaskHandle_t biometryTaskHandle = NULL;
// Handler para verificar o status
TaskHandle_t verifyStatusButtonTaskHandle = NULL;
// Handler de tarefa do Servo
TaskHandle_t servoOpenLockTaskHandle = NULL;
// Handler de tarefa do Servo que fehca o cofre
TaskHandle_t servoCloseLockTaskHandle = NULL;
// Handler de tarefa do Servo
//TaskHandle_t servoOpenSafeTaskHandle = NULL;
// Handler de tarefa do Servo que fecha o cofre
//TaskHandle_t servoCloseSafeTaskHandle = NULL;

// Função de tarefa para leitura RFID
void readRFID(void *pvParameters);
// Função de tarefa para leitura do teclado
void readKeypad(void *pvParameters);
// Função de tarefa para leitura da biometria
void readBiometry(void *pvParameters);
// Função para verificar o status
void verifyStatusButton(void *pvParameters);
// Função de tarefa para abrir Servo
void servoOpenLockTask(void *pvParameters);
// Função de tarefa para fechar Servo
void servoCloseLockTask(void *pvParameters);
// Função de tarefa do Servo que abre o cofre
//void servoOpenSafeTask(void *pvParameters);
// Função de tarefa do Servo que fecha o cofre
//void servoCloseSafeTask(void *pvParameters);

// Declara os fields do ThingSpeak
struct ThingSpeakData {
  String field1;
  String field2;
  String field3;
  String field4;
  String field5;
  String field6;
};

void setup() {

  // Inicialização da comunicação serial
  Serial.begin(115200);
  Serial.println("Inicializando o Sistema");

  // Configura os pinos como saídas
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Conexão à rede Wi-Fi
  delay(2000);
  Serial.print("\nConectando à rede ");
  Serial.println(ssid);

  // Fica no loop até conectar
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi Conectado. Endereço IP: ");
  String adressIP = WiFi.localIP().toString();
  Serial.println(adressIP);

  // Obtém o endereço MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);

  // Converte o endereço MAC para uma string
  char macAddress[18];
  sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Exibe o endereço MAC
  Serial.println("Endereço MAC: " + String(macAddress));

  // Guarda o processo do ESP
  String process = "";

  // Envia o endereço MAC para o backend e guarda o processo que será feito no ESP, deve ficar no Loop até encontrar uma opção válida
  // R -> RFID = 30:AE:A4:FD:E9:E4
  // K -> Teclado Numérico = 30:AE:A4:FF:26:F8
  // B -> Biometria = 30:AE:A4:FF:3A:60
  // Loop principal
  do {
    // Verifica o processo de acordo com o MAC
    process = getProcessFromThingSpeak(macAddress);

    // Adiciona um delay apenas se não houver um processo válido
    if (process != "R" && process != "K" && process != "B") {
      delay(5000);
    } else {
      Serial.print("Processo: ");
      Serial.println(process);
    }
  } while (process != "R" && process != "K" && process != "B");

  // Exibe o processo
  Serial.println(process == "R" ? "Esp Executando RFID" : process == "K" ? "Esp Executando Teclado Numérico"
                                                                         : "Esp Executando Biometria");

  //Grava o endereço de IP de todos os ESP
  sendToThingSpeak("LXO0SK0MUOZY2ZI7", adressIP, process, "", "", "", "");

  //Busca as informações no ThingSpeak
  ThingSpeakData result = readLastEntryFromThingSpeak();
  // Se reiniciou o sistema no meio do desbloqueio reseta contador
  if (result.field5 == "N") {
    sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, result.field4, result.field5, "0");
    delay(5000);
  }

  // Controla os leds que devem ligar
  controlLedRGB(true, false, false);

  //Se o processo for RFID
  if (process == "R") {

    Serial.println("RFID Configurado!");

    // Inicialização da SPI
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

    // Inicialização do RFID
    rfid.PCD_Init();

    Serial.print("Leitor: ");
    rfid.PCD_DumpVersionToSerial();

    // Criar tarefa de leitura RFID
    xTaskCreatePinnedToCore(readRFID, "RFIDTask", 10000, NULL, 1, &rfidTaskHandle, 0);

    //Rota para desligar travas do RFID
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println("Bloqueando acesso ao cofre ESP RFID...");
      // Controla os leds que devem ligar
      controlLedRGB(true, false, false);
      // Inicia a tarefa de desligar o Servo da trava
      vTaskResume(servoCloseLockTaskHandle);
      request->send(200, "text/plain", "Bloqueando acesso ao cofre ESP RFID...");
    });

    //Se o processo for teclado numérico
  } else if (process == "K") {

    Serial.println("Teclado Numérico Configurado");

    // Criar tarefa de leitura do teclado
    xTaskCreatePinnedToCore(readKeypad, "KeypadTask", 10000, NULL, 1, &keypadTaskHandle, 0);

    //Rota para desligar travas do teclado
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println("Bloqueando acesso ao cofre ESP Biometria...");
      // Controla os leds que devem ligar
      controlLedRGB(true, false, false);
      // Inicia a tarefa de desligar o Servo da trava
      vTaskResume(servoCloseLockTaskHandle);
      request->send(200, "text/plain", "Bloqueando acesso ao cofre ESP Biometria...");
    });

    //Se o processo for Biometria
  } else if (process == "B") {

    Serial.println("Biometria Configurada");

    pinMode(BTN_OFF_PIN, INPUT_PULLUP);
    pinMode(BTN_BIOMETRY_PIN, INPUT_PULLUP);

    // Inicializa o sensor biométrico com a porta serial (RX_PIN e TX_PIN)
    fingerprintSensor.begin(57600);

    if (fingerprintSensor.verifyPassword()) {
      Serial.println("Senha do sensor biométrico confirmada!");
    } else {
      Serial.println("Senha do sensor biométrico incorreta. Verifique a configuração.");
    }

    // Criar tarefa para verificar o status
    xTaskCreate(verifyStatusButton, "verifyStatusButtonTask", 10000, NULL, 1, &verifyStatusButtonTaskHandle);
    // Suspende a tarefa de verificar o status
    vTaskSuspend(verifyStatusButtonTaskHandle);
    // Criar tarefa de leitura sensor biométrico
    xTaskCreatePinnedToCore(readBiometry, "BiometryTask", 10000, NULL, 2, &biometryTaskHandle, 0);

    // Criar tarefa Servo abrir o cofre
    //xTaskCreate(servoOpenSafeTask, "ServoOpenSafeTask", 10000, NULL, 1, &servoOpenSafeTaskHandle);
    // Suspende a tarefa do Servo
    //vTaskSuspend(servoOpenSafeTaskHandle);
    // Criar tarefa Servo fechar o cofre
    //xTaskCreate(servoCloseSafeTask, "ServoCloseSafeTask", 10000, NULL, 1, &servoCloseSafeTaskHandle);
    // Suspende a tarefa do Servo
    //vTaskSuspend(servoCloseSafeTaskHandle);
  }

  // Criar tarefa Servo que abre a trava
  xTaskCreate(servoOpenLockTask, "ServoOpenLockTask", 10000, NULL, 1, &servoOpenLockTaskHandle);
  // Suspende a tarefa do Servo
  vTaskSuspend(servoOpenLockTaskHandle);
  // Criar tarefa Servo que fecha a trava
  xTaskCreate(servoCloseLockTask, "ServoCloseLockTask", 10000, NULL, 1, &servoCloseLockTaskHandle);
  // Suspende a tarefa do Servo
  vTaskSuspend(servoCloseLockTaskHandle);

  // Inicializa o servidor
  server.begin();
}

void loop() {
  // Não há nada a ser feito aqui, as tarefas estão em execução
}

// Verifica qual a função do ESP32 que foi carregado o código
String getProcessFromThingSpeak(String mac) {
  // Construir a URL da consulta
  String url = "http://api.thingspeak.com/channels/2358423/feeds.json?api_key=S3DKQ8N6OD5E2TN9";

  // Adicionar o endereço MAC à URL
  url += "&field1=" + mac;

  // Realizar a solicitação HTTP
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // Obter a resposta JSON
    String payload = http.getString();

    // Converter a string JSON para um objeto ArduinoJson
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, payload);

    // Acessar os feeds no JSON
    JsonArray feeds = jsonDoc["feeds"];

    // Verificar se há algum processo válido no feed mais recente
    if (feeds.size() > 0) {

      // Verificar se há algum processo válido no feed correspondente ao valor de "field1"
      for (JsonVariant feed : feeds) {
        String currentField1 = feed["field1"].as<String>();

        if (currentField1.equals(mac)) {
          // Correspondência encontrada, obter o valor de "field2"
          String field2_value = feed["field2"].as<String>();

          // Retorna o valor do "field2"
          return field2_value;
        }
      }

      // Se nenhum feed correspondente for encontrado
      Serial.println("Nenhum feed correspondente encontrado para o valor de field1 fornecido.");
      return "";
    }
  } else {
    Serial.print("Erro na solicitação HTTP: ");
    Serial.println(httpCode);
  }

  // Retorna uma string vazia se não encontrar correspondência ou houver um erro na solicitação HTTP
  return "";
}

// Função que verifica o status do ESP (cadastro ou leitura)
void verifyStatusButton(void *pvParameters) {
  (void)pvParameters;  // Parâmetro com o processo

  while (1) {

    // Lê o estado do botão
    int statusButton = digitalRead(BTN_OFF_PIN);

    // Verifica se o botão foi pressionado
    if (statusButton == LOW) {

      Serial.println("Botão bloquear pressionado!");

      // Inicia a tarefa de desligar o Servo final
      //vTaskResume(servoCloseSafeTaskHandle);

      String ip = "";

      // Enquanto não encontrar o IP deve permanecer na busca
      while (ip == "") {
        //Busca o IP do RFID
        ip = getIPFromThingSpeak("R");
        // Aguarda por 5 segundos (tempo entre requisições)
        vTaskDelay(pdMS_TO_TICKS(5000));
      }

      Serial.print("Ip RFID: ");
      Serial.println(ip);

      // Chama função responsável em bloquear o acesso nos outros ESP
      activateRouteAnotherESP(ip);

      ip = "";

      // Enquanto não encontrar o IP deve permanecer na busca
      while (ip == "") {
        //Busca o IP do teclado
        ip = getIPFromThingSpeak("K");
        // Aguarda por 5 segundos (tempo entre requisições)
        vTaskDelay(pdMS_TO_TICKS(5000));
      }

      Serial.print("Ip Teclado: ");
      Serial.println(ip);

      // Chama função responsável em bloquear o acesso nos outros ESP
      activateRouteAnotherESP(ip);

      // Controla os leds que devem ligar
      controlLedRGB(true, false, false);
      // Inicia a tarefa de desligar o Servo da trava
      vTaskResume(servoCloseLockTaskHandle);

      //Busca as informações no ThingSpeak
      ThingSpeakData result = readLastEntryFromThingSpeak();

      // Se reiniciou o sistema no meio do desbloqueio reseta contador
      sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, result.field4, result.field5, "0");

      // Suspende a tarefa de verificar o status
      vTaskSuspend(NULL);
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Aguarda por 100 milisegundos
  }
}

// Aciona a rota nos outros ESP para bloquear as travas
void activateRouteAnotherESP(String ip) {

  HTTPClient http;

  // URL completa para acionar a rota "/off" no outro ESP32
  String url = "http://" + String(ip) + "/off";

  Serial.println("Enviando requisição para acionar rota no outro ESP32...");

  // Envia a requisição HTTP GET
  http.begin(url);
  int httpCode = http.GET();

  // Verifica se a requisição foi bem-sucedida
  if (httpCode > 0) {
    Serial.printf("Resposta do servidor: %d\n", httpCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("Falha na requisição HTTP: %s\n", http.errorToString(httpCode).c_str());
  }

  // Libera os recursos da requisição HTTP
  http.end();
}

// Verifica qual a função do ESP32 que foi carregado o código
String getIPFromThingSpeak(String process) {
  // Construir a URL da consulta
  String url = "http://api.thingspeak.com/channels/2358574/feeds.json?api_key=ER1BK4FV94PPHJVK";

  // Realizar a solicitação HTTP
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // Obter a resposta JSON
    String payload = http.getString();

    // Converter a string JSON para um objeto ArduinoJson
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, payload);

    // Acessar os feeds no JSON
    JsonArray feeds = jsonDoc["feeds"];

    // Inicializar uma variável para armazenar o último IP correspondente
    String lastIP = "";

    // Iterar sobre os feeds (do mais recente para o mais antigo)
    for (int i = feeds.size() - 1; i >= 0; --i) {
      // Obter o feed atual
      JsonVariant feed = feeds[i];

      // Verificar se o valor de "field2" é igual ao fornecido
      if (feed["field2"].as<String>() == process) {
        // Valor de "field2" é correspondente, obter o valor de "field1" (IP)
        lastIP = feed["field1"].as<String>();
        // Parar a iteração, pois já encontramos o último IP correspondente
        return lastIP;
      }
    }
  } else {
    Serial.print("Erro na solicitação HTTP: ");
    Serial.println(httpCode);
  }

  // Retorna uma string vazia se não encontrar correspondência ou houver um erro na solicitação HTTP
  return "";
}

// Função de leitura do último registro do ThingSpeak
ThingSpeakData readLastEntryFromThingSpeak() {

  ThingSpeakData data;  // Objeto para armazenar os valores dos campos

  // Construir a URL para ler o último registro do ThingSpeak
  String url = "http://api.thingspeak.com/channels/2354855/feeds/last.json?api_key=7GNSQB81HFDQPX5C";

  // Criar objeto HTTPClient
  HTTPClient http;

  // Enviar a requisição HTTP com o método GET
  http.begin(url);
  int httpCode = http.GET();

  // Verificar se a requisição foi bem-sucedida
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Criar um objeto JSON para analisar a resposta
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, payload);

    // Extrair os valores dos campos
    data.field1 = jsonDoc["field1"].as<String>();
    data.field2 = jsonDoc["field2"].as<String>();
    data.field3 = jsonDoc["field3"].as<String>();
    data.field4 = jsonDoc["field4"].as<String>();
    data.field5 = jsonDoc["field5"].as<String>();
    data.field6 = jsonDoc["field6"].as<String>();
  } else {
    Serial.printf("Erro ao ler último registro do ThingSpeak. Código de status: %d\n", httpCode);
  }

  // Liberar recursos
  http.end();

  // Retornar os valores dos campos
  return data;
}

// Envia informações para o ThingSpeak
void sendToThingSpeak(String apiKey, String field1, String field2, String field3, String field4, String field5, String field6) {

  // Número do registro que será modificado
  int entryId = 1;

  // Construir a URL para enviar dados para o ThingSpeak
  String url = "http://api.thingspeak.com/update.json";

  // Criar objeto HTTPClient
  HTTPClient http;

  // Configurar dados para enviar
  String postData = "api_key=" + apiKey + "&entry_id=" + String(entryId);

  // Usuario
  if (field1 != "")
    postData += "&field1=" + field1;
  // Senha
  if (field2 != "")
    postData += "&field2=" + field2;
  // Biometria
  if (field3 != "")
    postData += "&field3=" + field3;
  // RFID
  if (field4 != "")
    postData += "&field4=" + field4;
  // Cadastro
  if (field5 != "")
    postData += "&field5=" + field5;
  // Etapa
  if (field6 != "")
    postData += "&field6=" + field6;

  // Enviar a requisição HTTP com o método POST
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);

  // Verificar se a requisição foi bem-sucedida
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Registro atualizado com sucesso no ThingSpeak!");
  } else {
    Serial.printf("Erro ao atualizar registro no ThingSpeak. Código de status: %d\n", httpCode);
  }

  // Liberar recursos
  http.end();
}

// Controla os Leds RGB
void controlLedRGB(bool ledRedOn, bool ledGreenOn, bool ledBlueOn) {
  digitalWrite(RED_PIN, ledRedOn);
  digitalWrite(GREEN_PIN, ledGreenOn);
  digitalWrite(BLUE_PIN, ledBlueOn);
}

// Tarefa para realizar a leitura RFID
void readRFID(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  // Inicializar a chave de leitura RFID
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  while (1) {
    // Procurar por novos cartões RFID
    if (rfid.PICC_IsNewCardPresent()) {
      // Verificar se o NUID foi lido
      if (rfid.PICC_ReadCardSerial()) {

        // Armazenar NUID no array nuidPICC
        for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
        }

        // Armazena o valor lido no RFID
        String valueRFID = printDec(rfid.uid.uidByte, rfid.uid.size);

        Serial.print("RFID: ");
        Serial.print(valueRFID);
        Serial.println();

        // Busca os dados cadastrado no ThingSpeak
        ThingSpeakData result = readLastEntryFromThingSpeak();

        //Verifica se está cadastrando ou fazendo a leitura
        if (result.field5 == "S") {

          // Controla os leds que devem ligar
          controlLedRGB(false, false, true);

          // Verifica se já existe um RFID cadastrado
          if (result.field4.isEmpty() || result.field4 == "null") {

            // Verifica quantos cadastros já foi feito
            int countStep = result.field6.toInt() + 1;

            // Status leitura/cadastro
            String status = countStep == 3 ? "N" : result.field5;

            //Se chegar a 3 reseta a contagem
            countStep = countStep == 3 ? 0 : countStep;

            //Envia RFID para o ThingSpeak
            sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, valueRFID, status, String(countStep));

            Serial.println("RFID Cadastrado!");

            // Atraso usando vTaskDelay
            vTaskDelay(pdMS_TO_TICKS(1000));

          } else
            Serial.println("RFID já foi cadastrado!");

          // Controla os leds que devem ligar
          controlLedRGB(true, false, false);

        } else {

          // Verifica se leu o RFID correto
          if (result.field4 == valueRFID) {
            if (result.field6 == "0") {
              Serial.println("RFID Desbloquado!");
              // Controla os leds que devem ligar
              controlLedRGB(false, true, false);
              // Verifica quantos cadastros já foi feito
              int countStep = result.field6.toInt() + 1;
              //Envia desbloqueio para o ThingSpeak
              sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, result.field4, result.field5, String(countStep));
              // Inicia a tarefa de registro do Servo
              vTaskResume(servoOpenLockTaskHandle);
            } else
              Serial.println("Necessário seguir a ordem de desbloqueio!");
          } else
            Serial.println("RFID Incorreto!");
        }

        // Atraso usando vTaskDelay
        vTaskDelay(pdMS_TO_TICKS(5000));

        // Parar o PICC
        rfid.PICC_HaltA();

        // Parar a criptografia no PCD
        rfid.PCD_StopCrypto1();
      }
    }

    // Atraso usando vTaskDelay
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Função para imprimir valores decimais
String printDec(byte *buffer, byte bufferSize) {
  String rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    rfid += String(buffer[i] < 0x10 ? " 0" : " ");
    rfid += String(buffer[i], DEC);
  }
  return rfid;
}

// Tarefa para realizar a leitura do teclado numérico
void readKeypad(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    // Chamar a função para obter a senha
    char *valueKeypad = writeKeypad();

    // Imprimir senha na Serial
    Serial.print("Senha digitada: ");
    Serial.println(valueKeypad);

    // Busca os dados cadastrado no ThingSpeak
    ThingSpeakData result = readLastEntryFromThingSpeak();

    //Verifica se está cadastrando ou fazendo a leitura
    if (result.field5 == "S") {

      // Controla os leds que devem ligar
      controlLedRGB(false, false, true);

      // Verifica se já existe um RFID cadastrado
      if (result.field2.isEmpty() || result.field2 == "null") {

        // Verifica quantos cadastros já foi feito
        int countStep = result.field6.toInt() + 1;

        // Status leitura/cadastro
        String status = countStep == 3 ? "N" : result.field5;

        //Se chegar a 3 reseta a contagem
        countStep = countStep == 3 ? 0 : countStep;

        //Envia RFID para o ThingSpeak
        sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, valueKeypad, result.field3, result.field4, status, String(countStep));

        Serial.println("Senha Cadastrada!");

        // Atraso usando vTaskDelay
        vTaskDelay(pdMS_TO_TICKS(1000));

      } else
        Serial.println("Senha já foi cadastrada!");

      // Controla os leds que devem ligar
      controlLedRGB(true, false, false);

    } else {

      // Verifica se digitou a senha correta
      if (result.field2 == valueKeypad) {
        if (result.field6 == "1") {
          Serial.println("Senha Desbloquada!");
          // Controla os leds que devem ligar
          controlLedRGB(false, true, false);
          // Verifica quantos cadastros já foi feito
          int countStep = result.field6.toInt() + 1;
          //Envia desbloqueio para o ThingSpeak
          sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, result.field4, result.field5, String(countStep));
          // Inicia a tarefa de registro do Servo
          vTaskResume(servoOpenLockTaskHandle);
        } else
          Serial.println("Necessário seguir a ordem de desbloqueio!");
      } else Serial.println("Senha Incorreta!");
    }

    // Atraso usando vTaskDelay
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// Função para registrar o teclado numérico e obter a senha
char *writeKeypad() {

  char inputBuffer[6] = { '\0' };  // Buffer para armazenar os caracteres digitados (5 caracteres + '\0')
  int charCount = 0;

  while (1) {
    char key = keypad.getKey();  // Obter a tecla pressionada

    if (key) {
      Serial.print("Tecla: ");
      Serial.println(key);

      inputBuffer[charCount] = key;  // Armazenar a tecla no buffer
      charCount++;

      if (charCount == 5) {
        // Se 5 caracteres foram digitados, retorne a senha
        return strdup(inputBuffer);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// Tarefa para realizar a leitura da biometria
void readBiometry(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    // Armazena o valor lido no sensor biométrico
    String valueBiometry = String(getFingerprintID());

    // Lê o estado do botão
    int statusButton = digitalRead(BTN_BIOMETRY_PIN);

    // Verifica se o botão foi pressionado
    if (statusButton == LOW) {
      valueBiometry = "110";
    }

    Serial.print("Biometria: ");
    Serial.print(valueBiometry);
    Serial.println();

    if (valueBiometry != "0") {

      // Busca os dados cadastrado no ThingSpeak
      ThingSpeakData result = readLastEntryFromThingSpeak();

      //Verifica se está cadastrando ou fazendo a leitura
      if (result.field5 == "S") {

        // Controla os leds que devem ligar
        controlLedRGB(false, false, true);

        // Verifica se já existe um RFID cadastrado
        if (result.field3.isEmpty() || result.field3 == "null") {

          // Verifica quantos cadastros já foi feito
          int countStep = result.field6.toInt() + 1;

          // Status leitura/cadastro
          String status = countStep == 3 ? "N" : result.field5;

          //Se chegar a 3 reseta a contagem
          countStep = countStep == 3 ? 0 : countStep;

          //Envia RFID para o ThingSpeak
          sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, valueBiometry, result.field4, status, String(countStep));

          Serial.println("Biometria Cadastrada!");

          // Atraso usando vTaskDelay
          vTaskDelay(pdMS_TO_TICKS(1000));

        } else
          Serial.println("Biometria já foi cadastrada!");

        // Controla os leds que devem ligar
        controlLedRGB(true, false, false);

      } else {

        // Verifica se leu o RFID correto
        if (result.field3 == valueBiometry) {
          if (result.field6 == "2") {
            Serial.println("Biometria Desbloquada!");
            // Controla os leds que devem ligar
            controlLedRGB(false, true, false);
            // Verifica quantos cadastros já foi feito
            int countStep = result.field6.toInt() + 1;
            //Envia desbloqueio para o ThingSpeak
            sendToThingSpeak("7GNSQB81HFDQPX5C", result.field1, result.field2, result.field3, result.field4, result.field5, String(countStep));
            // Inicia a tarefa de registro do Servo
            vTaskResume(servoOpenLockTaskHandle);
            // Inicia a tarefa de ligar o Servo final
            //vTaskResume(servoOpenSafeTaskHandle);
            // Inicia a tarefa de verificar o status do botão
            vTaskResume(verifyStatusButtonTaskHandle);
          } else
            Serial.println("Necessário seguir a ordem de desbloqueio!");
        } else
          Serial.println("Biometria Incorreta!");
      }

      // Atraso usando vTaskDelay
      vTaskDelay(pdMS_TO_TICKS(4000));
    }
    // Atraso usando vTaskDelay
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int getFingerprintID() {
  uint8_t p = fingerprintSensor.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem capturada!");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Nenhum dedo detectado.");
      return 0;
    default:
      Serial.println("Erro ao capturar a imagem. Tente novamente.");
      return 0;
  }

  p = fingerprintSensor.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida com sucesso!");
      break;
    default:
      Serial.println("Erro ao converter a imagem. Tente novamente.");
      return 0;
  }

  p = fingerprintSensor.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Impressão digital encontrada!");
    int idLeitura = fingerprintSensor.fingerID;  // Armazena o ID da impressão digital na variável
    Serial.print("ID da impressão digital encontrada: ");
    Serial.println(idLeitura);
    return idLeitura;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Impressão digital não encontrada.");
  } else {
    Serial.println("Erro na pesquisa de impressão digital. Tente novamente.");
    return 0;
  }
}

// Função que implementa a tarefa do servo
void servoOpenLockTask(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    //Tempo para conseguir suspender a Task antes de executar
    vTaskDelay(pdMS_TO_TICKS(100));

    // Conecta o servo ao pino GPIO
    ServoLock.attach(SERVO_PIN);

    // Controla os leds que devem ligar
    controlLedRGB(false, true, false);

    // Liga o servo
    ServoLock.write(180);

    // Aguarda por 150 milisegundos
    vTaskDelay(pdMS_TO_TICKS(150));

    // Pare o servo detendo-o
    ServoLock.detach();

    // Suspende a tarefa de ligar o servo
    vTaskSuspend(NULL);
  }
}

// Função que implementa a tarefa do servo
void servoCloseLockTask(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    //Tempo para conseguir suspender a Task antes de executar
    vTaskDelay(pdMS_TO_TICKS(100));

    // Conecta o servo ao pino GPIO
    ServoLock.attach(SERVO_PIN);

    // Controla os leds que devem ligar
    controlLedRGB(true, false, false);

    // Desliga o servo
    ServoLock.write(0);

    // Aguarda por 150 milisegundos
    vTaskDelay(pdMS_TO_TICKS(150));

    // Pare o servo detendo-o
    ServoLock.detach();

    // Suspende a tarefa de ligar o servo
    vTaskSuspend(NULL);
  }
}

/*
// Função que implementa a tarefa do servo
void servoOpenSafeTask(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    //Tempo para conseguir suspender a Task antes de executar
    vTaskDelay(pdMS_TO_TICKS(100));

    // Conecta o servo ao pino GPIO
    ServoOpenSafe.attach(SERVO_OPEN_SAFE_PIN);

    // Liga o servo
    ServoOpenSafe.write(180);

    // Aguarda por 150 milisegundos
    vTaskDelay(pdMS_TO_TICKS(150));

    // Pare o servo detendo-o
    ServoOpenSafe.detach();

    // Suspende a tarefa de ligar o servo
    vTaskSuspend(NULL);
  }
}


// Função que implementa a tarefa do servo
void servoCloseSafeTask(void *pvParameters) {
  (void)pvParameters;  // Parâmetro não utilizado

  while (1) {

    //Tempo para conseguir suspender a Task antes de executar
    vTaskDelay(pdMS_TO_TICKS(100));

    // Conecta o servo ao pino GPIO
    ServoOpenSafe.attach(SERVO_OPEN_SAFE_PIN);

    // Desliga o servo
    ServoOpenSafe.write(0);

    // Aguarda por 150 milisegundos
    vTaskDelay(pdMS_TO_TICKS(150));

    // Pare o servo detendo-o
    ServoOpenSafe.detach();

    // Suspende a tarefa de ligar o servo
    vTaskSuspend(NULL);
  }
}*/