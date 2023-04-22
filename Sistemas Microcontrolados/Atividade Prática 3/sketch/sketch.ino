/*
Sistemas Microcontrolados I - 2017

Aluno: João Pedro Tassoni
Turma: Eletrônica 3
*/

/*
 DECLARAÇÃO DE VARIÁVEIS
*/

byte pinoA = 12 ,pinoB = 11,pinoC = 10,pinoD = 9,pinoE = 8,pinoF = 6,pinoG = 7; // pinos do display associados à placa
byte bota = 4,stata = 0,oldstata = 0; // pinos para leitura do botão 1
byte botb = 3,statb = 0,oldstatb = 0; // pinos para leitura do botão 2
byte tr1 = 30,tr2 = 31,tr3= 32; // pinos dos transistores na placa
byte controle = 0; // variável usada para controlar uma maquina de estados relacionada aos displays
byte pinoAnalog = 0; // pino ligado ao potenciometro
int numeroPotNew = 0,numeroPotOld = 0,numeroSerial = 0,numeroFinal = 0,numeroAtual = 0; // números do cronometro
int numeroExibido = 0; // número a ser exibido no display
unsigned long contA = 0,contB = 0; // contador de tempo
int com = 0; // retorno do botão, serve para controlar a o começo, recomeço e reset da contagem
byte led = 13;

/*
 VARIÁVEIS RESPONSÁVEIS PELA LEITURA DA SERIAL
*/

#define cmdBufferSize 20
typedef void(*cmdCallback)(char*); // protótipo de variável para realizar comandos
const char cmdTerminator = '!'; // indicador de que o comando terminou
const char cmdParSeparator = '='; // separador do parâmetro

typedef struct 
{
  char* cmdName; // string que corresponde ao comando
  byte cmdNameSize; // tamanho da string com o nome do comando
  int cmdCode; // código numérico correspondendo ao comando
  boolean hasArgument; // indica se o comando possui ou não argumento
  cmdCallback callback; // callback para função de tratamento do comando
} cmdStruct;

/*
 FUNÇÃO (CALLBACK) PARA REALIZAR AÇÃO A PARTIR DO COMANDO
*/

void parar(char* param)
{
  String par = String(param);
  int pin = par.toInt(); // essas duas linhas transformam o parâmetro (número em formato de caracter) em um inteiro
 
  if((pin>0) && (pin<100))
  {
    if(com==0)
    {
      String msg = "OK. Tempo de ";
      msg = msg+par+" definido!";
      Serial.println(msg);
      numeroSerial = pin; // define o tempo de parada do contador
      numeroAtual = pin;
    }
  } 
  else 
  {
    Serial.println("O tempo deve estar entre 1 e 99!");
  }
}

/*
 COMANDOS ACEITOS
*/

int maxCmds = 1;
cmdStruct comandos[1] = 
{ // número máximo de comandos aceitos dentro das chaves []
  {"parar",5,0,true,parar}
};

/*
 PROCESSA O COMANDO
*/

void cmdProcess(char* msg, int msgsize)
{
  char cmd[cmdBufferSize];
  char args[cmdBufferSize];
  // limpa os buffers
  memset(cmd,0,cmdBufferSize);
  memset(args,0,cmdBufferSize);
  bool commandFound = false;
  // verifica se existem parâmetros do comando
  int pos = 0;
  int argSize = 0;
  for(int i=0; i<msgsize; i++) 
  {
    if (msg[i]==cmdParSeparator)
    {
    // achou o separador entre comando e argumento
    pos = i;
    }
  }

  // se tiver parametro, separa o mesmo e o comando em si
  if (pos != 0)
  {
    strncpy(cmd,msg,pos); // o texto do comando vai pra cmd
    argSize = msgsize-pos-1;
    strncpy(args,(msg+pos+1),argSize); // o texto do parâmetro (sem o =) vai pra args.
  } 
  else
  {
    strncpy(cmd,msg,msgsize); // só tem comando...
  }
 
  // agora testa qual é o comando
  for (int i=0; i<=maxCmds; i++) 
  {
    if (strcmp(cmd,comandos[i].cmdName)==0)
    {
      // achou o comando!!
      commandFound = true;
      // aciona a callback do comando...
      comandos[i].callback(args);
    }
  }

  if (!commandFound)
  {
    Serial.print("Comando não encontrado: ");
    Serial.println(msg);
  }
}

/*
 RECEBE O COMANDO PELA SERIAL
*/

void serialEvent() {
  static char cmdBuffer[cmdBufferSize] = "";
  static byte cmdBufferPos = 0;
  if (Serial.available() > 0)
  {
    int readByte = Serial.read();
      if (readByte != -1)
      {
      // é uma leitura válida...
      // verifica se é um término de comando
      if (readByte == cmdTerminator)
      {
        // comando acabou... manda para processamento
        cmdProcess(cmdBuffer,cmdBufferPos);
        // indica que o buffer foi usado ("limpo")
        cmdBufferPos = 0;
        // desencargo de consciencia (desnecessário... mas...)
        memset(cmdBuffer,0,cmdBufferSize);
      } 
      else
      {
        // coloca o caracter na posição e incrementa a posição para próxima leitura
        cmdBuffer[cmdBufferPos] = readByte;
        cmdBufferPos++;
      }
    }
  }
}

/*
FUNÇÕES
*/

void setup()
{  
  int i;
  for(i = 2;i < 53;i++)
  {
    pinMode(i,OUTPUT);
  }  
  
  Serial.begin(115200);
  pinMode(bota,INPUT);
  pinMode(botb,INPUT);
  
  oldstata=digitalRead(bota);
  oldstatb=digitalRead(botb); 
  
  digitalWrite(led,HIGH);
  
  contA=millis();
  contB=millis();
  
  Serial.println("Aguardando comandos");
  numeroPotOld=99*(analogRead(pinoAnalog))/1023;
}


void loop() {
  
  // leitura do potenciômetro
  if(com == 0)
  {
    float val=analogRead(pinoAnalog);
    numeroPotNew=99*val/1023;
    if((numeroPotOld+numeroPotNew>=numeroPotOld+1)||(numeroPotOld+numeroPotNew<=numeroPotOld-1))
    {
      numeroAtual = numeroPotNew;
      numeroPotOld = numeroPotNew;
    }
  }
    
  // leitura dos botões
  stata = digitalRead(bota);
  if(stata != oldstata)
  {
    if(stata == HIGH)
    {
      numeroFinal = numeroAtual;
      numeroAtual = 0;
      com = 1;
      digitalWrite(tr3,LOW);
    }
    oldstata = stata;
  }
  
  statb = digitalRead(botb);
  if(statb != oldstatb)
  {
    if(statb == HIGH)
    {
      com = 0;
      numeroSerial= 0;
      digitalWrite(led,LOW);
    }
    oldstatb = statb;
  }
  
  // controle das chaves
  switch(controle)
  {
    case 0:
      // ativa o display de DEZENA, e exibe o valor a ser setado ou o valor da contagem em andamento
      digitalWrite(tr1,HIGH);
      digitalWrite(tr2,LOW);
      numeroExibido=numeroAtual/10;
    break;
    
    case 1:
      // limpa o display de dezena
      digitalWrite(tr1,HIGH);
      digitalWrite(tr2,LOW);
      numeroExibido = 10;
    break;
    
    case 2:
      // ativa o display de UNIDADE, e exibe o valor a ser setado ou o valor da contagem em andamento
      digitalWrite(tr1,LOW);
      digitalWrite(tr2,HIGH);
      numeroExibido=numeroAtual%10;
    break;
    
    case 3:
      // limpa o display de unidade
      digitalWrite(tr1,LOW);
      digitalWrite(tr2,HIGH);
      numeroExibido = 10;
    break; 
    
    case 4:
      controle = 0;
    break;
  }

  // exibição de números no display
  switch(numeroExibido)
  {
      case 0:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,HIGH);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,LOW);
      break;
      case 1:
        digitalWrite(pinoA,LOW);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,LOW);digitalWrite(pinoE,LOW);digitalWrite(pinoF,LOW);digitalWrite(pinoG,LOW);
      break;
      case 2:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,LOW);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,HIGH);digitalWrite(pinoF,LOW);digitalWrite(pinoG,HIGH);
      break;
      case 3:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,LOW);digitalWrite(pinoF,LOW);digitalWrite(pinoG,HIGH);
      break;
      case 4:
        digitalWrite(pinoA,LOW);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,LOW);digitalWrite(pinoE,LOW);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,HIGH);
      break;
      case 5:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,LOW);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,LOW);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,HIGH);
      break;
      case 6:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,LOW);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,HIGH);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,HIGH);
      break;
      case 7:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,LOW);digitalWrite(pinoE,LOW);digitalWrite(pinoF,LOW);digitalWrite(pinoG,LOW);
      break;
      case 8:
        digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,HIGH);digitalWrite(pinoE,HIGH);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,HIGH);
      break;
      case 9:
       digitalWrite(pinoA,HIGH);digitalWrite(pinoB,HIGH);digitalWrite(pinoC,HIGH);digitalWrite(pinoD,LOW);digitalWrite(pinoE,LOW);digitalWrite(pinoF,HIGH);digitalWrite(pinoG,HIGH);
      break;
      case 10:
       digitalWrite(pinoA,LOW);digitalWrite(pinoB,LOW);digitalWrite(pinoC,LOW);digitalWrite(pinoD,LOW);digitalWrite(pinoE,LOW);digitalWrite(pinoF,LOW);digitalWrite(pinoG,LOW);
      break;      
   }
      
  // controle de tempo para os transistores
  if(millis() - contA >= 5)
  {
    controle++;
    contA = millis();
  }
  
  // controle de tempo para o contador (cronômetro)
  if(millis() - contB >= 1000)
  {
    if(com == 1){
      if(numeroAtual < numeroFinal){
         numeroAtual++;
      }
    }
    contB = millis();
  }

  // acende um LED quando o contador chegar ao final
  if(numeroAtual == numeroFinal)
  {
    if(com==1)
    {
    digitalWrite(tr3,HIGH);
    }
  }
  else
  { 
    digitalWrite(tr3,LOW);
  }
}
