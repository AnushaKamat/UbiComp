/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include <Adafruit_CircuitPlayground.h>
#include <stdio.h>
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
// Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
// Variable Definition

bool record_flag = 0,send_flag = 0;
#define N 600
#define M 3
int numofsamples=0;
float X= 0 ,Y = 0, Z= 0;
float features[M] ={0};
float max_x =0,max_y =0,max_z =0;
float mean_x =0, mean_y= 0,mean_z=0;



void setup(void)
{
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels(); //clear all pixels

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  /* Print Bluefruit information */
  ble.info();

  ble.verbose(false);  

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }


  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);

  CircuitPlayground.setPixelColor(0, 155,   150,   150); //LED to detect correct connection
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/




/*
 * Algorithm :
 *  1. Android sends command to record gesture
 *     Start command - A
 *     Stop COmmand  - Z
 *     Time Duration - 2sec
 *    
 *  2. In case of start command , record flag is ON , 2nd LED goes green
 *  
 *  3. In case of Stop Command , send flag is ON , 9th LED goes purple
 *  
 *  4. After message is sent, Clear Function helps Record and Send flag go OFF , 2nd and 9th LED go OFF
 *  
 *  5. Record Function  - Records motion in X, Y , Z direction 
 *                      - Finds Max of accelerometer data in X, Y , Z
 *                      - Accumulates all the data in respective direction and keeps count of number of samples recorded
 *  
 *  6. SendTo Phone Function - Calculates mean using accumulated value and number of samples
 *                           - Converts max and mean calculated into string values
 *                           - Creates a message template adding header('o'), end('x') and delimiters(',')
 *                           - Sends message to Bluetooth connected device 
 */



 
void loop(void)
{

  while ( ble.available() )
  {
    int c = ble.read();

    if((char)c == 'A')
    {
      record_flag = 1;          
    }
    if ((char)c == 'Z')
    {
     send_flag =1;
    }
   
  }
 

  if (send_flag == 1) 
  {
    CircuitPlayground.setPixelColor(8, 155,   30,   30);
    SendToPhone();
    clearparameters();
  }

   if (record_flag ==1)
  {
    CircuitPlayground.setPixelColor(1, 155,   150,   0);
    record();
  }
  
    
}



void record ()
{

  X = CircuitPlayground.motionX();
  max_x = getMax(X,max_x);
  Y = CircuitPlayground.motionY();
  max_y = getMax(Y,max_y);
  Z = CircuitPlayground.motionZ();
  max_z = getMax(Z,max_z);
  mean_x += X;
  mean_y += Y;
  mean_z += Z;
  numofsamples++;
}


void SendToPhone()
{
    String xmaxstr =String(max_x);
    String ymaxstr = String(max_y);
    String zmaxstr = String(max_z);

    mean_x = mean_x/numofsamples;
    mean_y = mean_y/numofsamples;
    mean_z = mean_z/numofsamples;
    
    String xmeanstr = String(mean_x);
    String ymeanstr = String(mean_y);
    String zmeanstr = String(mean_z);
    
    String startsignal = String('o');
    String delimiters =String(',');
    String stopsignal = String ('x');

    String Send_string = startsignal+xmaxstr+delimiters+ymaxstr+delimiters+zmaxstr+delimiters+xmeanstr+delimiters+ymeanstr+delimiters+zmeanstr+stopsignal;
    ble.print(Send_string);
}


float getMax( float newX, float prev_max)
{
  if(newX > prev_max)
  {
    return newX;
  }
  else
  {
    return prev_max;
  }
}


void clearparameters()
{
      record_flag = 0;
      numofsamples = 0;
      max_x =0;
      max_y = 0;
      max_z =0;
      mean_x =0;
      mean_y =0;
      mean_z =0;
      send_flag = 0;
      CircuitPlayground.setPixelColor(8, 0,   0,   0);
      CircuitPlayground.setPixelColor(1, 0,   0,   0);
}



