#define programname "LoRa_Module_Test_868Mhz"
#define programversion "V1.0"
#define dateproduced "10/10/2018"
#define aurthorname "Stuart Robinson"
#include <Arduino.h>


/*
*****************************************************************************************************************************
  LoRaTracker Test Programs

  Copyright of the author Stuart Robinson - 26/11/2017

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without the explicit permission
  of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the intended purpose and
  free from errors.
*****************************************************************************************************************************
*/

/*
********************************************************************************************************************************
  Program operation

  This test program has been written to check that a connected LoRa module such as Dorji DRF127x or Hope RFM9x that it is funtional.
  The LoRa device should transmit an FM audio tones at the frequency specified that can be heard on a UHF handheld FM receiver.
  It might seem peverse to test a LoRa device by sending FM audio tones, but its the easiest way of checking that the LoRa device
  is actually transmitting.

  A program start (power on and reset) there will be a short sequence of 5 LED flashes, then the LED should be constant on for around
  2.5 seconds. At this time the LoRa device should be sending an FM tone on the defined frequency. The register contents of the LoRa
  device will also be printed to the Arduino IDE serial monitor. A error detecting the LoRa device will result in a series of very
  fast LED flashes lasting around 5 seconds. The test sequence will repeat.

  Do not forget to fit an antenna to the LoRa device, you can destroy it if you use it without and antenna
********************************************************************************************************************************
*/

/*
********************************************************************************************************************************
  Connections

  The program uses the hardware SPI interface on the Arduino to connect to the LoRa device, so the SPI SCK, MOSI and MISO pins are
  assumed to be connected. The test program needs a minimum of one extra pin connected to act as chip select. Other pins
  may optionally be connected to the reset pin on the LoRa device and to DIO2 so that the LoRa device can transmit FM tones.
  You can explicitly define the required pins below by removing the two // characters in front of the #defines
********************************************************************************************************************************
*/

#define lora_NSS 10                  //Arduino pin number for NSS (device select) on LoRa device
#define lora_NReset 9                //Arduino pin number for reset pin on LoRa device, can be left not connected
#define lora_TonePin 6               //Arduino pin number connected to DIO2 pin on LoRa device, used for FM audio tone generation, can be left not connected 
#define lora_DIO0 2                  //Arduino pin number connected to DIO0 pin on LoRa device, can be left not connected
#define LED1 8                       //Arduino pin number for LED, when high LED should be on.  

#include <SPI.h>

const unsigned long Frequency = 868000000;          //frequency of transmissions in hertz
const int CalibrationOffset = 0;                    //you can use this to adjust the output, in hertz, steps are 61hz

#include "LoRa_Test.h"

#define Serial_Monitor_Baud 115200                   //this is baud rate used for the Arduino IDE Serial Monitor

void loop()
{
  float tempfloat;
  Serial.println(F("LED Flash"));
  Serial.println();
  led_Flash(5, 100);

  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  Serial.println(F("Checking LoRa Device"));
  Serial.println();
  Serial.print(F("Registers after reset - "));
  lora_ResetDev();                                     //ensure registers are in initial state
  lora_Print();                                        //initial print of registers
  tempfloat = lora_GetFreq();
  Serial.print(F("Frequency after reset - "));
  Serial.print(tempfloat, 3);
  Serial.println(F("Mhz"));
  Serial.println();

  if (lora_CheckDevice() == true)
  {
    Serial.println(F("Device Present"));
    Serial.println();
    Serial.print(F("Registers after setup - "));
    lora_Setup();                                      //Do the initial LoRa Setup
    lora_Print();
    lora_SetFreq(Frequency, CalibrationOffset);
    tempfloat = lora_GetFreq();
    Serial.print(F("New Frequency - "));
    Serial.print(tempfloat, 3);
    Serial.println(F("Mhz"));
    Serial.print(F("Transmit FM Tone"));
    digitalWrite(LED1, HIGH);
    lora_Tone(1000, 3500, 2);                          //Transmit an FM tone, 1000hz, 2500ms, 2dBm
    digitalWrite(LED1, LOW);
    Serial.println(F(" - Done"));
    Serial.println();
    lora_Print();
  }
  else
  {
    Serial.println(F("Device Not Found"));
    Serial.println();
    lora_Print();
    Serial.println();
    led_Flash(100, 25);
  }

  SPI.end();
  Serial.println();
  Serial.println();
  delay(1500);
}


void led_Flash(unsigned int flashes, unsigned int delaymS)
{
  //flash LED to show tracker is alive
  unsigned int index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}



void setup()
{
  pinMode(LED1, OUTPUT); 			               //for PCB LED
  pinMode(13, OUTPUT); 			                 //for Pro Mini LED, Pin13
  pinMode(lora_NReset, OUTPUT);			         //LoRa Device reset line
  pinMode (lora_NSS, OUTPUT);			           //LoRa Device select line
  digitalWrite(lora_NSS, HIGH);
  digitalWrite(lora_NReset, HIGH);

  Serial.begin(Serial_Monitor_Baud);         //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.println();

}
