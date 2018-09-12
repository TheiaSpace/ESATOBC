/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
 *
 * This file is part of Theia Space's ESAT OBC library.
 *
 * Theia Space's ESAT OBC library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT OBC library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT OBC library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESAT_OBCLinesTelemetry.h"
#include "ESAT_OBCClock.h"
#include "ESAT_OBCSubsystem.h"
#include "ESAT_TelemetryStorage.h"
#include <ESAT_Timer.h>
#include <SPI.h>
#include <Wire.h>

boolean ESAT_OBCLinesTelemetryClass::available()
{
  return true;
}

byte ESAT_OBCLinesTelemetryClass::packetIdentifier()
{
  return PACKET_IDENTIFIER;
}

boolean ESAT_OBCLinesTelemetryClass::fillUserData(ESAT_CCSDSPacket& packet)
{
  const byte userDataLength = 45;
  if (packet.capacity() < (ESAT_CCSDSSecondaryHeader::LENGTH + userDataLength))
  {
    return false;
  }
  packet.writeWord(analogRead(ADC12));
  packet.writeWord(analogRead(ADC13));
  packet.writeWord(analogRead(ADC14));
  packet.writeWord(analogRead(ADC15));
  packet.writeBoolean(boolean(digitalRead(ESPRST)));
  packet.writeBoolean(boolean(digitalRead(ESP_SLEEP)));
  packet.writeBoolean(boolean(digitalRead(ESP0)));
  packet.writeBoolean(boolean(digitalRead(GPIO0)));
  packet.writeBoolean(boolean(digitalRead(GPIO1)));
  packet.writeBoolean(boolean(digitalRead(GPIO2)));
  packet.writeWord(analogRead(CSSXMINUS));
  packet.writeWord(analogRead(CSSXPLUS));
  packet.writeWord(analogRead(CSSYMINUS));
  packet.writeWord(analogRead(CSSYPLUS));
  packet.writeBoolean(boolean(digitalRead(ENMTQX)));
  packet.writeBoolean(boolean(digitalRead(ENMTQY)));
  packet.writeBoolean(boolean(digitalRead(MTQX)));
  packet.writeBoolean(boolean(digitalRead(MTQY)));
  packet.writeBoolean(boolean(digitalRead(PWM)));
  packet.writeBoolean(boolean(digitalRead(TCH)));
  packet.writeBoolean(boolean(digitalRead(EMG)));
  packet.writeBoolean(boolean(digitalRead(SCL_O)));
  packet.writeBoolean(boolean(digitalRead(SDA_O)));
  packet.writeBoolean(boolean(digitalRead(CLK_O)));
  packet.writeBoolean(boolean(digitalRead(CS_SD)));
  packet.writeBoolean(boolean(digitalRead(CS_O)));
  packet.writeBoolean(boolean(digitalRead(MISO_O)));
  packet.writeBoolean(boolean(digitalRead(MOSI_O)));
  packet.writeBoolean(boolean(digitalRead(TEST_O)));
  packet.writeBoolean(boolean(digitalRead(RST_O)));
  packet.writeBoolean(boolean(digitalRead(LED_O)));
  packet.writeWord(analogRead(TEMPSENSOR));
  packet.writeWord(analogRead(VCC_2));
  // The I2C bus stops working after reading the lines directly,
  // so we have to reset it.
  SPI.begin();
  // The I2C bus stops working after reading the lines directly,
  // so we have to reset it.
  Wire.begin();
  return true;
}

ESAT_OBCLinesTelemetryClass ESAT_OBCLinesTelemetry;
