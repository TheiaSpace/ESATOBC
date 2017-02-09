/*
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

#include "ESATCOMMSSubsystem.h"
#include "ESATOBCSubsystem.h"
#include <ESATUtil.h>
#include <USBSerial.h>

void ESATCOMMSSubsystem::begin()
{
}

byte ESATCOMMSSubsystem::getStartOrder()
{
  return 2;
}

byte ESATCOMMSSubsystem::getSubsystemIdentifier()
{
  return 4;
}

void ESATCOMMSSubsystem::handleCommand(byte commandCode, String parameters)
{
  USB.println(parameters);
  int repetitions = 10;
  while (repetitions--)
  {
    USB.println(",");
    Serial.flush();
    Serial.println(parameters);
    int timeout = 20;
    while (timeout--)
    {
      delay(1000);
      USB.print(".");
      if (Serial.available())
      {
        String cmd = Serial.readStringUntil('\r');
        if (cmd.indexOf("#s:") >= 0)
        {
          if (cmd.indexOf("#s:3") >= 0)
          {
            USB.println("Connection Success");
            return;
          }
          else if (cmd.indexOf("#s:2") >= 0)
          {
            USB.println("Wifi:OK, Waiting for server");
          }
          else
          {
            USB.print("Wifi connection failed, Retrying");
          }
        }
      }
    }
  }
  USB.println("Connection error!");
}

ESATCommand ESATCOMMSSubsystem::readCommand()
{
  String packet;
  ESATCommand command;
  command.valid = false;
  if (Serial.available())
  {
    packet = Serial.readStringUntil('\r');
  }
  else if (USB.available())
  {
    packet = USB.readStringUntil('\r');
  }
  else
  {
    return command;
  }
  String identifier = packet.substring(0, 1);
  if (identifier == "@")
  {
    command.valid = true;
    command.subsystemIdentifier = packet.substring(2, 3).toInt();
    command.commandCode = strtol(packet.substring(5, 7).c_str(), 0, 16);
    const int commandLength = strtol(packet.substring(3, 5).c_str(), 0, 16);
    command.parameters = packet.substring(7, 7 + commandLength);
  }
  else if (packet.indexOf("#s:") >= 0)
  {
    status = packet.substring(4).toInt();
  }
  else if (identifier == "#")
  {
    command.valid = true;
    command.subsystemIdentifier = getSubsystemIdentifier();
    command.commandCode = 0;
    command.parameters = packet;
  }
  return command;
}

String ESATCOMMSSubsystem::readTelemetry()
{
  return Util.intToHexadecimal(status);
}

void ESATCOMMSSubsystem::update()
{
}

void ESATCOMMSSubsystem::writePacket(String packet)
{
  Serial.flush();
  Serial.print("@" + packet);
  USB.println("{\"type\":\"onPacket\",\"id\":\""
              + String(int(OBCSubsystem.identifier))
              + "\",\"data\":\""
              + String(int(OBCSubsystem.identifier), HEX).substring(0, 1)
              + packet
              +"\"}");
}

ESATCOMMSSubsystem COMMSSubsystem;
