//module.setDisplayDigit(11, 1, 0); //число, номер индикатора(0-7), точка(0-1)
//delay(1000);
//module.clearDisplayDigit(1, 0); //номер индикатора(0-7), точка(0-1)
//delay(1000);

//PORTB |= (1 << PB5);//pin HIGH
//PORTB &= ~(1 << PB5);//pin LOW

#include <TM1638.h>

TM1638 module(2, 3, 4);//DIO, CLK, STB.     PD2, PD3, PD4
int j = 0;                     // считалка до трёх на Uметр
volatile int button = 0;       // состояние кнопок TM1638
int Ubat = 0;                  // напряжение батареи
int UbatPin = A2;              // пин для измерения напряжение батареи
int Uheadlight = 0;            // напряжение на фаре
int UheadlightPin = A3;        // пин для измерения напряжение на фаре
int Iheadlight = 0;            // ток фары
int IheadlightPin = A4;        // пин для измерения тока фары
int Iper = 0;                  // ток периферии
int IperPin = A5;              // ин для измерения тока периферии
volatile int led = 0;          //

void setup() {
  ВСЁ ПРОЕВРИТЬ!!!!!!!!!!!!!!
  DDRD |= 01111100;     //6-2 выходы
  PORTD |= (1 << PD4);     // вывод 4 в HIGH, все выкл
  PORTD |= (1 << PD5);     // вывод 5 в HIGH
  PORTD |= (1 << PD6);     // вывод 6 в HIGH
  analogReference(INTERNAL);// 1.1 В
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  module.setupDisplay(true, 0); // яркость (0~7).
  //8.8.8.8.8.8.8.8.
  for (int i = 0; i < 8) {
    module.setDisplayDigit(8, i - 1, 1);//число, номер индикатора(0-7), точка(0-1)
    i++;
  }
  delay(1000);
  module.clearDisplay();


  // настройка прерывания по таймеру каждые 259 мс (вызов функции ISR (TIMER1_OVF_vect))
  //TIMSK2 |= (1 << TOIE2);                             //разрешить прерывание при переполнении
  //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);  //установить делитель частоты на 1024
  //sei();                                              //разрешить прерывания

  TIMSK1 |= 0x01;     // прерывание по переполнению
  TCCR1B = 0x03;      // делитель на 64
  TIFR1 = 0x00;       // очищаем флаг переполнения (Overflow Flag)
  TCCR1A = 0x00;      // Timer2 Control Reg A: Wave Gen Mode normal
  sei();              // разрешение прерываний
}


void loop() {
  // стоп
  delay(50);
  if (~bitRead(PIND, 6)) {
    Serial.println('h');
  }
  else {
    Serial.println('i');
  }
}



ISR (TIMER1_OVF_vect) {         //функция, вызываемая таймером-счетчиком каждые 259 мс.{

  led &= B10001111; проеврить!!!!!!!!!!
  led |= B00001110;



  button = module.getButtons();

  switch (button) {                // HIGH - выкл, LOW - вкл!!! &= ~    |=               PD5 - фара, PB5 - кол., PB4 - габ.
    case 1:// если нажата кнопка 1 (все выкл)
      PORTD |= (1 << PD5);     // (9)  фара выкл
      PORTD |= (1 << PB5);     // (17) кол. выкл
      PORTD |= (1 << PB4);     // (16) габ. выкл
      led = B00000000;
      Serial.println('a');  //Serial.print(byte(78)) передается как "N"
      break;

    case 2:// если нажата кнопка 2 ((PB4)(16) габ.)
      if (bitRead(PORTB, 4) == HIGH) {
        PORTB &= ~(1 << PB4);     //габ. LOW
        led &= B11111101;         //led low
      }
      else {
        PORTD |= (1 << PB4);     //габ. HIGH
        led |= B00000010;        //led high
        Serial.println('b');
      }
      break;

    case 4:// если нажата кнопка 3 ((PD5)(9)  фара)
      if (bitRead(PORTD, 5) == HIGH) {
        PORTB &= ~(1 << PD5);     //фара LOW
        led &= B11111011;         //led low
      }
      else {
        PORTD |= (1 << PD5);     //фара HIGH
        led |= B00000100;        //led high
        Serial.println('b');
      }
      break;

    case 8:// если нажата кнопка 4 ((PB5)(17) кол.)
      if (bitRead(PORTB, 5) == HIGH) {
        PORTB &= ~(1 << PB5);     //кол. LOW
        led &= B11110111;         //led low
      }
      else {
        PORTD |= (1 << PB5);     //кол. HIGH
        led |= B00001000;        //led high
        Serial.println('b');
      }
      break;

      //default:
  }


  //PORTB |= (1 << PB5);//pin HIGH
  //PORTB &= ~(1 << PB5);//pin LOW

  if (bitRead(PINC, 0) & bitRead(PORTB, 2)) {           //ПП                   проверить bitRead(PORTB на выходах на транзисторы
    PORTB &= ~(1 << PB2);    //pin low, led high
    led |= B00000001;
    Serial.println('d');
  }
  else {
    PORTB |= (1 << PB2);     //pin high, led low
    led &= B11111110;
    Serial.println('e');
  }

  if (bitRead(PINC, 1) & bitRead(PORTB, 3)) {             //ЛП
    PORTB &= ~(1 << PB3);    //pin low, led high
    led |= B10000000;
    Serial.println('f');
  }
  else {
    PORTB |= (1 << PB3);     //pin high, led low
    led &= B01111111;
    Serial.println('g');
  }

  module.setLEDs(led);

  j++;
  if (j >= 3) {
    // проверка напряжения
    Ubat = analogRead(UbatPin);
    Serial.println(Ubat);
    //индикация напряжения
    Ubat = Ubat / 6;
    module.setDisplayToString("   U");
    for (int i = 0; i < 3; i++) {
      module.setDisplayDigit(Ubat % 10, 2 - i, (2 & (1 << i)) != 0);
      Ubat /= 10;
    }
    j = 0;
  }


}