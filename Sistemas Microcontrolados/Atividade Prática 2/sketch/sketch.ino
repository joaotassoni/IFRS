/*
Sistemas Microcontrolados I - 2017

Aluno: João Pedro Tassoni
Turma: Eletrônica 3
*/

// variáveis para designar as saídas da placa
const int LED = 11, BUT = 12;

// variáveis para controle do botão
boolean stt = 0, oldstt = 0;

// string onde se insere o texto para tradução
String frase = "SOS";

// struct com tabela de conversão de letras em código morse
static const struct {
  const char letra, *morse;
} Morse[] =
{
  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "-.-." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', ".-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },
  { ' ', "    " },
  { '.', ".-.-.-" },
  { ',', "--..--" },
  { '?', "..--.." },
  { '!', "-.-.--" },
  { ':', "---..." },
  { ';', "-.-.-." },
  { '(', "-.--." },
  { ')', "-.--.-" },
};

void setup()
{
  // define os pinos do led e do botão como entrada e saída, respectivamente
  pinMode(LED,OUTPUT);
  pinMode(BUT,INPUT); 
  
  // primeira leitura do botão
  oldstt = digitalRead(BUT);
}

void loop()
{
  
  // faz a leitura do botão e compara com a leitura antiga
  stt = digitalRead(BUT);
  if (oldstt != stt)
  {
    if (stt == HIGH)
    {
      // se o botão for pressionado, inicia a conversão para o código morse
      toMorse(frase, frase.length());
    }
    oldstt = stt;
  }
}

void toMorse(String msg, int msgSize)
{
  // variáveis de contador
  int cont1, cont2;
  
  // string que recebe toda a frase em morse
  String morsePhrase = "";
  
  // loop com o tamanho da mensagem
  for (cont1 = 0; cont1 <= msgSize; cont1++)
  {  
    // loop for com valor máximo o número de letras convertidas em morse na struct "Morse" 
    for (cont2 = 0; cont2 < 45; cont2++)
    {
      // compara a letra da mensagem com as letras da struct "Morse", e toupper transforma letras minúsculas em maiúsculas para a comparação
      if (toupper(msg[cont1]) == Morse[cont2].letra )
      {
        // adiciona à variável "morsePhrase" o que ela já possui mais o código correspondente à última letra comparada
        morsePhrase = morsePhrase + Morse[cont2].morse;
        break;
      }
    }
    
    // adiciona um espaço dentro da variável "morsePhrase" entre as letras convertidas para morse
    morsePhrase = morsePhrase + " ";
  }

  // reaproveita a variável "cont1" para um novo for, com valor máximo igual ao número de caracteres na string "morsePhrase"
  for (cont1=0;cont1<= morsePhrase.length();cont1++)
  {
    // determina o estado do led a partir de uma localização na string "morsePhrase"
    switch(morsePhrase[cont1])
    {
      // se for um ponto:
      case '.':
      // led fica aceso e apagado por 0,5 segundos
        digitalWrite(LED,HIGH);
        delay(500);
        digitalWrite(LED,LOW);
        delay(500);
      break;
      
      // se for um traço:
      case '-':
        // led fica aceso e apagado por 1,5 segundos  
        digitalWrite(LED,HIGH);
        delay(1500);
        digitalWrite(LED,LOW);
        delay(1500);
      break;
	  
      // se for um espaço:
      case ' ':
        // led fica apagado por 0,5 segundos para designar o fim de uma letra
        //OBS: um espaço entre palavras é convertido em 4 espaços, que resulta em uma pausa de 2 segundos com o led apagado entre palavras
	      delay(500);
      break;
    }
  }
}
