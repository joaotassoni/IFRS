/*
Sistemas Microcontrolados I - 2017

Aluno: João Pedro Tassoni
Turma: Eletrônica 3
*/

#include<LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

byte pinoPotenciometro = 8;
byte pinoBotoes = 0;
int posicoes[3];
unsigned long lastmillis = millis();
byte lado = 2;

String msg = "abcdefghijklmnopqrstuvwxyz123456789!@#$%¨&*()";
String linha1 = "                    ", linha2 = "                    ";
String esp = "                                ";
int tamanhoString = msg.length() + 32;
  
void setup()
{
  lcd.begin(16,2);
  msg = esp + msg + esp;
}

void loop()
{
  
  // define a velocidade do scroll (em milisegundos) pela leitura do potenciômetro
  int velocidade = (analogRead(pinoPotenciometro)*750.0/1023.0)+250;
  
  // define o sentido do scroll pelo botão pressionado
  if((analogRead(pinoBotoes) > 380 )&&(analogRead(pinoBotoes)<555))
  {
    lado = 0;
        
    posicoes[0] = 0;
    posicoes[1] = 15;
    posicoes[2] = 30; 
  } // se o botão LEFT foi pressionado, lado = esquerda
  
  if(analogRead(pinoBotoes) < 50)
  {
    lado = 1;
    
    posicoes[0] = tamanhoString;
    posicoes[1] = tamanhoString + 15;
    posicoes[2] = tamanhoString + 31; 
  } //se o botão RIGHT foi pressionado, lado = direita

  if((analogRead(pinoBotoes) > 600) && (analogRead(pinoBotoes) < 790))
    lado = 2; // pausa o scroll até o sentido ser escolhido novamente

  scrollMsg(lado,velocidade);
}

boolean scrollMsg(byte sentido, unsigned long intervalo){
  int x,y; 

  switch(sentido)
  {
  
    /*
    SCROLL PARA A ESQUERDA
    */
    case 0:
    
      for(x = posicoes[0]; x <= posicoes[1]; x++)
      {
        linha1[y] = msg[x];
        y++;
      }
      y=0;
    
      for(x = posicoes[1]; x <= posicoes[2]; x++)
      {
        linha2[y] = msg[x];
        y++;
      }
      y=0;
  
      if(millis() - lastmillis >= intervalo)
      {
        lcd.setCursor(0,0);
        lcd.print(linha1);
        lcd.setCursor(0,1);
        lcd.print(linha2);
            
        posicoes[0] = posicoes[0] + 1;
        posicoes[1] = posicoes[1] + 1;
        posicoes[2] = posicoes[2] + 1;
      
        lastmillis = millis();      
      }
  
      if(posicoes[0] == tamanhoString)
      {
        posicoes[0] = 0;
        posicoes[1] = 15;
        posicoes[2] = 31; 
      }
    break;
 
    /*
    SCROLL PARA A DIREITA
    */
    case 1:
    
      for(x = posicoes[0];x <= posicoes[1]; x++)
      {
        linha1[y] = msg[x];
        y++;
      }
      y = 0;
    
      for(x=posicoes[1];x<=posicoes[2];x++)
      {
        linha2[y] = msg[x];
        y++;
      }
      y = 0;
  
      if(millis() - lastmillis >= intervalo)
      {
        lcd.setCursor(0,0);
        lcd.print(linha1);
        lcd.setCursor(0,1);
        lcd.print(linha2);
      
        posicoes[0] = posicoes[0] - 1;
        posicoes[1] = posicoes[1] - 1;
        posicoes[2] = posicoes[2] - 1;
      
        lastmillis = millis();
      }
  
      if(posicoes[2] == 31)
      {
        posicoes[0] = tamanhoString;
        posicoes[1] = tamanhoString + 15;
        posicoes[2] = tamanhoString + 31;
      }
    
    break;
  
    /*
    LIMPA O DISPLAY
    */
    case 2:
      lcd.clear();
    break;
  }
  
}
