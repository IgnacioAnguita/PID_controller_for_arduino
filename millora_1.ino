#include <TimerOne.h>

float consigna = 0;
float D;
float error = 0;
float KP = 45000;
volatile int adc ;
float percentatge;
int adc_max = 0;
int adc_min = 0;
float error_i = 0;
float error_anterior = 0;
float Ki = 215000;
float Kd = 2362;
float Ts = 0.002;
bool verif = true;

void ISR_lectura() {
  if (verif == true) {
    //millora proposta, calcula ,l'angle màxim i el mínim.
    delay(100);//deixa esperar un temps per tenir un valor inicial fiable
    adc_min = analogRead(A0); //troba el valor de l'angle mínim
    D = 1023;
    Timer1.setPwmDuty(9, D); //posa el motor en marxa a màxima potència
    delay(2000); //el deixa funcionar fins que arribi al  màxim
    adc_max = analogRead(A0); //troba el valor de l'angle màxim
    D = 0; //apaga el motor
    Timer1.setPwmDuty(9, D);
    delay(2000);//deixa esperar uns segons al motor per que torni a la posició inicial
    verif = false;

  }
  if (verif != true) {
    adc = analogRead(A0); ///adc serà un valor entre 0 i 1023
    percentatge = (((float)adc - adc_min) / (adc_max - adc_min));//calculem percentatge
    error_anterior = error; //calculem l'error anterior
    error = consigna - percentatge;//calculem l'error
    error_i = error_i + error; //calculem l'error acumulat
    D = (KP * error) + Ki * Ts * (error_i) + Kd * (error - error_anterior) / Ts; //trobem la D per a un PI
    //Saturem la D
    if (D > 1023) {
      D = 1023;
    }
    if (D < 0) {
      D = 0;
    }
    Timer1.setPwmDuty(9, D);
  }

}

void setup() {
  Serial.begin(9600); //set up port serie
  Timer1.initialize(2000);//ini timer 1 amb T=2000 micro segons
  Timer1.pwm(9, 0); //init pwm
  Timer1.attachInterrupt(ISR_lectura);
}


void loop() {
  //llegim valor del tant per 1 per teclat
  if (Serial.available() > 0) {
    consigna = Serial.parseFloat();
  }
  //el saturem en cas de que es sobrepasi per sobra o sota dels valors límits
  if (consigna > 1) {
    consigna = 1;
  }
  if (consigna < 0) {
    consigna = 0;
  }

}


