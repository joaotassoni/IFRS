/*
Sistemas Microcontrolados I - 2017

Projeto - Economia de Energia com Arduino

Alunos: João Pedro Tassoni, Filipy Dias e Leonardo Garibaldi
Turma: Eletrônica 3
*/


int pinorele = 2;
int pinopir = 7;
int acionamento;

void setup()
{
    pinMode(pinorele, OUTPUT);
    pinMode(pinopir, INPUT);
    Serial.begin(9600);
}

void loop()
{
    acionamento = digitalRead(pinopir);
    if (acionamento == LOW);
    {
        digitalWrite(pinorele, LOW);
        Serial.println("Parado");
    }
    else
    {
        digitalWrite(pinorele, HIGH);
        Serial.println("Movimento !!!");
    }
}