/*
Sistemas Microcontrolados II - 2018

Projeto II - Carrinho Seguidor de Linha

Alunos: João Pedro Tassoni e Filipy Dias
Turma: Eletrônica 4
*/

// pinagem
byte pinSensorDireita  = A4;
byte pinSensorEsquerda = A5;
byte pinSensorFrente   = A6;

byte pinMotorDireitaA  = 34;
byte pinMotorDireitaB  = 36;

byte pinMotorEsquerdaA = 30;
byte pinMotorEsquerdaB = 32;

byte pinVelMotorDireita  = 46;
byte pinVelMotorEsquerda = 44;

// leitura de sensores
float sensorEsquerda = 0;
float sensorDireita  = 0;
float sensorFrente   = 0;

float velocidadeMotorDireita  = 60;
float velocidadeMotorEsquerda = 60;
float intervalo = 500;

// outras variáveis
int modo = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(pinMotorDireitaA, OUTPUT);
  pinMode(pinMotorEsquerdaA, OUTPUT);

  pinMode(pinMotorDireitaB, OUTPUT);
  pinMode(pinMotorEsquerdaB, OUTPUT);
}

void loop()
{
    leituraSensores();
    controlePosicao();
    moveMotor();
}

void leituraSensores()
{
  analogRead(pinSensorDireita);
  delay(1);
  int a = analogRead(pinSensorDireita);
  analogRead(pinSensorEsquerda);
  delay(1);
  int b = analogRead(pinSensorEsquerda);
  analogRead(pinSensorFrente);
  delay(1);
  int c = analogRead(pinSensorFrente);
  delay(1);

  sensorDireita = a;
  sensorEsquerda = b;
  sensorFrente = c;

  Serial.print("Sensor Direita = ");
  Serial.print(sensorDireita);

  Serial.print(" - Sensor Esquerda = ");
  Serial.print(sensorEsquerda);

  Serial.print(" - Sensor Frente = ");
  Serial.print(sensorFrente);
  Serial.println();
}

void controlePosicao()
{

  if (sensorEsquerda  < intervalo)
  { // esquerda
    modo = 2;
  }
  else if (sensorFrente  < intervalo)
  { // frente
    modo = 3;
  }
  else if (sensorDireita  < intervalo)
  { // direita
    modo = 1;
  }
}

void moveMotor()
{
  switch (modo)
  {
    
    case 0:
      return;
    break;

    case 1:
      velocidadeMotorDireita  = 0;
      velocidadeMotorEsquerda =  110;

      digitalWrite(pinMotorDireitaA, LOW);
      digitalWrite(pinMotorDireitaB, LOW);
      digitalWrite(pinMotorEsquerdaA, HIGH);
      digitalWrite(pinMotorEsquerdaB,LOW );
      analogWrite(pinVelMotorDireita, velocidadeMotorDireita);
      analogWrite(pinVelMotorEsquerda, velocidadeMotorEsquerda);

      return;
    break;

    case 2:
      velocidadeMotorDireita  = 110;
      velocidadeMotorEsquerda =  0;

      digitalWrite(pinMotorDireitaA, HIGH);
      digitalWrite(pinMotorDireitaB, LOW);
      digitalWrite(pinMotorEsquerdaA, LOW);
      digitalWrite(pinMotorEsquerdaB,LOW );
      analogWrite(pinVelMotorDireita, velocidadeMotorDireita);
      analogWrite(pinVelMotorEsquerda, velocidadeMotorEsquerda);

      return;
    break;

    case 3:
      velocidadeMotorDireita  = 100;
      velocidadeMotorEsquerda = 100;

      digitalWrite(pinMotorDireitaA, HIGH);
      digitalWrite(pinMotorDireitaB, LOW);
      digitalWrite(pinMotorEsquerdaA, HIGH);
      digitalWrite(pinMotorEsquerdaB,LOW );
      analogWrite(pinVelMotorDireita, velocidadeMotorDireita);
      analogWrite(pinVelMotorEsquerda, velocidadeMotorEsquerda);
      
      return;
    break;
  }
  return;
}