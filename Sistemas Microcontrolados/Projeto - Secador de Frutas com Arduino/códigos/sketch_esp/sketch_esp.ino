/*
Sistemas Microcontrolados II - 2018

Projeto I - Secador de Frutas com Arduino - Código ESP8266

Alunos: João Pedro Tassoni e Filipy Dias
Turma: Eletrônica 4
*/

/* BIBLIOTECAS */

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>

/* VARIÁVEIS */
byte comando = 0;
String msginor = "                              ";
String msginoe = "                              ";

char tempoFalta[8] = {'0','0',':','0','0',':','0','0'};

unsigned long tempo;

uint8_t statusAnt   =  0;

char tempInt[3] = {'0','0','0'};
char tempExt[3] = {'0','0','0'};
byte iniciar = 0;

String fruta = " ";

const char* mqtt_server = "m10.cloudmqtt.com";
const char* mqtt_user   = "jxxybjbi";
const char* mqtt_pass   = "vmXqueresj-x";
const int   mqtt_port   = 18865;

WiFiClient espClient;
PubSubClient client(espClient);

/*FUNÇÕES*/

void setup()
{
  Serial.begin(115200); // inicializa a Serial
  WiFiManager wifiManager; // inicializa a função WiFiManager
  
  client.setServer(mqtt_server, mqtt_port); // conecta ao servidor e porta especificados
  client.setCallback(callback); // define a função a ser chamada quando receber informações do servidor

  if (WiFi.isConnected() != true)  // se nenhuma rede estiver conectada
    wifiManager.autoConnect("Projeto"); // cria um AP para conectar com o celular e configurar uma conexão
    
  delay(25);

  tempo = millis(); // inicializa o contador
}

void loop()
{
  if (millis() -  tempo > 1000)
  { // atualiza informações a cada segundo
    hora(); // tempo restante
    delay(10);
    recebeMSGarduino();
    client.publish("des/hora",tempoFalta);
    temperaturaI(); // temperatura interna da caixa
    delay(10);
    recebeMSGarduino();
    client.publish("des/tempint",tempInt);
    temperaturaE(); // temperatura interna do protótipo
    delay(10);
    recebeMSGarduino();
    client.publish("des/tempext",tempExt);
    estado(); // processo em andamento
    delay(10);
    recebeMSGarduino();
      
    if(iniciar == 1) { // envia para o servidor o valor de iniciar (para atualizar o botão) e o de fruta (para manter enquanto há um processo em andamento)
      client.publish("des/ini","1");
      client.publish("des/fruta","a");
    }
    
    tempo = millis(); // reseta o contador de tempo
  }

  if (!client.connected())
  { // verifica a conexão do ESP com o Servidor
    delay(25);
    reconnect();
  }

  client.loop();

  enviaMSGarduino(); // chama a função para enviar mensagens do ESP para o Arduino
}

void recebeMSGarduino()
{ // recebe e interpreta mensagens da placa Arduino
  msginor = Serial.readStringUntil(';');

  Serial.print(msginor);
  delay(10);

  if (msginor[0] == '1')
  { // recebe temperatura e converte para a variável tempInt
    String T = "    ";
    T[0] = msginor[6];
    T[1] = msginor[7];
    T[2] = msginor[8];
    T[3] = msginor[9];
    T[4] = msginor[10];

    sprintf(tempInt,"%d", T.toInt());
  }

  if (msginor[0] == '2')
  { // recebe temperatura e converte para a variável tempExt
    String T = "    ";
    T[0] = msginor[6];
    T[1] = msginor[7];
    T[2] = msginor[8];
    T[3] = msginor[9];
    T[4] = msginor[10];

    sprintf(tempInt,"%d", T.toInt());
  }

  if (msginor[0] == '0')
  { // recebe e converte o tempo que falta do processo para uma variável (tempoFalta)
    tempoFalta[0] = msginor[6];
    tempoFalta[1] = msginor[7];
    tempoFalta[2] = msginor[8];
    tempoFalta[3] = msginor[9];
    tempoFalta[4] = msginor[10];
    tempoFalta[5] = msginor[11];
    tempoFalta[6] = msginor[12];
    tempoFalta[7] = msginor[13];
  }

  if (msginor[0] == "3")
  {
    iniciar = msginor[2];
  }

}

void enviaMSGarduino()
{ // envia mensagens para a placa Arduino
  Serial.write(msginoe);
}

void hora()
{ // envia uma mensagem para "pedir" ao Arduino o tempo que falta para o processo acabar
  Serial.write("9;");
}

void temperaturaI()
{ // envia uma mensagem para "pedir" ao Arduino a temperatura da caixa
  Serial.write("10;");
}

void temperaturaE()
{ // envia uma mensagem para "pedir" ao Arduino a temperatura do protótipo
  Serial.write("11;");
}

void estado()
{ // envia uma mensagem para "perguntar" ao Arduino se há algum processo em andamento
  Serial.write("12;");
}

void reconnect()
{ // tenta a reconexão com o servidor MQTT
  delay(50);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("regfdvrgerfergfgh", mqtt_user, mqtt_pass))
    {
      Serial.println("connected");
      client.subscribe("des/ini");
    } 
    else
    {
      Serial.println("Try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{ // função que é chamada sempre que o servidor manda algo
  // verifica a mensagem recebida do Servidor
  
  String payloadStr = "";

  for (int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  } // passa a mensagem recebida para uma String

  Serial.println();
  if (String(topic).equals("des/fru"))
  {
    if ((payloadStr.equals("liga")) || (payloadStr.equals("1"))) 
    {
      msginoe = "1;"; // compara a mensagem recebida com "1" ou "liga" e envia uma mensagem correspondente para o Arduino 
    }
    else if ((payloadStr.equals("desliga")) || (payloadStr.equals("0")))
    {
      msginoe = "2;";// compara a mensagem recebida com "0" ou "desliga" e envia uma mensagem correspondente para o Arduino
    }
  }
  if (String(topic).equals("des/fruta"))
  {
    if (payloadStr.equals("3"))
    { // compara a mensagem recebida com "3" e envia uma mensagem correspondente para o Arduino
      msginoe = "3;";
      fruta = '3';
    }
    else if (payloadStr.equals("4"))
    { // compara a mensagem recebida com "4" e envia uma mensagem correspondente para o Arduino
      msginoe = "4;";
      fruta = '4';
    }
    else if (payloadStr.equals("5"))
    { // compara a mensagem recebida com "5" e envia uma mensagem correspondente para o Arduino
      msginoe = "5;";
      fruta = '5';
    }
    else if (payloadStr.equals("6"))
    { // compara a mensagem recebida com "6" e envia uma mensagem correspondente para o Arduino
      msginoe = "6;";
      fruta = '6';
    }
    else if (payloadStr.equals("7"))
    { // compara a mensagem recebida com "7" e envia uma mensagem correspondente para o Arduino
      msginoe = "7;";
      fruta = '7';
    }
    else if (payloadStr.equals("8"))
    { // compara a mensagem recebida com "8" e envia uma mensagem correspondente para o Arduino
      msginoe = "8;";
      fruta = '8';
    }
  }
}
