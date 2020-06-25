/* @autor: Esther Zurita Curbelo.
 * @asignatura: Periféricos e Interfaces.
 * @fecha: 03/02/19
 */
 
/*
 * Programa para simular un reloj a tiempo real (hora:minutos):
 * El usuario pulsa el botón auxiliar (conectado al pin 21 en este caso) para indicar una hora inicial.
 * Para colocar la hora inicial primero se modifican los minutos con los pulsadores: superior (para incrementar) e
 * inferior (para decrementar), y se pulsa el pulsador derecho una vez elegido los minutos para colocar la hora.
 * La hora se ajusta de la misma forma que los minutos, pero una vez finalizada la hora se pulsa en el pulsador del centro
 * Si no se pone una hora inicial empieza en 00:00 am.
 */
byte n[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D,0x7D, 0x07, 0x7F, 0x6F};  /* 
                                                                          * Vector auxiliar con los 9 dígitos que se mostrarán 
                                                                          * en los segmentos del display 7-segmentos para formar la hora.
                                                                          */
volatile int sec, und, dec, cent, mil, time1, time2, time3; /* 
                                                      * und y dec son variables para controlar los minutos y cent y mil para controlar
                                                      * la hora.
                                                      * time 1,2 y 3 para controlar las interrupciones que se produzcan.
                                                      * sec es la variable para controlar aproximadamente los segundos que pasan.
                                                      */
void setup() {
  DDRL = B00001111; // Puerto L se encuentra el display de 7-segmentos (PL[7:4] entrada y PL[3:0] de salida).
  DDRA = 0xFF;      //Puerto A de salida.
  DDRC = B00000001; // Puerto C se encuetra los pulsadores (PC[7:1] entrada y PC[0] salida).
  PORTC = B11111000; //Se activa la resistencia pull-up a los pulsadores.
  und =0;
  dec = 0;
  cent = 0;
  mil = 0;
  time2 = 0;
  time3 = 1000;
  sec= 0;
  pinMode(20, INPUT);
  
  cli(); 
  EICRA |= (1<<ISC11) | (1<<ISC10); //El pin 21 produce una interrupción en el flanco de subida.
  EIMSK |= (1<<INT1); 
  sei();
  cli(); 
  EICRA |= (1<<ISC01) | (1<<ISC00); //El pin 21 produce una interrupción en el flanco de subida.
  EIMSK |= (1<<INT0); 
  sei();
  PORTA = 0xFF;
  
}


void loop() {
  sec++; 
  PORTL = B11111110;
  PORTA = n[und];
  delay(5);
  PORTL = B11111101;
  PORTA = n[dec];
  delay(5);
  PORTL = B11111011;
  PORTA = n[cent];
  delay(5);
  PORTL = B11110111;
  PORTA = n[mil];
  delay(5);
  tone(20, 1000,1);
  
  
  
}
/*
 * Rutina de Servicio para el pin 21 (Hora inicial)
 */
ISR(INT0_vect){
  time1 = millis();
  if((time1-time2) > 100){ //Se comprueba interrupción
    time2 = time1;
    minutos();
    hora();
    tone(37, 300, 100); //Indica que se ha actualizado la hora inicial.
    sec = 0;
  }
}
/*
 * Rutina de servicio para el pin 20: incrementa los minutos y las horas correspondientes cada 60s.
 */
ISR(INT1_vect){
  if( time3 >= 32500){
    if(und <9){
          und++;
    }else{
          und= 0;
          if(dec <5){ //Se comprueba que no se llegua al minuto 60.
              dec++;
          }else{
              dec=0;
          }
    }
    if(dec == 0 && und == 0){ //Si los minutos son 00 indica que han pasado 60min y se incrementa la hora.
        if(mil != 2 && cent <9){ //Controlar que solo llegue hasta 23:59 pm
            cent++;
        }else if( mil != 2){
            cent= 0;
            mil++;
        }else if( cent <3){
            cent++;
        }else{
          cent = 0;
          mil = 0;
        }
    }
    tone(37, 300, 100); 
    time3 = 1000;
  }else{
    time3 +=1000;
  }
}



/*
 * Minutos iniciales que el usuario introduce.
 */
void minutos(){
  und = 0;
  dec = 0;
  while(bitRead(PINC, 5) != 0){ 
  if(bitRead(PINC,3) == 0){
      while(bitRead(PINC,3) == 0){
        PORTL = B11111110;
        PORTA = n[und];
        delay(5);
        PORTL = B11111101;
        PORTA = n[dec];
        delay(5);
      }
      if(und <9){
        und++;
      }else{
        und= 0;
        if(dec <5){
          dec++;
        }else{
          dec=0;
        }
      }
      delay(150);
      
  }else if(bitRead(PINC,6) == 0){
      while(bitRead(PINC,6) == 0){
         PORTL = B11111110;
         PORTA = n[und];
         delay(5);
         PORTL = B11111101;
         PORTA = n[dec];
         delay(5);
      }
      if(und >0){
        und--;
      }else{
        und= 9;
        if(dec >0){
          dec--;
        }else{
          dec=5;
        }
      }
      delay(150);
  }
  PORTL = B11111110;
      PORTA = n[und];
  delay(5);
  PORTL = B11111101;
  PORTA = n[dec];
  delay(5);
  }
  
}
/*
 * Hora inicial que el usuario introduce.
 */
void hora(){
  
  while(bitRead(PINC, 4) != 0){
  if(bitRead(PINC,3) == 0){
      while(bitRead(PINC,3) == 0){
        PORTL = B11111110;
        PORTA = n[und];
        delay(5);
        PORTL = B11111101;
        PORTA = n[dec];
        delay(5);
        PORTL = B11111011;
        PORTA = n[cent];
        delay(5);
        PORTL = B11110111;
        PORTA = n[mil];
        delay(5);
      }
      if(mil != 2 && cent <9){ //Para las horas entre 00h y 19h.
        cent++;
      }else if( mil != 2){
        cent= 0;
        mil++;
      }else if( cent <4){ //Para las horas entre  20h y 23h.
        cent++;
      }else{
        cent = 0;
        mil = 0;
      }
      delay(150);
      
  }else if(bitRead(PINC,6) == 0){
      while(bitRead(PINC,6) == 0){
        PORTL = B11111110;
        PORTA = n[und];
        delay(5);
        PORTL = B11111101;
        PORTA = n[dec];
        delay(5);
        PORTL = B11111011;
        PORTA = n[cent];
        delay(5);
        PORTL = B11110111;
        PORTA = n[mil];
        delay(5);
      }
     if(mil == 2 && cent >0){//Decrementar para las horas entre 20h y 23h.
        cent--;
     }else if( mil == 2 && cent == 0){ //Caso: 20h a 19h
        cent= 9;
        mil--;
    }else if( mil!= 0 && cent >0){ //Casos: desde 19h hasta 11h
      cent--;
    }else if( mil!= 0 && cent == 0){ //Caso: 10h.
      cent = 9;
      mil--;
    }else if( cent >0){ //Casos restantes.
      cent--;
    }else{ //Caso: 00h se pasa a 23h.
      cent = 3;
      mil = 2;
    }
      delay(150);
  }
        //Mostar las horas que se van eligiendo
        PORTL = B11111110;
        PORTA = n[und];
        delay(5);
        PORTL = B11111101;
        PORTA = n[dec];
        delay(5);
        PORTL = B11111011;
        PORTA = n[cent];
        delay(5);
        PORTL = B11110111;
        PORTA = n[mil];
        delay(5);
  }
}
