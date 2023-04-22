/*
Sistemas Microcontrolados I - 2017

Aluno: João Pedro Tassoni
Turma: Eletrônica 3
*/

#include<LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // inicializa o shield LCD

byte coluna[3] = {23,25,27}; // cria um vetos com as posições de alimentação das colunas do KEYPAD
byte linha[4] = {29,31,33,35}; //cria um vetos com as posições de leitura das linhas do KEYPAD
char comparador = ' ';
char ultimoLido = ' ';

void setup()
{
    lcd.begin(16,2);
    lcd.clear();

    Serial.begin(115200);

    pinMode(coluna[0],OUTPUT);pinMode(coluna[1],OUTPUT);pinMode(coluna[2],OUTPUT); // declara todas as posições de colunas como saída
    pinMode(linha[0],INPUT_PULLUP);pinMode(linha[1],INPUT_PULLUP);pinMode(linha[2],INPUT_PULLUP);pinMode(linha[3],INPUT_PULLUP); // declara todas as posições de linhas como como entradas com proteção PULLUP
}

void loop()
{
    ultimoLido = readKeypad(); // variável recebe o valor da leitura do teclado

    if(ultimoLido != 'N')
    {
        if(ultimoLido != 'M')
        {
        lcd.setCursor(0,0);
        lcd.print(ultimoLido); // exibe o último valor retornado, caso ele seja um novo valor e não seja uma leitura vazia ('N')
        }
    }
}

char readKeypad()
{
    //delay(200); // para melhorar a visualização do funcionamento do programa via monitor serial, um delay pode ser implementado aqui (mas não é utilizado no programa final)
    char retorno = 'N'; // variável que será retornada ao fim da execução da função, inicializada com o valor 'N', que representa que nenhuma tecla foi pressionada

    digitalWrite(coluna[0],LOW);digitalWrite(coluna[1],HIGH);digitalWrite(coluna[2],HIGH); // "LIGA" a coluna 1 e "DESLIGA" as outras

    if(digitalRead(linha[0]) == LOW)
        retorno = '1';
    if(digitalRead(linha[1]) == LOW)
        retorno = '4';
    if(digitalRead(linha[2]) == LOW)
        retorno = '7';
    if(digitalRead(linha[3]) == LOW)
        retorno = '*';
    // lê todas as linhas e retorna o valor correspondente à tecla pressionada

    digitalWrite(coluna[0],HIGH);digitalWrite(coluna[1],LOW);digitalWrite(coluna[2],HIGH); // "LIGA" a coluna 2 e "DESLIGA" as outras

    if(digitalRead(linha[0]) == LOW)
        retorno = '2';
    if(digitalRead(linha[1]) == LOW)
        retorno = '5';
    if(digitalRead(linha[2]) == LOW)
        retorno = '8';
    if(digitalRead(linha[3]) == LOW)
        retorno = '0';
    // lê todas as linhas e retorna o valor correspondente à tecla pressionada

    digitalWrite(coluna[0],HIGH);digitalWrite(coluna[1],HIGH);digitalWrite(coluna[2],LOW); // "LIGA" a coluna 3 e "DESLIGA" as outras

    if(digitalRead(linha[0]) == LOW)
        retorno = '3';
    if(digitalRead(linha[1]) == LOW)
        retorno = '6';
    if(digitalRead(linha[2]) == LOW)
        retorno = '9';
    if(digitalRead(linha[3]) == LOW)
        retorno = '#';
    // lê todas as linhas e retorna o valor correspondente à tecla pressionada


    if(retorno != comparador)
    {
        // retorna a tecla que foi pressionada, se ela for diferente da última tecla
        Serial.println(retorno);
        comparador = retorno;
        return(retorno);
    }

    if(retorno == 'N')
    {
      // retorna que nenhuma tecla foi pressionada
      Serial.println("NENHUMA TECLA PRESSIONADA");
      return(retorno);
    }
    
    retorno = 'M';
    // retrona que alguma tecla se manteve pressionada (leitura igual a anterior e não-nula)
    Serial.println("ULTIMA TECLA SE MANTEVE PRESSIONADA");
    return(retorno);
}
