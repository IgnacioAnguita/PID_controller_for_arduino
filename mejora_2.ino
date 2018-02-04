#include <TimerOne.h>

float D;
float error=0;
float KP = 1;
volatile int adc ;
float percentatge;
int adc_max = 969;//partim de que coneixem els valors corresponents als angles màxims i mínims
int adc_min = 641;
float error_i = 0;
float error_anterior = 0;
float Ki = 0;
float Ts = 10; //es un valor arbitràri
float consigna = 0.25;
float t=0;
bool verif_Kp = false;
bool verif_KI = false;
bool verif2 = true;


void ISR_lectura() {
  if(verif2 == true){
    adc = analogRead(A0); ///adc serà un valor entre 0 i 1023
    percentatge = (((float)adc - adc_min) / (adc_max - adc_min));//calculem percentatge
    error_anterior = error;//calculem l'error anterior
    error = consigna - percentatge;//calculem l'error
    error_i = error_i + error;//calculem l'error acumulat
    D = (KP * error) + Ki * Ts * (error_i);//trobem la D per a un PI
    //Saturem la D
    if (D > 1023) {
      D = 1023;
    }
    if (D < 0) {
      D = 0;
    }
    Timer1.setPwmDuty(9, D); 
    t = t + 1; //augmentem el contador
  }
  if (verif2 == false) {
    D = 0; //reiniciem tot posant el motor a 0
    Timer1.setPwmDuty(9,D);
    delay(2000); //esperem que torni a la posició inicial
    verif2 = true;
  }
}

void setup() {
  Serial.begin(9600); //set up port serie
  Timer1.initialize(2000);//ini timer 1 amb T=2000 micro segons
  Timer1.pwm(9, 0); //init pwm
  Timer1.attachInterrupt(ISR_lectura);
}


void loop() {
  if (t = 2000 && verif_Kp == false) { 
    if (error > 0.15) { //varia la Kp depenent de l'error, deixem bastant marge d'error per parar abans de que hi hagin oscilacions
      error = abs(error); //posem un error absolut
      KP = KP * exp(error); //La Kp augmenta exponencialment amb l'error, si l'error es fa petit, l'augment tendirà a 0.
      verif2 = false; //reiniciem el sistema apagant el motor
      t = 0; //reiniciem el contador
    }
    if (error < 0.15) {
      KP = 0.8 * KP; //baixem la Kp ja que hi posarem un integrador
      verif_Kp = true; //sortim de l'if de la Kp per pasar al if de la Ki;
      verif2 = false; //reiniciem el sistema apagant el motor
      t = 0; //reiniciem el contador
      Ki=1; //li donem un valor inicial a la Ki
    }
  }
  //amb la següent iteració busquem la millor Ki, deixant bastant marge i anant més lents que amb
  //la Kp ja que la Ki pot doner oscilacions molt ràpidament.
  if (t = 5000 && verif_KI == false) {

    if (error > 0.05) { //varia la Ki depenent de l'error, deixem bastant marge d'error per parar abans de que hi hagin oscilacions
      error = abs(error); //posem un error absolut
      Ki = Ki * exp(error); //La Ki augmenta exponencialment amb l'error, si l'error es fa petit, l'augment tendirà a 0.
      verif2 = false; //reiniciem el sistema apagant el motor
      t = 0; //reiniciem el contador
      error_i=0;//posem a 0 l'error acumulat per tornar a iterar en busca d'una nova Ki
    }
    if (error < 0.05) {
      verif_KI = true;//una vegada ja tenim un error suficientment petit sortim de totes les iteracions
    }

  }
  //imprimim per pantalla els valors de Kp,Ki i error
  Serial.print("Kp= ");
  Serial.print(KP);
  Serial.print(" ");
  Serial.print("Ki= ");
  Serial.print(Ki);
  Serial.print(" ");
  Serial.print("Error =");
  Serial.println(error);
}


