//Bibliotecas Utilizadas
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

MDNSResponder mdns;
//Credenciais da rede
const char* ssid = "";
const char* password = "";

//Define a porta do server como 80
WebServer server(80);
String webpage = "";

//Portas utilizadas
int led1 = 13;  //Vermelho
int led2 = 12;  // Verde
int led3 = 14;  //Azul
int PIR = 25;
int LDR = 34;
int Touch = 32;

//Variaveis utilizadas
int cor = 6;
int ligado = 0;
int leitura;

//variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

void setup() {
  //Define os pinos de entrada e saida
  pinMode(PIR, INPUT);
  pinMode(LDR, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  //Inicia com os leds desligados
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);

  //Configura a taxa de transferência em bits por segundo
  Serial.begin(115200);

  //Inicializa as configurações da rede
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Informações sobre a conexão da rede
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  //Se receber do servidor /led1ON atualiza a variavel ligado com 1, só liga o led se a presença for detectada
  server.on("/led1ON", []() {
    server.send(200, "text/html", webpage);
    ligado = 1;
  });

  //Se receber do servidor /led1OFF atualiza a variavel ligado com 0
  server.on("/led1OFF", []() {
    server.send(200, "text/html", webpage);
    ligado = 0;
  });

  //Se receber do servidor /ledVermelho atualiza a variavel cor com 0
  server.on("/ledVermelho", []() {
    server.send(200, "text/html", webpage);
    cor = 0;
  });

  //Se receber do servidor /ledAzul atualiza a variavel cor com 2
  server.on("/ledAzul", []() {
    server.send(200, "text/html", webpage);
    cor = 2;
  });

  //Se receber do servidor /ledVerde atualiza a variavel cor com 1
  server.on("/ledVerde", []() {
    server.send(200, "text/html", webpage);
    cor = 1;
  });

  //Se receber do servidor /ledMagenta atualiza a variavel cor com 3
  server.on("/ledMagenta", []() {
    server.send(200, "text/html", webpage);
    cor = 3;
  });

  //Se receber do servidor /ledAmarelo atualiza a variavel cor com 4
  server.on("/ledAmarelo", []() {
    server.send(200, "text/html", webpage);
    cor = 4;
  });

  //Se receber do servidor /ledCiano atualiza a variavel cor com 5
  server.on("/ledCiano", []() {
    server.send(200, "text/html", webpage);
    cor = 5;
  });

  //Se receber do servidor /ledBranco atualiza a variavel cor com 6
  server.on("/ledBranco", []() {
    server.send(200, "text/html", webpage);
    cor = 6;
  });

  //Começa a executar as conexões de entrada
  server.begin();
  Serial.println("HTTP server started");

  //Cria uma nova tarefa qua sera executada no núcleo zero
  xTaskCreatePinnedToCore(
    coreTaskZero,   //Função para implementar a tarefa
    "coreTaskZero",        //Nome da tarefa
    10000,          //Numero de palavras alocadas
    NULL,           //Parametro de entrada para a tarefa
    1,              //Prioridade da tarefa
    NULL,           //Referencia para a tarefa
    taskCoreZero);  //Núcleo que executará a tarefa

  //Mostra em qual nucleo a tarefa esta executando
  String mensagem = "Executando setup no core  ";
  mensagem = mensagem + xPortGetCoreID();
  Serial.println(mensagem);
}

void loop() {

  //Mostra em qual nucleo a tarefa esta executando
  //String mensagem = "Executando loop no core  ";
  //mensagem = mensagem + xPortGetCoreID();
  // Serial.println(mensagem);
  //delay(2000);


  //Leitura do sensor LDR
  leitura = analogRead(LDR);

  //Quando o touch for acionado atualiza a variavel ligado com 1
  if (touchRead(Touch) <= 20) {
    ligado = 1;
  }
}

// Função para alterar a cor do led
void ligarLed() {

  //cor 0 = vermelho
  if (cor == 0) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);

    //cor 1 = verde
  } else if (cor == 1) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);

    //cor 2 = azul
  } else if (cor == 2) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);

    //cor 3 = Magenta
  } else if (cor == 3) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);

    //cor 4 = Amarelo
  } else if (cor == 4) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);

    //cor 5 = Ciano
  } else if (cor == 5) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);

    //cor 6 = branco
  } else if (cor == 6) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }
}

//Função responsavel por ligar o led
void coreTaskZero(void* pvParameters) {
  //Mostra em qual nucleo a tarefa esta executando
  String mensagem = "Executando a tarefa no core ";
  mensagem = mensagem + xPortGetCoreID();
  Serial.println(mensagem);

  //laço para a execução das tarefas
  while (true) {
    server.handleClient();

    //Condicional para ligar o led
    if (ligado == 1 || leitura < 3000) {  //se ligado = 1 ou valor da leitura menor do q o indicado, 
                                          //vai verificar se tem alguem no local
      if (digitalRead(PIR)) {             //Se a presença for detectada o led sera ligado
        ligado = 1;
        ligarLed();  //Chama a função para ligar o led
      }
    } else if (ligado == 0) {  //Se a variavel ligado for igual a 0 então os leds serão apagados
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
    }

    //Verifica se ainda tem alguem no ambiente
    if (!digitalRead(PIR)) {  //Se não encontrar ninguém ele atualiza o valor de ligado para zero
      ligado = 0;
    }
  }
}