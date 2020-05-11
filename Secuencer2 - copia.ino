#include <Adafruit_MCP3008.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_MCP3008 adc;

//variables del keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','4'},
{'5','6','7','8'},
{'9','A','B','C'},
{'D','E','F','G'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//variables multiple-hold del keypad
unsigned long loopCount;
unsigned long startTime;
String msg;

//variables del filtrado digital
float alpha=0.5;
float Y0=0.0;
float Y1=0.0;
float Y2=0.0;
float Y3=0.0;
float Y4=0.0;
float Y5=0.0;
float Y6=0.0;
float Y7=0.0;
float Y8=0.0;
float Y9=0.0;
float S0=Y0;
float S1=Y1;
float S2=Y2;
float S3=Y3;
float S4=Y4;
float S5=Y5;
float S6=Y6;
float S7=Y7;
float S8=Y8;
float S9=Y9;
//datos filtrados para slider midi
//los datos midi se mapean en un rango de 127
int d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
//datos filtrados para secuencia
//los datos de secuancia se mapean dependiendo del diseño del secuenciador
//las propuestas pueden ser en 8 (una octava), 15 (2 octavas), 9(pasos), X(tempo)
int p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;

//matriz de notas
int notas[8][12] = {
  {24,25,26,27,28,29,30,31,32,33,34,35},
  {36,37,38,39,40,41,42,43,44,45,46,47},
  {48,49,50,51,52,53,54,55,56,57,58,59},
  {60,61,62,63,64,65,66,67,68,69,70,71},
  {72,73,74,75,76,77,78,79,80,81,82,83},
  {84,85,86,87,88,89,90,91,92,93,94,95},
  {96,97,98,99,100,101,102,103,104,104,106,107},
  {108,109,110,111,112,113,114,115,116,117,118,119}
};
int octava = 3;

//matriz temporal para secuencia
int notas_secuencia[25];

//variables conteo canal
int sum_canal=1;
int canal=1;
int var_canal=0;
int var_canal_control=0;

//variable cambio pots
int lecturas_norm[10];
int lecturasAnteriores_norm[10];
int lecturas_seq[10];
int lecturasAnteriores_seq[10];

int contawhile=0;

//variables para el filtrado multimap
int in[]  = {0,7,121,142,416,532,617,707,791,860,923,1023};
int out[] = {0,93,186,279,372,465,558,651,744,837,930,1023};

//función multimap
int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] already tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}

void setup() {
  Serial.begin(115200);
  //Inicio ADC
  //while (!Serial);
  adc.begin();
  //Inicio key-hold
  loopCount = 0;
  startTime = millis();
  msg = "";
  //Inicio LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BIENVENIDO");
  lcd.setCursor(0,1);
  lcd.print("Espera...");

  //Espera para tomar datos filtrados
  while(contawhile<100){
    Pots();
    contawhile=contawhile+1;
  }
  lecturas_norm[0]=d0;
  lecturas_norm[1]=d1;
  lecturas_norm[2]=d2;
  lecturas_norm[3]=d3;
  lecturas_norm[4]=d4;
  lecturas_norm[5]=d5;
  lecturas_norm[6]=d6;
  lecturas_norm[7]=d7;
  lecturas_norm[8]=d8;
  lecturas_norm[9]=d9;

  lcd.clear();
}

void loop() {
  count++;
  loopCount++;
  if ( (millis()-startTime)>5000 ) {
     //Serial.print("Average loops per second = ");
     //Serial.println(loopCount/5);
     startTime = millis();
     loopCount = 0;
  }

  if(digitalRead(A0)==FALSE){
    lcd.setCursor(0,0);
    lcd.print("Teclado ")
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Secuen. ")
  }
  lcd.setCursor(8,0);
  lcd.print("Oc:")
  lcd.print(octava)
  lcd.setCursor(13,0);
  lcd.print("C:")
  lcd.print(canal)

  if(kpd.getKeys()){
    for(int i=0; i<LIST_MAX; i++){
      if(kpd.key[i].stateChanged){
        switch(kpd.key[i].kstate){
          case PRESSED:
            if(digitalRead(A0)==FALSE){

            }
            break;
          case HOLD:
            break;
          case RELEASED:
            break;
          case IDLE:
            break;
        }
      }
    }
  }
}

void Pots(){
  Y0=multiMap((float)adc.readADC(0), in, out, 12);
  Y1=multiMap((float)adc.readADC(1), in, out, 12);
  Y2=multiMap((float)adc.readADC(2), in, out, 12);
  Y3=multiMap((float)adc.readADC(3), in, out, 12);
  Y4=multiMap((float)adc.readADC(4), in, out, 12);
  Y5=multiMap((float)adc.readADC(5), in, out, 12);
  Y6=multiMap((float)adc.readADC(6), in, out, 12);
  Y7=multiMap((float)adc.readADC(7), in, out, 12);
  Y8=multiMap((float)analogRead(A2);, in, out, 12);
  Y9=multiMap((float)analogRead(A3);, in, out, 12);
  S0=(alpha*Y0)+((1-alpha)*S0);
  S1=(alpha*Y1)+((1-alpha)*S1);
  S2=(alpha*Y2)+((1-alpha)*S2);
  S3=(alpha*Y3)+((1-alpha)*S3);
  S4=(alpha*Y4)+((1-alpha)*S4);
  S5=(alpha*Y5)+((1-alpha)*S5);
  S6=(alpha*Y6)+((1-alpha)*S6);
  S7=(alpha*Y7)+((1-alpha)*S7);
  S8=(alpha*Y8)+((1-alpha)*S8);
  S9=(alpha*Y9)+((1-alpha)*S9);
  if(digitalRead(A0)==FALSE){
    d0=map(S0,0,1024,0,127);
    d1=map(S1,0,1024,0,127);
    d2=map(S2,0,1024,0,127);
    d3=map(S3,0,1024,0,127);
    d4=map(S4,0,1024,0,127);
    d5=map(S5,0,1024,0,127);
    d6=map(S6,0,1024,0,127);
    d7=map(S7,0,1024,0,127);
    d8=map(S8,0,1024,0,127);
    d9=map(S9,0,1024,0,127);
  }
  else{
    p0=map(S0,0,1024,0,127);
  }
}