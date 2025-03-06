/*
     AUTOR:   Professor Danilo
     LINK:    https://www.youtube.com/@professordanilo ; https://linktr.ee/professor_danilo
     SKETCH:  Medindo empuxo do motor da Mobfog
     Detalhes: na coluna da esquerda, os pinos do módulo; na coluna da direita, os pinos do Arduino UNO
              RELÉ
              vcc - 5V
              gnd - GND
              in  - 7
              COM - chave liga desliga
              NO  - fio que vai para o skib

              DISPLAY
              scl - A5
              sda - A4
              vcc - 5V              
              gnd - GND

              BALANÇA
              gnd - GND
              dt  - 2
              sck - 3
              vcc - 5V

              BUZZER
              +   - 6
              -   - GND

              MÓDULO CARTÃO SD
              gnd  - GND
              miso - 12
              sck  - 13
              mosi - 11
              cs   - 10
              +5   - 5V
    
     DATA:    06/03/2025
*/

// INCLUSÃO DE BIBLIOTECAS
#include <HX711.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

// DEFINIÇÕES DE PINOS
#define pinDT 2
#define pinSCK 3
#define escala -801463 //calibrar a balança primeiro
#define buzzer 6
#define rele 7
#define desliga 1
#define liga 0
#define numeroMedidas 2
#define intervalo 25

//Inicializa o display no endereço 0x26
// MUDE PARA 0x27 CASO NÃO TENHA JAMPEADO SEU DISPLAY
LiquidCrystal_I2C lcd(0x26, 16, 2);

// INSTANCIANDO OBJETOS
HX711 scale;
File myFile;

// DECLARAÇÃO DE VARIÁVEIS
float medida = 0;
float forca = 0;
float gravidade = 9.8;
int pinoCS = 10;
unsigned long time;
unsigned long deltaTime;

void setup() {
  pinMode(rele, OUTPUT);
  digitalWrite(rele, desliga);

  Serial.begin(57600);  //mude o monitor serial para esta velocidade

  lcd.init();  //iniciando lcd
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Iniciando");
  lcd.setCursor(0, 1);
  lcd.print("cartao SD");
  delay(1000); //espera um segundo
  lcd.clear();
  if (!SD.begin(pinoCS)) {
    lcd.setCursor(0, 0);
    lcd.print("Leitura SD");
    lcd.setCursor(0, 1);
    lcd.print("FALHOU.");
    delay(3000);
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Verifique o");
    lcd.setCursor(0, 1);
    lcd.print("Cartao SD.");
    delay(10000);
    lcd.clear();
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Iniciado a");
  lcd.setCursor(0, 1);
  lcd.print("balanca.");
  delay(1000);

  scale.begin(pinDT, pinSCK);  // pinos de comunicação com a balança
  scale.set_scale(escala);     // limpando a escala
  /*para obter o número acima, rode o programa sem o número: () no lugar de (-801463)
  depois divida este número pelo valor do objeto utilizado como referencia em kg*/

  delay(2000);
  scale.tare();  // tara da balança
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Balanca zerada");
  delay(1000);
  for (int i = 40; i >= 11; i--) {
    lcd.setCursor(0, 0);
    lcd.print("Ligue o rele em ");
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.setCursor(2, 1);
    lcd.print("s e se AFASTE");
    delay(1000);
    lcd.clear();
  }
  for (int i = 10; i >= 4; i--) {
    lcd.setCursor(0, 0);
    lcd.print("CONT. REGRESSIVA");
    lcd.setCursor(7, 1);
    lcd.print(i);
    tone(buzzer, 440, 250);
    delay(1000);
    lcd.clear();
  }
  for (int i = 3; i >= 1; i--) {
    lcd.setCursor(0, 0);
    lcd.print("CONT. REGRESSIVA");
    lcd.setCursor(7, 1);
    lcd.print(i);
    tone(buzzer, 440, 250);
    delay(500);
    tone(buzzer, 440, 250);
    delay(500);
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("CONT. REGRESSIVA");
  lcd.setCursor(7, 1);
  lcd.print(0);
  tone(buzzer, 440, 3000);
  delay(1000);
  lcd.clear();
}

void loop() {
  File myFile = SD.open("empuxo.txt", FILE_WRITE);
  if (myFile) {
    myFile.print("Tempo (ms)");
    myFile.print("; ");
    myFile.println("Força (N)");
    digitalWrite(rele, liga);
    deltaTime = millis();
    for (int i = 0; i <= 100; i++) {
      medida = scale.get_units(numeroMedidas);  // salvando a média de n medidas
      forca = medida * gravidade; //peso = massa * gravidade
      //Serial.println(forca, 3); // envia medida para o monitor serial com 3 casas decimais
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("COLETANDO DADOS");
      lcd.setCursor(0, 1);
      lcd.print("NUM.");
      lcd.setCursor(5, 1);
      lcd.print(i);
      lcd.setCursor(9, 1);
      lcd.print(forca);
      lcd.setCursor(15, 1);
      lcd.print("N");
      time = millis() - deltaTime;
      myFile.print(time);
      myFile.print("; ");
      myFile.println(forca);
      delay(intervalo);
      digitalWrite(rele, desliga);
    }
  }
  myFile.close();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FIM DO EXPERIME.");
  lcd.setCursor(0, 1);
  lcd.print("DESLIGUE A BATE.");
  while (1) {}
  /*
  scale.power_down(); // desliga sensor
  delay(intervalo);
  scale.power_up(); //liga sensor
  */
}
