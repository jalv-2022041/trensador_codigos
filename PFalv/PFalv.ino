/*
 * Fundacion Kinal
 * Centro educativo tecnico laboral Kinal
 * Electronica
 * Grado: Quinto
 * Sección: A
 * Curso: TICS
 * Nombres: JOSUE ALVIZURIS, JOSUE PADILLA, BYRON DAVILA, EDGAR GALVEZ
 * CARNES: ...
 * Proyecto: TRENZADORA DE PULSERAS
 * 
 */

//Librerias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

//Funciones
void hacer_pulsera(void);

//Directivas
#define BTN_ENTER 2 // Boton para enviar la cantidad
#define BTN_MAS 12 // Pin del botón para sumar centímetros
#define BTN_MENOS 13 // Pin del botón para restar centímetros
//    Pines Stepper carrete 
#define PIN_STEP_CARRETE1 3            // Pin de la bobina A1
#define PIN_STEP_CARRETE2 4            // Pin de la bobina A2
#define PIN_STEP_CARRETE3 5            // Pin de la bobina B1
#define PIN_STEP_CARRETE4 6            // Pin de la bobina B2
//    Pines PuenteH enrrollado
#define MOTOR_IN1 7     //Mover el Motor DC hacia un lado
#define MOTOR_ENA A7    //Ajustar vellocidad

//Variables
int stp_cm = 2048; //Pasos pro centimetro
int cant_cm = 0; //Cantidad de cm
byte RPM = 400;  //Velocidad optima de entorchado

//Constructores
LiquidCrystal_I2C lcd(0x27, 16, 2);   //Constructor LCD
Stepper carrete(2048, PIN_STEP_CARRETE1, PIN_STEP_CARRETE2, PIN_STEP_CARRETE3, PIN_STEP_CARRETE4);  //Constructor Stepper


void setup() {
  Serial.begin(9600);  //Iniciar puerto Serial
  lcd.begin(16, 2);  //Iniciar LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   TRENZADOR    ");
  carrete.setSpeed(15);
  //Entradas
  pinMode(BTN_MAS, INPUT);    //Suma 1cm
  pinMode(BTN_MENOS, INPUT);  //Resta 1cm
  
  //Salidas
//    Carrete
  pinMode(PIN_STEP_CARRETE1, OUTPUT);
  pinMode(PIN_STEP_CARRETE2, OUTPUT);
  pinMode(PIN_STEP_CARRETE3, OUTPUT);
  pinMode(PIN_STEP_CARRETE4, OUTPUT);
  
//    Motor DC
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);
  analogWrite(MOTOR_ENA, RPM);   //Velocidad del motor DC
}

void loop() {
  Serial.println(cant_cm);
  lcd.setCursor(0, 1);
  lcd.print("LONG.: ");
  if(digitalRead(BTN_ENTER) == HIGH && digitalRead(BTN_MENOS) == LOW && digitalRead(BTN_MAS) == LOW){   //Sumar 1cm
    hacer_pulsera();
    }
  if(digitalRead(BTN_MAS) == HIGH && digitalRead(BTN_MENOS) == LOW){   //Sumar 1cm
    cant_cm++;
    delay(250);
    }
  if(digitalRead(BTN_MENOS) == HIGH && digitalRead(BTN_MAS) == LOW){  //Restar 1cm
    cant_cm--;
    delay(250);
    }
    lcd.print(cant_cm);
  lcd.print(" CM    ");
}

void hacer_pulsera() {
  lcd.setCursor(0, 1);
  if(cant_cm > 0 && cant_cm <= 100){
  lcd.print("   EN PROCESO   ");
  digitalWrite(MOTOR_IN1, HIGH);  //Enciende el entorchado
  for(int cm = 0; cm < cant_cm; cm++){   //Da la cantidad de vueltas necesarias para la cantidad de cm solicitados
    carrete.step(stp_cm);
    }
  delay(100);
  digitalWrite(MOTOR_IN1, LOW);  //Apaga el entorchado
  cant_cm = 0;
  lcd.setCursor(0, 1);
  lcd.print("   TERMINADO    ");
  delay(1000);
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("    INVALIDO    ");
    delay(1000);
  }
  }
