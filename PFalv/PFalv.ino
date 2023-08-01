#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <L298N.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BTN 2 // Pin del botón para seleccionar la longitud deseada
#define BTN_ADD 13 // Pin del botón para sumar centímetros
#define BTN_SUB 15 // Pin del botón para restar centímetros


int flag = 0;
int pulse = 0;
int last = 0;
int state = 0;
int desiredLONGITUD_cm = 0;  // Longitud deseada en centímetros

// Configuración del motor stepper unipolar (ajusta los valores según tu motor)
#define STEPS_PER_REV 32     // Número de pasos por vuelta completa
#define RPM 60               // Velocidad de rotación deseada en RPM
#define COIL_A1 3            // Pin de la bobina A1
#define COIL_A2 4            // Pin de la bobina A2
#define COIL_B1 5            // Pin de la bobina B1
#define COIL_B2 6            // Pin de la bobina B2

//Configuración del motor Stepper unipolar 2
#define NO1 9
#define NO2 10
#define NO3 11
#define NO4 12

// Configuración del motor controlado por L298
#define MOTOR_IN1 7
#define MOTOR_IN2 8

// Crear un objeto Stepper
Stepper stepper(STEPS_PER_REV, COIL_A1, COIL_A2, COIL_B1, COIL_B2);

// Crear un objeto Stepper para controlar el motor stepper con vueltas continuas
Stepper stepper2(STEPS_PER_REV, NO1, NO2, NO3, NO4);

// Crear un objeto L298N para controlar el motor con L298
L298N motorL298(MOTOR_IN1, MOTOR_IN2);

const int CM_PER_STEP = 2; // Número de centímetros recorridos por cada paso del motor

void setup() {
  pinMode(BTN, INPUT);
  pinMode(BTN_ADD, INPUT);
  pinMode(BTN_SUB, INPUT);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  pinMode(BTN, INPUT_PULLUP);
  pinMode(BTN, INPUT_PULLUP);
  lcd.setCursor(0, 0);
  lcd.print("  MAQUINA DE PULSERAS  ");
  lcd.setCursor(0, 1);
  lcd.print(" PRESS TO START ");
  
  // Inicializar el objeto Stepper con velocidad RPM
  stepper.setSpeed(RPM);
  // Inicializar el objeto Stepper con velocidad RPM
  stepper2.setSpeed(RPM);

  // Inicializar el motor controlado por L298
  motorL298.setSpeed(120);
}

void loop() {
  if (!digitalRead(BTN)) {
    delay(50);
    flag = 2;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  MAQUINA DE PULSERAS  ");
    lcd.setCursor(0, 1);
    lcd.print("LONGITUD = ");
    lcd.setCursor(13, 1);
    lcd.print("CM ");
  }

  if (flag == 2) {
    lcd.setCursor(9, 1);
    lcd.print(state);

    if (pulse != last) {
      state++;
      pulse = last;
      delay(10);
    }
  }

  if (state > 0 && !digitalRead(BTN)) {
    delay(50);
    flag = 3;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" MAQUINA DE PULSERAS  ");
    lcd.setCursor(0, 1);
    lcd.print("LONGITUD = ");
    lcd.setCursor(13, 1);
    lcd.print("CM ");
    
    // Esperar a que se presione el botón para seleccionar la longitud deseada
    while (digitalRead(BTN)) {
      // Esperar a que se presione el botón
    }

    // Leer el valor de longitud deseada
    desiredLONGITUD_cm = state;

    // Mostrar la longitud deseada en el LCD
    lcd.setCursor(9, 1);
    lcd.print(desiredLONGITUD_cm);
    lcd.print("   "); // Borrar caracteres anteriores

    // Calcular el número de pasos necesarios para recorrer los centímetros deseados con el motor stepper unipolar
    int steps_to_move = desiredLONGITUD_cm / CM_PER_STEP;

    // Mover el motor stepper para recorrer la distancia deseada en centímetros
    stepper.step(steps_to_move);

    // Mover el motor stepper con vueltas continuas para que realice vueltas continuas
    stepper2.step(STEPS_PER_REV);

    // Encender el motor controlado por L298 y esperar a que termine de enrollar
    motorL298.forward();
    motorL298.stop(); // Detener el motor L298 después de que stepper2 haya terminado de moverse

    // Reiniciar estado y mostrar mensaje de finalización
    state = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  MAQUINA DE PULSERAS  ");
    lcd.setCursor(0, 1);
    lcd.print("   TRABAJO COMPLETO   ");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  MAQUINA DE PULSERAS  ");
    lcd.setCursor(0, 1);
    lcd.print(" PRESS TO START ");
  }
  // Si el botón de sumar centímetros es presionado
  if (digitalRead(BTN_ADD) == LOW) {
    delay(50); // Pequeño retardo para evitar rebotes
    desiredLONGITUD_cm++; // Incrementar la longitud deseada en 1 centímetro

    // Mostrar la longitud deseada en el LCD
    lcd.setCursor(9, 1);
    lcd.print(desiredLONGITUD_cm);
    lcd.print("   "); // Borrar caracteres anteriores

    // También podrías implementar un límite máximo para la longitud deseada si lo deseas.
    // Por ejemplo:
    // if (desiredLONGITUD_cm > MAX_LONGITUD_CM) {
    //   desiredLONGITUD_cm = MAX_LONGITUD_CM;
    // }

    // Esperar a que se suelte el botón
    while (digitalRead(BTN_ADD) == LOW) {
      // Esperar a que se suelte el botón
    }
  }

  // Si el botón de restar centímetros es presionado
  if (digitalRead(BTN_SUB) == LOW) {
    delay(50); // Pequeño retardo para evitar rebotes
    desiredLONGITUD_cm--; // Decrementar la longitud deseada en 1 centímetro

    // Mostrar la longitud deseada en el LCD
    lcd.setCursor(9, 1);
    lcd.print(desiredLONGITUD_cm);
    lcd.print("   "); // Borrar caracteres anteriores

    // Asegurarse de que la longitud deseada no sea negativa
    if (desiredLONGITUD_cm < 0) {
      desiredLONGITUD_cm = 0;
    }

    // Esperar a que se suelte el botón
    while (digitalRead(BTN_SUB) == LOW) {
      // Esperar a que se suelte el botón
    }
  }
}
