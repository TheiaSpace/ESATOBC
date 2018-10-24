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

#include "ESAT_OBC-telemetry/ESAT_OBCLinesTelemetry.h"

boolean ESAT_OBCLinesTelemetryClass::available()
{
  // The OBC lines telemetry packet is always available.
  return true;
}

boolean ESAT_OBCLinesTelemetryClass::fillUserData(ESAT_CCSDSPacket& packet)
{
  // This packet contains readings of all lines with the exception of
  // those managed by hardware communication device peripherals (the
  // I2C, serial and SPI interfaces), which cannot be read directly
  // without invalidating the state of the communication device
  // peripherals.
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
  packet.writeBoolean(boolean(digitalRead(CS_SD)));
  packet.writeBoolean(boolean(digitalRead(CS_O)));
  packet.writeBoolean(boolean(digitalRead(TEST_O)));
  packet.writeBoolean(boolean(digitalRead(RST_O)));
  packet.writeBoolean(boolean(digitalRead(LED_O)));
  packet.writeWord(analogRead(TEMPSENSOR));
  packet.writeWord(analogRead(VCC_2));
  // This packet is valid in general, except when it is truncated
  // because its capacity was too small.
  if (packet.triedToWriteBeyondCapacity())
  {
    return false;
  }
  else
  {
    return true;
  }
}

ESAT_OBCLinesTelemetryClass ESAT_OBCLinesTelemetry;
