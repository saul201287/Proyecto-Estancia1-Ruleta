#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include <stdlib.h> // Para usar funciones relacionadas con números aleatorios

#define MAX_DEVICES 8
#define CS_PIN 5 // Pin de Chip Select
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Definición de la matriz de leds
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Lista de mensajes aleatorios
char *mensajes[] = {"Robo", "El robo es malo", "no debes", "Hacerlo"};

// Variable estática para mantener el índice del mensaje actual
static int indiceMensajeActual = 0;

void setup() {
  P.begin();
  // Inicia el generador de números aleatorios con una semilla basada en el tiempo actual
  randomSeed(analogRead(0));
}

void loop() {
  mensajeSelect();
  P.displayAnimate();
  delay(2000);
}

void mensajeSelect() {
  // Muestra el mensaje actual
  P.displayText(mensajes[indiceMensajeActual], PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  
  // Incrementa el índice del mensaje actual
  indiceMensajeActual++;
  
  // Si llega al final de la lista de mensajes, vuelve al principio
  if (indiceMensajeActual >= sizeof(mensajes) / sizeof(mensajes[0])) {
    indiceMensajeActual = 0;
  }
}