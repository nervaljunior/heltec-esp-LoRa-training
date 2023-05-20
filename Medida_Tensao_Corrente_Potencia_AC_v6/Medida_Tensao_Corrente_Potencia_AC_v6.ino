

/*    Seleção do MODO de operação:
      0 - Transmite os valores brutos dos ADCs
      1 - Transmite os valores instantâneos de tensão e corrente, em volts e ampères
      2 - Transmite os valores RMS de tensão e corrente, em volts e ampères
      3 - Transmite os valores RMS de tensão e corrente, em volts e ampères para Serial FK
*/

// Materiais utilizados:
// Módulo ESP32 LoRa
// Resistores 10k1% e 82k ohm
// Capacitores de 0,22nF, 4n7, 47nF
// Capacitor de 1000uF/25V
// LED
// Sensor de tensão ZMPT101B
// Sensor de corrente ASC7112
// Multímetro
// Osciloscópio

#include <heltec.h>
#include <Wire.h>

#define MODO 0
/* Protótipo da função */
//Pinos de entrada utilizados
const uint8_t pinAdcTensao = 34; // GPIO34 para sensor de tensão
const uint8_t pinAdcCorrente = 35; // GPIO35 para sensor de corrente

//Caracter separador dos dados
const char separador = '\t';

// Frequência e canal utilizado pelo módulo LoRa
#define LORA_BAND 915E6
#define BAND_915MHZ 1

//valores de offset de cada sensor (determinados estatisticamente através de capturas)
const float offsetVAC = 1383.328;
const float offsetIAC = 1395.393991;
const int RelePin = 23;         // pino ao qual o Módulo Relé está conectado
String statusRele = "OFF"; // variavel para ler dados recebidos pela serial
String off = "OFF";


void setup()
{
  Serial.begin(115200); //Iniciando a comunicação serial
  while (!Serial);
  // Iniciando comunicação com o módulo LoRa
  if (!LoRa.begin(LORA_BAND, BAND_915MHZ)) {
  Serial.println("Erro ao iniciar o módulo LoRa");
  while (true);
}

}
void loop() {
  Serial.print("Mensagem de teste\n");
  #if MODO == 0
  //Envio direto dos códigos do ADC sem nenhum tratamento

  //Variáveis
  int32_t tensao = 0;
  int32_t corrente = 0;

  //Captura
  tensao = analogRead(pinAdcTensao);
  corrente = analogRead(pinAdcCorrente);

  //Transmissão
  LoRa.beginPacket(); //Inicia o pacote de transmissão
  LoRa.print(tensao); //Adiciona os dados ao pacote
  LoRa.print(separador);
  LoRa.print(corrente);
  LoRa.endPacket(); //Encerra o pacote e envia os dados

  delay(1000);
  #endif

  #if MODO == 1
  //Envio dos códigos convertidos em tensão e corrente instatâneos

  //coeficientes das regressões lineares. Usados para a conversão dos códigos
  const float coefA_VAC = 0.2546;
  const float coefB_VAC = 0.3807;
  const float coefA_IAC = 0.117;
  const float coefB_IAC = 0.1299;

  //Variáveis
  int32_t tensao = 0;
  int32_t corrente = 0;

  //Captura
  tensao = analogRead(pinAdcTensao);
  corrente = analogRead(pinAdcCorrente);

  //Transmissão
  //Serial.print(micros());
  //Serial.print(separador);
  LoRa.beginPacket();
  LoRa.print((float(tensao) - offsetVAC)*coefA_VAC + coefB_VAC);
  LoRa.print(separador);
  LoRa.print((float(corrente) - offsetIAC)*coefA_IAC + coefB_IAC);
  LoRa.endPacket(); //Encerra o pacote e envia os dados
  delay(1000);

  #endif


  #if MODO == 2
    //Envio dos códigos convertidos da tensão e corrente RMS

    //coeficientes das regressões lineares. Usados para a conversão dos códigos
    const float coefA_VAC = 0.2652;
    const float coefB_VAC = 0.0;
    const float coefA_IAC = 0.1191;
    const float coefB_IAC = 0.0;

    //Período mínimo de captura de 16,7ms para garantir que ao menos um ciclo seja capturado
    const int32_t duracaoDaAmostragem = (int32_t)((1000.0 / 60.0) * 6.0); //6 ciclos de 1000ms/60 (60Hz)

    //Variáveis
    int32_t tensao = 0;
    int32_t corrente = 0;
    uint32_t qtdDeAmostras = 0;
    float acumuladorTensao = 0.0;
    float acumuladorCorrente = 0.0;

    //Captura
    unsigned long inicio = millis();
    do
    {
      tensao = analogRead(pinAdcTensao) - offsetVAC;
      corrente = analogRead(pinAdcCorrente) - offsetIAC;

      acumuladorTensao = acumuladorTensao + (tensao * tensao); //Soma dos quadrados
      acumuladorCorrente = acumuladorCorrente + (corrente * corrente);//Soma dos quadrados

      qtdDeAmostras++;
    } while ((millis() - inicio) < duracaoDaAmostragem);

    //Cálculo dos valores RMS
    acumuladorTensao = acumuladorTensao / float(qtdDeAmostras); //Média dos quadrados
    acumuladorCorrente = acumuladorCorrente / float(qtdDeAmostras); //Média dos quadrados

    acumuladorTensao = sqrt(acumuladorTensao); //RMS da captura
    acumuladorCorrente = sqrt(acumuladorCorrente); //RMS da captura

    acumuladorTensao = acumuladorTensao * coefA_VAC + coefB_VAC; //Conversão em volts
    acumuladorCorrente = acumuladorCorrente * coefA_IAC + coefB_IAC; //Conversão em ampères

    //Transmissão
    String mensagem = String(acumuladorTensao) + separador + String(acumuladorCorrente);
    LoRa.beginPacket();
    LoRa.print(mensagem);
    LoRa.endPacket();
    delay(1000);
  #endif


  #if MODO == 3
    //Envio dos códigos convertidos da tensão e corrente RMS (para o FK Serial)

    //coeficientes das regressões lineares. Usados para a conversão dos códigos
    const float coefA_VAC = 0.2652;
    const float coefB_VAC = 0.0;
    const float coefA_IAC = 0.1191;
    const float coefB_IAC = 0.0;

    //Período mínimo de captura de 16,7ms para garantir que ao menos um ciclo seja capturado
    const int32_t duracaoDaAmostragem = (int32_t)((1000.0 / 60.0) * 6.0); //6 ciclos de 1000ms/60 (60Hz)

    //Variáveis
    int32_t tensao = 0;
    int32_t corrente = 0;
    uint32_t qtdDeAmostras = 0;
    float acumuladorTensao = 0.0;
    float acumuladorCorrente = 0.0;

    //Captura
    unsigned long inicio = millis();
    do
    {
      tensao = analogRead(pinAdcTensao) - offsetVAC;
      corrente = analogRead(pinAdcCorrente) - offsetIAC;

      acumuladorTensao = acumuladorTensao + (tensao * tensao); //Soma dos quadrados
      acumuladorCorrente = acumuladorCorrente + (corrente * corrente);//Soma dos quadrados

      qtdDeAmostras++;
    } while ((millis() - inicio) < duracaoDaAmostragem);

    //Cálculo dos valores RMS
    acumuladorTensao = acumuladorTensao / float(qtdDeAmostras); //Média dos quadrados
    acumuladorCorrente = acumuladorCorrente / float(qtdDeAmostras); //Média dos quadrados

    acumuladorTensao = sqrt(acumuladorTensao); //RMS da captura
    acumuladorCorrente = sqrt(acumuladorCorrente); //RMS da captura

    acumuladorTensao = acumuladorTensao * coefA_VAC + coefB_VAC; //Conversão em volts
    acumuladorCorrente = acumuladorCorrente * coefA_IAC + coefB_IAC; //Conversão em ampères

    //Transmissão
    LoRa.beginPacket();
    LoRa.print("t");
    LoRa.print(separador);
    LoRa.print(micros());
    LoRa.print(separador);
    LoRa.print("Vac");
    LoRa.print(separador);
    LoRa.print(acumuladorTensao);
    LoRa.print(separador);
    LoRa.print("Iac");
    LoRa.print(separador);
    LoRa.println(acumuladorCorrente);
    LoRa.endPacket();
    delay(1000);
  #endif
}


 void ativarRele()
{
  if (statusRele == "OFF")
  {
    Serial.println("Caiu no if do ON");
    statusRele = String("ON");
    int readStatus = digitalRead(RelePin);
    Serial.println(String(readStatus));
    digitalWrite(RelePin, HIGH);
  }

  else
  {
    Serial.println("Caiu no if do OFF");
    statusRele = String("OFF");
    int readStatus = digitalRead(RelePin);
    Serial.println(String(readStatus));
    digitalWrite(RelePin, LOW);
  }
}