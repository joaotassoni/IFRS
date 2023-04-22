/*
Sistemas Microcontrolados I - 2017

Aluno: João Pedro Tassoni
Turma: Eletrônica 3
*/

// constantes para facilitar o controle dos LED's e portas
const int R=8;
const int G=9;
const int B=10;
const int ENT=11;

// variáveis que vão servir de contadores, do switch-case e vezes que o pushbutton foi apertado, respectivamente
int cont1=0;
int cont2=0;

// variáveis para controlar o status do botão
int stt1;
int stt2;

// variáveis para contar o tempo (com millis)
unsigned long oldm;
unsigned long newm;


void setup()
{
  // declaração do modo de operação de cada porta, usando as variáveis para facilitar a representação
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(ENT,INPUT);
  
  // igualando os valores indicados para a primeira execução do programa
  oldm=millis();
  stt1=digitalRead(ENT);
}

void loop()
{
  // igualando a variável newm à millis no início de cada loop
  newm=millis();
  
  // condições para fazer a leitura do botão e registrar na variável cont2
  stt2=digitalRead(ENT);
  if(stt1!=stt2)
  {
  	if(stt2==0)
    {
      cont2++;
      if(cont2>5)cont2=0;
    }
    stt1=stt2;
  }

  // início da estrututa switch-case, com o uso da cont1 para controle  
  switch(cont1)
  {
  case 0:
    // ligando o LED verde, esperando 3 segundos, o desligando, incrementando cont1 e resetando o contador de tempo (igualando oldm à newm)
    digitalWrite(G,HIGH);
    if(newm-oldm>=3000)
    {
      digitalWrite(G,LOW);      
      cont1=1;
      oldm=newm;
    }
  break;

  case 1:
 	  // liga o LED azul, espera 1 segundo e o desliga, além de resetar o contador de tempo e decidir se vai incrementar cont1 ou
    // pular para o último caso, variando a ação dependendo do cont2 (que varia quando o botão é pressionado)
    digitalWrite(B,HIGH);
    if(newm-oldm>=1000)
    {
      digitalWrite(B,LOW);
      oldm=newm;       
      if(cont2<=1)
        cont1=6;
      else
        cont1=2;
    }
  break;    

  case 2:
    // espera 1 segundo (onde o LED azul fica desligado), depois liga o LED. Espera mais 1 segundo e desliga o LED azul, além de fazer
    // a mesma coisa que o caso de cima pra ver se incrementa ou pula para o final
    if(newm-oldm>=1000)
        digitalWrite(B,HIGH);
    if(newm-oldm>=2000)
    {
      digitalWrite(B,LOW);
      oldm=newm;        
      if(cont2==2)
        cont1=6;
      else
        cont1=3;
    }
  break;   

  case 3:
    // espera 1 segundo (onde o LED azul fica desligado), depois liga o LED. Espera mais 1 segundo e desliga o LED azul, além de fazer
    // a mesma coisa que o caso de cima pra ver se incrementa ou pula para o final
    if(newm-oldm>=1000)
    	digitalWrite(B,HIGH);
    if(newm-oldm>=2000)
    {
      digitalWrite(B,LOW);
      oldm=newm;        
      if(cont2==3)
        cont1=6;
      else
        cont1=4;}
  break;     

  case 4:
    // espera 1 segundo (onde o LED azul fica desligado), depois liga o LED. Espera mais 1 segundo e desliga o LED azul, além de fazer
    // a mesma coisa que o caso de cima pra ver se incrementa ou pula para o final  
    if(newm-oldm>=1000)
        digitalWrite(B,HIGH);
    if(newm-oldm>=2000){
      digitalWrite(B,LOW);
      oldm=newm;        
      if(cont2==4)
        cont1=6;
      else
        cont1=5;}
  break;     

  case 5:
    // espera 1 segundo (onde o LED azul fica desligado), depois liga o LED. Espera mais 1 segundo e desliga o LED azul, mas dessa
    // vez incrementa cont1 obrigatoriamente pulando para o último caso
    if(newm-oldm>=1000)
      digitalWrite(B,HIGH);
      
    if(newm-oldm>=2000)
    {
      digitalWrite(B,LOW);
      oldm=newm;        
      cont1=6;
    }
  break;    

  case 6:
    // espera 1 segundo, depois liga o LED vermelho por 2 segundos
    if(newm-oldm>=1000)
      digitalWrite(R,HIGH);
    if(newm-oldm>=3000)
    {
      // desliga o LED vermelho, zera o contador de tempo e zera a variável de controle cont1    
      digitalWrite(R,LOW);
      oldm=newm;
      cont1=0;
    }
  break;   
  }
  
  // se o contador de vezes pressionado for maior que 5 (o máximo especificado para o trabalho), zera o mesmo
  if(cont2>=6)
    cont2=0;
}