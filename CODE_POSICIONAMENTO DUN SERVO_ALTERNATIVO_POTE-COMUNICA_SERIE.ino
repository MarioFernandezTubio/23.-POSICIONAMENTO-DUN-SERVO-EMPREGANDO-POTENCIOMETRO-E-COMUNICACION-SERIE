
/*
-----POSICIONAMENTO DUN SERVO EMPREGANDO ALTERNATIVAMENTE ----
		---- POTENCIÓMETRO OU COMUNICACIÓN SERIE ----

Realiza unha montaxe na que un servo (alimentado por 
unha batería externa) se conecte no pin PWM D03 (3~) 
como entrada de control (saída para Arduíno) e un potenciómetro 
se conecte no pin analóxico A05 como entrada do Arduíno. 
O programa  reactivo ao potenciómetro, pero ademais, 
se existe unha orde en forma de texto na entrada serie, 
débese priorizar a orde lida. É dicir, compróbase a entrada serie, 
se hai entrada execútase, se non existe orde en forma de texto, 
posiciónase o servo onde indique o potenciómetro. Ademais, 
se a posición do potenciómetro non variou, 
o servo debe permanecer na posición indicada polo monitor serie. 
No momento en que muden ou ben a posición do potenciómetro 
ou ben a orde do monitor serie, o servo debe reposicionarse. 
Sempre tendo en conta a prioridade do monitor serie sobre o potenciómetro.
Débese situar inicialmente o servo na posición 0º, 
antes de que o controle o monitor serie.

- O servo só se pode accionar un máximo de 0.15 veces por segundo
- O potenciómetro controla o ángulo de xiro do servo.


*****************************************************************/
/***************************************************************

Programa feito por Mario Fernández Tubío.
Última revisión: 05/05/2025

********************************************************/
#include <Servo.h>
#define pote A5

int valorPOTE = 0;        // Posición do Pote

/* Temos que ter claro que nesta montaxe a posición do Pote
   e a posición do servo non son a mesma e incluso poden non 
   coincidir. */

int pos = 0;              // Posición do servo
int novaPos = 0;          // Nova posición do servo
int ultimaPosPote = 0;   // Última posición do Pote
bool ordeserial = true;   // Entrada de texto ou de potenciómetro.

// Por defecto teremos a posición en 0º porque manda o TEXTO.

int veloz = 150;
int paso = 5;
String orde = "";         // Orde en texto, por iso é String


Servo VIRAVIRA;


/************* EMPEZAMOS CO CÓDIGO *************/

void setup(){
  
  VIRAVIRA.attach(3);
  Serial.begin(9600);
  
  pos = 0;             // Poñer en posición 0º
  VIRAVIRA.write(0);   // O servo ponse inicialmente en 0º
  
  Serial.println("Servo en 0 graos. A onde cho levo?...");
  
  while (Serial.available() == 0) {
      
}
} 

/*********** FUNCION LOOP (O NÚCLEO DO PROGRAMA) **********/
void loop(){
  
   if (Serial.available() > 0) {
    serial();        // A orde por texto sempre ten prioridade
  } else if (!ordeserial) {
    potencio();     // Se non hai nova orde, mirar o pote
  }
  
  
 /* Facemos unha lectura temporal para ver se o Pote cambiou.
    E se cambiou, pasamos a "modo potenciómetro".    */
  
    int lecturaTemp = analogRead(pote);
		lecturaTemp = constrain(lecturaTemp, 50, 975);
		lecturaTemp = map(lecturaTemp, 50, 975, 0, 180);
		lecturaTemp = (lecturaTemp / paso) * paso; 
  
  
/*   Non cambiamos a prioridade do serial ata que 
     o potenciómetro cambie.                         */
  
  if (lecturaTemp != ultimaPosPote) {
    	ultimaPosPote = lecturaTemp;
    	ordeserial = false;
  }
 
    delay(veloz);    // Tempo de reactividade.
}
  


/****** AGORA IMOS COAS FUNCIÓNS QUE CHAMAMOS NO CÓDIGO ******/


          /****** FUNCIÓN POTENCIÓMETRO ******/

void potencio (){   //Función do POTENCIÓMETRO.
 
// Lemos o pote 
  valorPOTE = analogRead(pote);

    
/* Limitamos o rango de entrada para asegurarnos de que 
   map() só recibe valores no rango esperado.

        constrain(valor,min,max)*/
  valorPOTE = constrain(valorPOTE, 50, 975);
  
/* Remapear o rango para que "50 sexa 0" e "975 sexa 1023".
     "map(valor, fromLow, fromHigh, toLow, toHigh)"*/
  valorPOTE = map(valorPOTE, 50,    975,       0,     180); 
  

// Axustamos á precisión de 10 grados
  valorPOTE = (valorPOTE / paso) * paso;

  
// Só mover o servo se cambiou a posición
  if (valorPOTE != ultimaPosPote) {   
    ultimaPosPote = valorPOTE;
      pos = valorPOTE;
      VIRAVIRA.write(pos);
       ordeserial = false;
    
// Saída ó monitor serie
    Serial.print("Servo movido a: ");
    Serial.print(pos);
    Serial.print(" graos -----> ");
    Serial.println(texto(pos));  

}
}

/**************** FUNCIÓN TEXTO SERIAL ********************/
  
void serial (){  //Función do TEXTO serial.
  
  
    // Ler a orde do monitor   
    orde = Serial.readStringUntil('\n'); 
   
    orde.trim(); /* Aseguramos lectura correcta 
                 eliminando os espazos en branco*/
  
// Interpretamos a orde
      novaPos = interpretarOrde(orde);
  
      if (novaPos != -1) {  /*Sempre que non sexa unha 
                              orde inválida*/
     
// Axustar á precisión
      pos = (novaPos / paso) * paso;
      VIRAVIRA.write(pos);    //Accionamos o servo  
      ordeserial = true;      // Mantemos a orde do monitor serie
      
	  Serial.print("Servo movido a: ");
      Serial.print(pos);
      Serial.print(" graos -----> ");
      Serial.println(texto(pos));
      } else {
      Serial.println("Repite, que non che entendo.");
    
  }

}


/********* Aquí Interpretamos a Orde do SERIAL *********/

//Función para interpretar a orde textual

int interpretarOrde(String orde) {
  
  orde.toLowerCase(); /* Convertemos todo texto escrito a 
  						 minúsculas. Así non temos que escribir
                         exactamente o texto da orde */
  
  if (orde == "esquerda" || orde == "180") return 180;
  
  else if (orde == "diagonal esquerda" || 
           orde == "135") return 135;
    
  else if (orde == "centro" || 
           orde == "90") return 90;
   
  else if (orde == "diagonal dereita" || 
           orde == "45") return 45;
    
  else if (orde == "dereita" || 
           orde == "0") return 0;
   
      
  return -1; // Orde non válida
}

// Función para devolver a orde textual da posición

String texto(int pos) {
  
  if (pos == 180) return "esquerda";
  else if (pos == 135) return "diagonal esquerda";
  else if (pos == 90) return "centro";
  else if (pos == 45) return "diagonal dereita";
  else if (pos == 0) return "dereita";
  else return String(pos);

}
