/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Printers.h>
#include <XBee.h>
#include <SoftwareSerial.h>

SoftwareSerial XSerial(2, 3); //RX, TX
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

int dataLed_1 = 5;
int dataLed_2 = 9;
int dataLed_3 = 11;
int errorLed = 12;

uint8_t option = 0;
uint8_t data1 = 0;
uint8_t data2 = 0;
uint8_t data3 = 0;

String stringW, stringX, stringY;

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

void setup() {
//  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed_1,  OUTPUT);
  pinMode(dataLed_2,  OUTPUT);
  pinMode(dataLed_3,  OUTPUT);
  
  // start serial
  XSerial.begin(9600);
  Serial.begin(9600);
  xbee.setSerial(XSerial);
  
  flashLed(errorLed, 3, 50);
}

// continuously reads packets, looking for RX16 or RX64
void loop() {
    
    xbee.readPacket();
    stringW = String("");
    stringX = String("");
    stringY = String("");
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                xbee.getResponse().getRx16Response(rx16);
                option = rx16.getOption();
                
                stringW += char(rx16.getData(3));
                stringW += char(rx16.getData(4));
                stringW += char(rx16.getData(5));
                stringW += char(rx16.getData(6));
                stringW += char(rx16.getData(7));
                stringW += char(rx16.getData(8));
                stringW += char(rx16.getData(9));
                stringW += char(rx16.getData(10));
                data1 = map(stringW.toFloat() * 10000, -10000, 10000, 0, 255);
                analogWrite(dataLed_1, data1);
                Serial.println("W : ");
                Serial.println(data1);
                
                stringX += char(rx16.getData(18));
                stringX += char(rx16.getData(19));
                stringX += char(rx16.getData(20));
                stringX += char(rx16.getData(21));
                stringX += char(rx16.getData(22));
                stringX += char(rx16.getData(23));
                stringX += char(rx16.getData(24));
                stringX += char(rx16.getData(25));
                data2 = map(stringX.toFloat() * 10000, -10000, 10000, 0, 255);
                analogWrite(dataLed_2, data2);
                Serial.println("X : ");
                Serial.println(data2);
        	
                stringY += char(rx16.getData(33));
                stringY += char(rx16.getData(34));
                stringY += char(rx16.getData(35));
                stringY += char(rx16.getData(36));
                stringY += char(rx16.getData(37));
                stringY += char(rx16.getData(38));
                stringY += char(rx16.getData(39));
                stringY += char(rx16.getData(40));
                data3 = map(stringY.toFloat() * 10000, -10000, 10000, 0, 255);
                analogWrite(dataLed_3, data3); 
                Serial.println("Y : ");
                Serial.println(data3);
                
        } else {
                xbee.getResponse().getRx64Response(rx64);
        	option = rx64.getOption();
                
        }
      } else {
      	// not something we were expecting
        flashLed(errorLed, 1, 25);
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
    } 
}
