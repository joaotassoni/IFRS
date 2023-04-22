/*
Sistemas Microcontrolados II - 2018

Projeto I - Secador de Frutas com Arduino - Código Arduino MEGA

Alunos: João Pedro Tassoni e Filipy Dias
Turma: Eletrônica 4
*/

/*BIBLIOTECAS*/

#include<LiquidCrystal.h>
#include<Wire.h>
#include<DS3231.h> 

/*INICIALIZAÇÕES*/

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DS3231 rtc;
RTCDateTime dataehora;

/*DEFINIÇÕES*/ //Facilitam a compreensão do código

// números correspondentes aos botões do Keypad
#define btnRIGHT   0
#define btnUP      1
#define btnDOWN    2
#define btnLEFT    3
#define btnSELECT  4
#define btnNONE    5

// número correspondente a cada fruta
#define KIWI     0
#define ABACAXI  1
#define MACA     2
#define AMEIXA   3
#define TOMATE   4
#define BANANA   5

// tempo para desidratar cada fruta

String tKiwi =     "02:15";
String tAbacaxi =  "02:45";
String tMaca =     "02:15";
String tAmeixa =   "02:30";
String tTomate =   "03:00";
String tBanana =   "02:10";

/*PINOS*/

byte shieldKeypad = 0;
byte sensorTemp =   8;
byte resistencia = 22;
byte buzzer =      23;

/*VARIÁVEIS*/

int lastButton = btnNONE;
int button =     btnNONE;
int pressed =    btnNONE;

byte select = 0; // variável utilizada para controle das maquinas de estado envolvidas na exibição dos menus
byte estado = 0; // variável que guarda o estado da resistência (LOW = LIGADA e HIGH = DESLIGADA)

float tempInt = 0; // temperatura dentro do isopor
float tempExt = 0; // temperatura do lado de fora da caixa
float tempFruta = 107.0; // temperatura para desidratar frutas

int menu1 = 4; // controle de Menu (Linha 1 do LCD)
int menu2 = 1; // controle de Menu (Linha 2 do LCD)
int menu3 = 0; // controle de Menu (Seleção de fruta)
int menu4 = 0; // controle de Menu (Setagem do RTC)

byte fruta = 0; //fruta Selecionada

String data = "ww/ww/wwww"; // string que posteriormente receberá a data pelo RTC
byte diaAcaba = 0; // variável que recebe quantos dias faltam para o processo acabar
String hora = "xx:xx:xx"; // string que posteriormente receberá a hora pelo RTC
String horaAcaba = "yy:yy:yy"; // string que recebe o horário atual + tempo de preparo da fruta
String horaFalta = "00:00:00"; // string que calcula o tempo restante de secagem para a fruta
int dadosRTC[5] = {0, 0, 0, 0, 0}; // vetor de inteiros que recebe os dados de setagem do RTC quando o usuário fizer a mesma
boolean flag = 0; // variável usada para acionar uma condição na função desidrata2()

boolean iniciar = 0; // variável que guarda 0 quando não há nenhum processo em andamento e 1 quando alguma fruta está sendo desidratada
unsigned long lastmillis = millis(); // variável para controle de tempo
int contador = 0; // variável que é incrementada a cada 500ms, e vai servir para guardar dados na EEPROM

void setup()
{
  Serial.begin(115200); // inicialização da Serial
  Serial2.begin(115200); // inicialização da Serial

  lcd.begin(16, 2); // inicialização do Shield LCD 16x2

  Wire.begin();
  rtc.begin();
  dataehora = rtc.getDateTime(); // inicializa o Relógio em Tempo-Real

  pinMode(resistencia, OUTPUT); // define o pino da resistência como saída
  pinMode(buzzer, OUTPUT); // define o pino do buzzer (alarme) como saída
  digitalWrite(resistencia, HIGH);
  sensores();

  Serial.println("começou");
  Serial.println();
}

void loop()
{
  if (millis() - lastmillis >= 500)
  { // a cada 0,5 segundos:
    sensores(); // faz a chamada da função sensores (que faz a leitura do LM35 e o RTC)
    if (lastButton != btnSELECT)
      lastButton = btnNONE; // "reseta" a função de leitura do keypad (habilita uma nova leitura, apesar da tecla pressionada não ter sido solta)
    lastmillis = millis(); // reinicia o contador
  }
  menu(); // chama a função menu

  if (iniciar == 0)
  { // caso nenhum processo esteja em andamento:
    digitalWrite (resistencia, HIGH); // desliga o pino da resistência
    flag = 0;
  }

  if (iniciar == 1) // caso algum processo esteja em andamento:
    calor(); // chama a função calor (que controla a resistência de acordo com a temperatura)

  if (Serial2.available() > 0)
    nodeMCUrecebe();
}

void sensores()
{ // função que faz as leituras dos sensores de temperatura intreno e externo
  analogRead(sensorTemp);
  delay(1);
  tempInt = ((analogRead(sensorTemp) * (5.0 / 1023.0)) / 0.01);

  dataehora = rtc.getDateTime();

  String dia = "  ";
  String mes = "  ";
  String ano = "    ";

  dia = dataehora.day;
  mes = dataehora.month;
  ano = dataehora.year;

  if (dia.toInt() < 10)
    dia = '0' + dia;

  if (mes.toInt() < 10)
    mes = '0' + mes;

  data = dia + '/' + mes + '/' + ano;

  String horas = "  ";
  String minutos = "  ";
  String segundos = "  ";

  horas = dataehora.hour;
  minutos = dataehora.minute;
  segundos = dataehora.second;

  if (horas.toInt() < 10)
    horas = '0' + horas;

  if (minutos.toInt() < 10)
    minutos = '0' + minutos;

  if (segundos.toInt() < 10)
    segundos = '0' + segundos;

  hora = horas + ':' + minutos + ':' + segundos;

  rtc.forceConversion();
  tempExt = rtc.readTemperature();
}

void menu()
{ // função que cuida das maquinas de estados e exibe informações no display LCD
  int key = keys(); // chama a leitura do keypad

  if (select == 0)
  {
    switch (menu1)
    { // maquina de estados que irá controlar a informação a ser exibida na linha 1 do LCD, que aparecerá quando a tecla SELECT do Shield for pressionada
      case 0:
      break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Temp in: ");
        lcd.print(tempInt);
        lcd.print("C    ");
      break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Temp ex: ");
        lcd.print(tempExt);
        lcd.print("C   ");
      break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Dia: ");
        lcd.print(data);
        lcd.print("  ");
      break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Hora: ");
        lcd.print(hora);
        lcd.print("  ");
      break;

      case 5:
        if (iniciar == 0)
        {
          select = 1;
          lcd.clear();
        } 
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("TEM CERTEZA?    ");
          menu2 = 9;
        }
        menu1 = 8;
      break;

      case 6:
        if (iniciar == 0)
        {
          String a = "    "; a[0] = '0'; a[1] = '0'; a[2] = data[0]; a[3] = data[1];
          int b = a.toInt(); a[0] = '0'; a[1] = '0'; a[2] = data[3]; a[3] = data[4];
          int c = a.toInt(); a[0] = data[7]; a[1] = data[8]; a[2] = data[9]; a[3] = data[10];
          int d = a.toInt(); a[0] = '0'; a[1] = '0'; a[2] = hora[0]; a[3] = hora[1];
          int e = a.toInt(); a[0] = '0'; a[1] = '0'; a[2] = hora[3]; a[3] = hora[4];
          int f = a.toInt();
          dadosRTC[0] = b; dadosRTC[1] = c; dadosRTC[2] = d; dadosRTC[3] = e; dadosRTC[4] = f;

          select = 2;
          lcd.clear();
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("Processo em");
          lcd.setCursor(0, 1);
          lcd.print("andamento!");
          tone(buzzer, 1500);
          delay(500);
          noTone(buzzer);
          delay(1500);
          menu1 = 4;
        }

      break;

      case 7:
        if (iniciar == 1)
        {
          lcd.setCursor(0, 0);
          lcd.print("Falta ");
          lcd.setCursor(6, 0);
          lcd.print(horaFalta);
          lcd.setCursor(0, 1);
          lcd.print("<- VOLTAR");
          menu2 = 11;
        }
        else if (iniciar == 0)
        {
          lcd.setCursor(0, 0);
          lcd.print("Inic. ");
          if (fruta == KIWI) lcd.print("KIWI");
          if (fruta == ABACAXI) lcd.print("ABACAXI");
          if (fruta == MACA) lcd.print("MACA");
          if (fruta == AMEIXA) lcd.print("AMEIXA");
          if (fruta == TOMATE) lcd.print("TOMATE");
          if (fruta == BANANA) lcd.print("BANANA");

          lcd.setCursor(0, 1);
          lcd.print(".");
          tone(buzzer, 1500);
          delay(500);
          noTone(buzzer);
          delay(500);
          lcd.print(".");
          tone(buzzer, 1500);
          delay(500);
          noTone(buzzer);
          delay(500);
          lcd.print(".");
          tone(buzzer, 1500);
          delay(500);
          noTone(buzzer);
          desidrata1();
          desidrata2();

          lcd.setCursor(0, 0);
          lcd.print("Acaba as        ");
          lcd.setCursor(9, 0);
          lcd.print(horaAcaba);
          lcd.setCursor(0, 1);
          lcd.print("<- VOLTAR");
          iniciar = 1;
          menu1 = 12; menu2 = 11;
        }
      break;

      case 8:
      break;

      case 9:
        menu1 = 4;
        menu2 = 1;
        menu3 = 0;
      break;

      case 10:
        iniciar = 0;
        menu1 = 4;
        menu2 = 1;
        menu3 = 0;
        lcd.setCursor(0, 0);
        lcd.print("Proc. cancelado");
        lcd.setCursor(0, 1);
        lcd.print(".");
        tone(buzzer, 1500);
        delay(500);
        noTone(buzzer);
        delay(500);
        lcd.print(".");
        tone(buzzer, 1500);
        delay(500);
        noTone(buzzer);
        delay(500);
        lcd.print(".");
        tone(buzzer, 1500);
        delay(500);
        noTone(buzzer);
        desidrata1();
        desidrata2();
      break;

      case 11:
        menu1 = 4;
        menu2 = 1;
        menu3 = 0;
        break;

      case 12:
      break;

    }

    switch (menu2)
    { // maquina de estados que irá controlar a linha 2 do LCD, e irá definir o que o usuário quer que apareça na linha 1 ao pressionar o botão SELECT
      case 0:
        break;

      case 1:
        lcd.setCursor(0, 1);
        lcd.print("Exibir temp in >");
      break;

      case 2:
        lcd.setCursor(0, 1);
        lcd.print("Exibir temp ex<>");
      break;

      case 3:
        lcd.setCursor(0, 1);
        lcd.print("Exibir data   <>");
      break;

      case 4:
        lcd.setCursor(0, 1);
        lcd.print("Exibir hora   <>");
      break;

      case 5:
        lcd.setCursor(0, 1);
        if (iniciar == 0)
          lcd.print("Selec. fruta  <>");
        else
          lcd.print("Cancelar proc.<>");
      break;

      case 6:
        lcd.setCursor(0, 1);
        lcd.print("Ajustar RTC   <>");
      break;

      case 7:
        lcd.setCursor(0, 1);
        if (iniciar == 0)
          lcd.print("Iniciar        <");
        else
          lcd.print("Andam. proc.   <");
      break;

      case 8:
        break;

      case 9:
        lcd.setCursor(0, 1);
        lcd.print("NAO            >");
      break;

      case 10:
        lcd.setCursor(0, 1);
        lcd.print("SIM            <");
      break;

      case 11:
      break;
    }

    if (key == btnRIGHT)
      menu2++;

    if (key == btnLEFT)
      menu2--;

    if (key == btnSELECT)
    {
      menu1 = menu2;
      lcd.clear();
    }

    if (menu1 != 8)
    {
      if (menu2 == 0)
        menu2 = 1;

      if (menu2 == 8)
        menu2 = 7;
    }
    else
    {
      if (menu2 == 8)
        menu2 = 9;

      if (menu2 == 11)
        menu2 = 10;
    }
  }

  if (select == 1)
  {
    switch (menu3)
    {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("KIWI            ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tKiwi); lcd.print("     ");
      break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("ABACAXI         ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tAbacaxi); lcd.print("     ");
      break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("MACA            ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tMaca); lcd.print("     ");
      break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("AMEIXA          ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tAmeixa); lcd.print("     ");
      break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("TOMATE          ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tTomate); lcd.print("     ");
      break;

      case 5:
        lcd.setCursor(0, 0);
        lcd.print("BANANA          ");
        lcd.setCursor(0, 1);
        lcd.print("tempo: ");
        lcd.print(tBanana); lcd.print("     ");
      break;

      case 6:
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("<- VOLTAR       ");
      break;
    }

    if (key == btnRIGHT)
      menu3++;

    if (key == btnLEFT)
      menu3--;

    if (key == btnSELECT)
    {
      if (menu3 != 6)
        fruta = menu3;
      menu1 = 1;
      menu2 = 1;
      lcd.clear();
      select = 0;
    }

    if (menu3 < 0)
      menu3 = 6;

    if (menu3 > 6)
      menu3 = 0;
  }

  if (select == 2)
  {
    switch (menu4)
    {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Selecione o dia:");

        lcd.setCursor(0, 1);
        if (dadosRTC[menu4] < 10)
          lcd.print('0');

        lcd.print(dadosRTC[menu4]);
        lcd.print("            <>");
      break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Selecione o mes:");

        lcd.setCursor(0, 1);
        if (dadosRTC[menu4] < 10)
          lcd.print('0');

        lcd.print(dadosRTC[menu4]);
        lcd.print("            <>");
      break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Selecione o ano:");

        lcd.setCursor(0, 1);
        if (dadosRTC[menu4] < 10)
          lcd.print('0');

        lcd.print(dadosRTC[menu4]);
        lcd.print("          <>");
      break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Selecione a hora:");

        lcd.setCursor(0, 1);
        if (dadosRTC[menu4] < 10)
          lcd.print('0');

        lcd.print(dadosRTC[menu4]);
        lcd.print("            <>");
      break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Selecione os    ");

        lcd.setCursor(0, 1);
        lcd.print("minutos: ");
        if (dadosRTC[menu4] < 10)
          lcd.print('0');

        lcd.print(dadosRTC[menu4]);
        lcd.print("   <>");
      break;

      case 5:
        lcd.setCursor(0, 0);
        if (dadosRTC[0] < 10)
          lcd.print('0');
        lcd.print(dadosRTC[0]);
        lcd.print("/");
        if (dadosRTC[1] < 10)
          lcd.print('0');
        lcd.print(dadosRTC[1]);
        lcd.print("/");
        lcd.print(dadosRTC[2]);
        lcd.print(" ");
        if (dadosRTC[3] < 10)
          lcd.print('0');
        lcd.print(dadosRTC[3]);
        lcd.print(":");
        if (dadosRTC[4] < 10)
          lcd.print('0');
        lcd.print(dadosRTC[4]);
        lcd.setCursor(0, 1);
        lcd.print("Continuar?       ");
      break;

      case 6:
        rtc.setDateTime(dadosRTC[2], dadosRTC[1], dadosRTC[0], dadosRTC[3], dadosRTC[4], 0);
        select = 0;
        menu1 = 4;
        menu2 = 1;
        menu4 = 0;
      break;
    }

    if (key == btnRIGHT)
      dadosRTC[menu4]++;

    if (key == btnLEFT)
      dadosRTC[menu4]--;

    if (key == btnSELECT)
    {
      menu4++;
      delay(20);
    }

    if (dadosRTC[0] < 1 )
      dadosRTC[0] = 31;
    if (dadosRTC[0] > 31)
      dadosRTC[0] = 1;

    if (dadosRTC[1] < 1 )
      dadosRTC[1] = 12;
    if (dadosRTC[1] > 12)
      dadosRTC[1] = 1;

    if (dadosRTC[2] < 2000 )
      dadosRTC[2] = 2100;
    if (dadosRTC[2] > 2100)
      dadosRTC[2] = 2000;

    if (dadosRTC[3] < 0 )
      dadosRTC[3] = 23;
    if (dadosRTC[3] > 23)
      dadosRTC[3] = 0;

    if (dadosRTC[4] < 0 )
      dadosRTC[4] = 59;
    if (dadosRTC[4] > 59)
      dadosRTC[4] = 0;
  }
}

int keys() 
{ // função que cuida da leitura dos botões do shield do LCD
  int abtn = analogRead(0); // le a entrada dos botoes
  
  // converte para o codigo de botao apropriado
  if (abtn > 1000) button = btnNONE;
  if (abtn < 50) button = btnRIGHT;
  if ((abtn >= 50) && (abtn < 195)) button = btnUP;
  if ((abtn >= 195) && (abtn < 380)) button = btnDOWN;
  if ((abtn >= 380) && (abtn < 555)) button = btnLEFT;
  if ((abtn >= 555) && (abtn < 790)) button = btnSELECT;

  // verifica se o estado dos botões foi trocado
  if (button != lastButton)
  {
    pressed = button;
    lastButton = button;
  } 
  else
  {
    pressed = btnNONE;
  }
  return pressed;
}

void desidrata1()
{ // função que calcula o horário em que o processo acaba
  String tFruta = "    ";
  horaAcaba = hora;
  sensores();

  switch (fruta)
  {
    case KIWI:
      tFruta = tKiwi;
    break;

    case ABACAXI:
      tFruta = tAbacaxi;
    break;

    case MACA:
      tFruta = tMaca;
    break;

    case AMEIXA:
      tFruta = tAmeixa;
    break;

    case TOMATE:
      tFruta = tTomate;
    break;

    case BANANA:
      tFruta = tBanana;
    break;
  }

  String a = "  ";
  String b = "  ";
  String c = "  ";
  String d = "  ";

  a = tFruta[0]; a = a + tFruta[1];
  b = tFruta[3]; b = b + tFruta[4];
  c = hora[0]; c = c + hora[1];
  d = hora[3]; d = d + hora[4];

  int e = a.toInt() + c.toInt();
  int f = b.toInt() + d.toInt();

  for (; f > 59; f = f - 60)
  {
    e++;
  }

  if (e > 23)
  {
    e = e - 24;
    diaAcaba = 1;
  }

  horaAcaba = e;
  horaAcaba = horaAcaba + ':';
  horaAcaba = horaAcaba + f;

  if (f < 10)
  {
    horaAcaba = e;
    horaAcaba = horaAcaba + ':';
    horaAcaba = horaAcaba + '0';
    horaAcaba = horaAcaba + f;
  }

  if (e < 10)
    horaAcaba = '0' + horaAcaba;
}

void desidrata2()
{  // função que calcula quanto tempo falta para o processo acabar
  if (flag == 0) {
    sensores();
    flag = 1;
  }

  String a = "  ";
  String b = "  ";
  String c = "  ";
  String d = "  ";
  String e = "  ";
  String f = "  ";

  a = horaAcaba[0]; a = a + horaAcaba[1];
  b = horaAcaba[3]; b = b + horaAcaba[4];
  c = "00";
  d = hora[0]; d = d + hora[1];
  e = hora[3]; e = e + hora[4];
  f = hora[6]; f = f + hora[7];

  int h = a.toInt() - d.toInt();
  int i = b.toInt() - e.toInt();
  int j = c.toInt() - f.toInt();

  if (j < 0)
  {
    j = j + 60;
    i--;
  }

  if (i < 0)
  {
    h--;
    i = i + 60;
  }

  if (diaAcaba == 1)
  {
    h = h + 24;
    diaAcaba = 0;
  }

  if (j < 10)
  {
    horaFalta = j;
    horaFalta  = '0' + horaFalta ;
    horaFalta = ':' + horaFalta ;
  } 
  else
  {
    horaFalta = j;
    horaFalta = ':' + horaFalta;
  }

  if (i < 10)
  {
    horaFalta = i + horaFalta;
    horaFalta = '0' + horaFalta;
    horaFalta = ':' + horaFalta;
  }
  else
  {
    horaFalta = i + horaFalta;
    horaFalta = ':' + horaFalta;
  }

  if (h < 10)
  {
    if(h < 0)
      h = h + 24;
    
    horaFalta = h + horaFalta;
    horaFalta = '0' + horaFalta;
  }
  else
  {
    horaFalta = h + horaFalta;
  }
  Serial.println(horaFalta);
}

void calor()
{ // função que irá simplesmente ligar ou desligar a resistência de aquecimento
  if (tempInt - 5 < tempFruta)
  {
    estado = LOW;
  }
  else
  {
    estado = HIGH;
  }
  
  digitalWrite(resistencia, estado);
  
  desidrata2();

  if ((diaAcaba == 0) && (horaFalta[0] == '0') && (horaFalta[1] == '0') && (horaFalta[3] == '0') && (horaFalta[4] == '0') && (horaFalta[6] == '0') && (horaFalta[7] == '0'))
    alarme1(); // se a hora coincidir, chama a função alarme
}

void alarme1()
{ // função que controla o alarme de fins de processo
  iniciar = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Frutas prontas!");
  lcd.setCursor(0, 1);

  tone(buzzer, 1500);
  delay(500);

  noTone(buzzer);
  delay(500);

  lcd.print(".");
  delay(500);

  tone(buzzer, 1500);
  delay(500);

  lcd.print(".");

  noTone(buzzer);
  delay(1000);

  lcd.print(".");

  tone(buzzer, 1500);
  delay(1000);
  noTone(buzzer);//Liga e desliga o buzzer para emitir um som de frequência de 1500Hz 3 vezes

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Retire as frutas");
  lcd.setCursor(0, 1);
  lcd.print("<- VOLTAR");
  menu1 = 12; menu2 = 11;
}

void alarme2()
{ // função que controla o alarme para comandos recebidos do celular
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Comando Aplicat.");
  tone(buzzer, 1500);
  delay(500);
  noTone(buzzer);
  lcd.setCursor(0, 1);
}

void nodeMCUrecebe()
{ // função que trata os dados recebidos do ESP8266
  String recebe = "          ";
  recebe = Serial2.readString;
  String envia  = "          ";
    
  if ((recebe[0] == '0')&&(recebe[1] == '1'))
  {
    menu1 = 7;
    
    alarme2();
    lcd.print("INICIAR!");
    delay(1000);
    lcd.clear();
  }
  else if ((recebe[0] == '0')&&(recebe[1] == '2'))
  {
    lcd.clear();
    menu1 = 10;
    
    alarme2();
    lcd.print("PARAR!");
    delay(1000);
    lcd.clear();
  }

  if (iniciar == 0)
  {
    if ((recebe[0] == '0')&&(recebe[1] == '3'))
    {
      fruta = KIWI;
      
      alarme2();
      lcd.print("Fruta = KIWI");
      delay(1000);
      lcd.clear();
    }
    else if ((recebe[0] == '0')&&(recebe[1] == '4'))
    {
      fruta = ABACAXI;
      
      alarme2();
      lcd.print("Fruta = ABACAXI");
      delay(1000);
      lcd.clear();
    }
    else if ((recebe[0] == '0')&&(recebe[1] == '5'))
    {
      fruta = MACA;
      
      alarme2();
      lcd.print("Fruta = MACA");
      delay(1000);
      lcd.clear();
    }
    else if ((recebe[0] == '0')&&(recebe[1] == '6'))
    {
      fruta = AMEIXA;
      
      alarme2();
      lcd.print("Fruta = AMEIXA");
      delay(1000);
      lcd.clear();
    }
    else if ((recebe[0] == '0')&&(recebe[1] == '7'))
    {
      fruta = TOMATE;
      
      alarme2();
      lcd.print("Fruta = TOMATE");
      delay(1000);
      lcd.clear();
    }
    else if ((recebe[0] == '0')&&(recebe[1] == '8'))
    {
      fruta = BANANA;
      
      alarme2();
      lcd.print("Fruta = BANANA");
      delay(1000);
      lcd.clear();
    }
  }
  
  if ((recebe[0] == '0')&&(recebe[1] == '9'))
  {
    envia = "0";
    envia = envia + horaFalta;
    Serial.println("hora");
    
  }
  else if ((recebe[0] == '1')&&(recebe[1] == '0'))
  {
    envia = "1";
    envia = envia + tempInt;
    Serial.println("Interpretou 10");
  }
  else if ((recebe[0] == '1')&&(recebe[1] == '1'))
  {
    envia = "2";
    envia = envia + tempExt;
  }
  else if ((recebe[0] == '1')&&(recebe[1] == '2'))
  {
    envia = "3 ";
    envia = envia + iniciar;
  }
  else {}
  
  Serial.println(envia);
  nodeMCUenvia(envia);
}

void nodeMCUenvia(String mensagem)
{ // função que envia a mensagem desejada ao ESP8266
  mensagem += ';';
  Serial2.print(mensagem);
}
