#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include "uRTCLib.h"
#define MODE 2
#define SET 3
#define UP 4
#define DOWN 5
#define SALARMA 6
#define ALARMA 8
#define STEMP A0

uRTCLib rtc(0x68);
LiquidCrystal_I2C lcd(0x27, 16, 2);

char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
int horas, minutos, segundos, aHora = 12, aMinutos = 0, contador = 0, valorSensor, periodo = 3000;
unsigned long TiempoAhora = 0;
float volts, celsius;
void setup()
{
  URTCLIB_WIRE.begin();
  //rtc.set(0, 22, 0, 1, 18, 6, 23);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year) set day of week (1=Sunday, 7=Saturday)
  lcd.begin();
  pinMode(MODE, INPUT);
  pinMode(SET, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(SALARMA, INPUT);
  pinMode(ALARMA, OUTPUT);
}

void loop()
{
  TiempoAhora = millis();
  while (millis() < TiempoAhora+periodo)
  {
    rtc.refresh();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora: ");
    lcd.print(rtc.hour());
    lcd.print(':');
    lcd.print(rtc.minute());
    lcd.print(':');
    lcd.print(rtc.second());

    lcd.setCursor(0, 1);
    lcd.print("Fecha: ");
    lcd.print(rtc.day());
    lcd.print('/');
    lcd.print(rtc.month());
    lcd.print('/');
    lcd.print(rtc.year());
    if (digitalRead(MODE) == 1)
    {
      delay(200);
      while (digitalRead(SET) == 0 || digitalRead(MODE) == 0)
      {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Conf. Hora, SET");
        lcd.setCursor(0, 1);
        lcd.print("Next, MODE");
        if (digitalRead(SET) == 1 && digitalRead(MODE) == 1)
          break;
        else if (digitalRead(SET) == 1)
        {
          delay(200);
          horas = rtc.hour();
          while (digitalRead(SET) == 0)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ajustar hora:");
            lcd.setCursor(0, 1);
            lcd.print(horas);
            if (digitalRead(UP) == 1)
            {
              horas++;
              if (horas > 23)
                horas = 0;
            }
            if (digitalRead(DOWN) == 1)
            {
              horas--;
              if (horas < 0)
                horas = 23;
            }
            delay(200);
          }
          delay(200);
          minutos = rtc.minute();
          while (digitalRead(SET) == 0)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ajustar minutos:");
            lcd.setCursor(0, 1);
            lcd.print(minutos);
            if (digitalRead(UP) == 1)
            {
              minutos++;
              if (minutos > 59)
                minutos = 0;
            }
            if (digitalRead(DOWN) == 1)
            {
              minutos--;
              if (minutos < 0)
                minutos = 59;
            }
            delay(200);
          }
          delay(200);
          segundos = rtc.second();
          while (digitalRead(SET) == 0)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ajustar segundos:");
            lcd.setCursor(0, 1);
            lcd.print(segundos);
            if (digitalRead(UP) == 1)
            {
              segundos++;
              if (segundos > 59)
                segundos = 0;
            }
            if (digitalRead(DOWN) == 1)
            {
              segundos--;
              if (segundos < 0)
                segundos = 59;
            }
            delay(200);
          }
          rtc.set(segundos, minutos, horas, daysOfTheWeek[rtc.dayOfWeek() - 1], rtc.day(), rtc.month(), rtc.year());
        }
        else if (digitalRead(MODE) == 1)
        {
          while (digitalRead(SET) == 0 || digitalRead(MODE) == 0)
          {
            delay(200);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Alarma, SET");
            lcd.setCursor(0, 1);
            lcd.print("FechaHora, M+S");
            if (digitalRead(SET) == 1)
            {
              delay(200);
              while (digitalRead(SET) == 0)
              {
                delay(200);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Config. hora");
                lcd.setCursor(0, 1);
                lcd.print(aHora);
                if (digitalRead(UP) == 1)
                {
                  aHora++;
                  if (aHora > 23)
                    aHora = 0;
                }
                if (digitalRead(DOWN) == 1)
                {
                  aHora--;
                  if (aHora < 0)
                    aHora = 23;
                }
              }
              delay(200);
              while (digitalRead(SET) == 0)
              {
                delay(200);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Config. min.");
                lcd.setCursor(0, 1);
                lcd.print(aMinutos);
                if (digitalRead(UP) == 1)
                {
                  aMinutos++;
                  if (aMinutos > 59)
                    aMinutos = 0;
                }
                if (digitalRead(DOWN) == 1)
                {
                  aMinutos--;
                  if (aMinutos < 0)
                    aMinutos = 59;
                }
              }
            }
            else if (digitalRead(MODE) == 1)
              break;
          }
        }
      }
    }
    if (digitalRead(SALARMA) == 1 && aHora == rtc.hour() && aMinutos == rtc.minute())
    {
      contador = 0;
      while (contador < 60)
      {
        digitalWrite(ALARMA, 1);
        delay(500);
        digitalWrite(ALARMA, 0);
        delay(500);
        contador++;
        if (digitalRead(SALARMA) == 0)
          break;
      }
      aHora = 12;
      aMinutos = 0;
    }
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Temperatura");
  valorSensor = analogRead(STEMP);
  volts = (valorSensor * 5) / 1023.0;
  celsius = volts * 100;
  lcd.setCursor(5,1);
  lcd.print(celsius);
  delay(3000);
}
