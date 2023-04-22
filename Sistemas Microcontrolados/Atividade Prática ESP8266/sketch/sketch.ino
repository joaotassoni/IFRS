/*
Sistemas Microcontrolados II - 2018

Alunos: João Pedro Tassoni e Filipy Dias
Turma: Eletrônica 4
*/

#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>

/* pinos */
int pinLed1 = D1;
int pinLed2 = D2;

int pinKey1 = D5;
int pinKey2 = D6;

int pinLM   = A0;

/* variáveis de Estado */
boolean led1 = 0;
boolean led2 = 0;

boolean key1 = 0;
boolean key2 = 0;

float LM = 0;

/* variáveis */
String mensagem = "        ";
unsigned long lastmillis = millis();

/* strings com páginas HTML */
String html ="<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> <a href=\"changeLed1\">Mudar LED 1</a> <br> <a href=\"changeLed2\">Mudar LED 2</a> <br><br> <a href=\"sensor1\">Estado KEY 1</a> <br> <a href=\"sensor2\">Estado KEY 2</a> <br><br> <a href=\"lm35\">Leitura LM35</a> </body> </html>";
String finalHTML = "<br><br><br><br><br> <a href=\"home\">Retornar para a pagina inicial</a> <br> </body> </html>";

String estadoLed1 =  "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 1 =        " + finalHTML;
String estadoLed2 =  "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 2 =        " + finalHTML;

String estadoKey1 =  "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 1 =        " + finalHTML;
String estadoKey2 =  "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 2 =        " + finalHTML;

String leituraLM35 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> LM 35 =        " + finalHTML;

ESP8266WebServer server(80);
WiFiClient cl; // cria o objeto cliente.

void setup()
{
  Serial.begin(115200);
  
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);

  pinMode(pinKey1, INPUT);
  pinMode(pinKey2, INPUT);

  // desliga os dois LED's
  digitalWrite(pinLed1, led1);
  digitalWrite(pinLed2, led2);
  
  
  // parte de código disponibilizado no moodle
  // força desconexão do NodeMCU

  WiFi.mode(WIFI_STA); // coloca em modo estação

  WiFi.disconnect(); // desconecta

  // determina a quantidade de redes visiveis pelo NodeMCU

  byte rn = WiFi.scanNetworks();

  Serial.print("Numero de redes: ");
  Serial.println(rn);
  Serial.println("Redes visiveis");
  Serial.print("Indice");
  Serial.print('\t');
  Serial.print('\t');
  Serial.print("Nome");
  Serial.print('\t');
  Serial.print('\t');
  Serial.println("Intensidade");

  // lista agora o nome (SSID) de cada rede

  for (int i = 0; i < rn; i++)
  {

    // recupera o nome (SSID) da rede

    Serial.print(i);
    Serial.print('\t');
    Serial.print('\t');

    String nome = WiFi.SSID(i);

    Serial.print(nome);

    // recupera intensidade do sinal

    long intensidade = WiFi.RSSI(i);

    Serial.print('\t');
    Serial.print('\t');
    Serial.println(intensidade);
  }

  // pede a seleção da rede para conexão...

  Serial.println("Selecione a rede - indique o numero");

  String valor = "";

  while (valor == "") 
  {
    valor = Serial.readString();
  }

  Serial.print("Rede selecionada: ");
  Serial.println(valor);
  Serial.println("Entre a senha");

  String senha = "";

  while (senha == "") 
  {
    senha = Serial.readString();
  }

  Serial.print("Senha informada: ");
  Serial.println(senha);

  // tenta realizar a conexão

  Serial.println("");
  Serial.println("Conectando...");


  // OBRIGATORIO ... conectar na rede

  // inicia a conexão (strings tem que ser strings C)

  WiFi.begin(WiFi.SSID(valor.toInt()).c_str(), senha.c_str());

  wl_status_t stat = WL_DISCONNECTED;

  while (stat != WL_CONNECTED)
  {

    stat = WiFi.status();

    Serial.println("...");

    if (stat == WL_NO_SSID_AVAIL)
    {
      Serial.println("Nome da rede inexistente");
      break;

    }

    if (stat == WL_CONNECT_FAILED)
    {
      Serial.println("Erro na conexao");
      break;
    }

    delay(200);
  }

  if (stat != WL_CONNECTED)
  {

    Serial.println("Nao foi possivel conectar");

  } 
  else 
  {
    if (stat == WL_CONNECTED)
      Serial.println("Conectado");
  }

  // mostra os parametros da conexao
  // mostra o endereço MAC

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // mostra endereço IP local

  Serial.print("IP local: ");

  IPAddress localIP = WiFi.localIP();

  Serial.println(localIP);

  //WEB SERVER
  // manda o cliente para a pagina desejada

  server.begin();

  server.on("/changeLed1", changeLed1);
  server.on("/changeLed2", changeLed2);
  server.on("/sensor1", sensor1);
  server.on("/sensor2", sensor2);
  server.on("/lm35", tSensor);
  server.on("/home", goHome);
  server.on("/", goHome);
  
  server.send(200, "text/html", mensagem);
}

void loop() 
{
  // faz a leitura de dados do servidor
  server.handleClient();
}

void changeLed1() 
{ // muda o estado do led 1 e o exibe no servidor
  led1 = !digitalRead(pinLed1);
  digitalWrite(pinLed1, led1);
  
  if(led1 == LOW)
    estadoLed1 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 1 DESLIGADO " + finalHTML;
  if(led1 == HIGH)
    estadoLed1 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 1 LIGADO " + finalHTML;
  
  server.send(200, "text/html", estadoLed1);
}

void changeLed2() 
{ // muda o estado do led 2 e o exibe no servidor
  led2 = !digitalRead(pinLed2);
  digitalWrite(pinLed2, led2);
  
  if(led2 == LOW)
    estadoLed2 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 2 DESLIGADO " + finalHTML;
  if(led2 == HIGH)
    estadoLed2 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Led 2 LIGADO " + finalHTML;

  server.send(200, "text/html", estadoLed2);
}

void sensor1()
{ // faz a leitura da chave 1 e exibe o seu estado no servidor
  key1 = digitalRead(pinKey1);
  
  if(key1 == LOW)
    estadoKey1 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 1 ABERTA " + finalHTML;
  if(key1 == HIGH)
    estadoKey1 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 1 FECHADA" + finalHTML;
  
  server.send(200, "text/html", estadoKey1);
}

void sensor2()
{ // faz a leitura da chave 2 e exibe o seu estado no servidor
  key2 = digitalRead(pinKey2);
  
  if(key2 == 0)
    estadoKey2 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 2 ABERTA " + finalHTML;
  if(key2 == 1)
    estadoKey2 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> Key 2 FECHADA" + finalHTML;

  server.send(200, "text/html", estadoKey2);
}

void tSensor() 
{ // faz a leitura do sensor LM35 e exibe a temperatura lida no servidor
  LM = ((analogRead(pinLM) * 330.0) / 1023.0);
  leituraLM35 = "<html> <body> <h2>Microcontrolados II</h2> Tarefa 3 - Joao e Filipy <br><br> LM 35 = " +  String(LM)  + " graus celsius" + finalHTML;
  
  server.send(200, "text/html", leituraLM35);
}

void goHome() 
{ // exibe a "página inicial" no servidor
  server.send(200, "text/html", html);
}
