/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_OnBoardDataHandling.h"
#include <ESAT_KISSStream.h>

void ESAT_OnBoardDataHandlingClass::disableUSBTelecommands()
{
  usbTelecommandsEnabled = false;
}

void ESAT_OnBoardDataHandlingClass::disableUSBTelemetry()
{
  usbTelemetryEnabled = false;
}

void ESAT_OnBoardDataHandlingClass::dispatchTelecommand(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  for (ESAT_Subsystem* subsystem = firstSubsystem;
       subsystem != nullptr;
       subsystem = subsystem->nextSubsystem)
  {
    const word subsystemsApplicationProcessIdentifier =
      subsystem->getApplicationProcessIdentifier();
    if (subsystemsApplicationProcessIdentifier
        == primaryHeader.applicationProcessIdentifier)
    {
      subsystem->handleTelecommand(packet);
      return;
    }
  }
}

void ESAT_OnBoardDataHandlingClass::enableUSBTelecommands(byte buffer[],
                                                          const unsigned long bufferLength)
{
  usbTelecommandsEnabled = true;
  usbTelecommandBuffer = buffer;
  usbTelecommandBufferLength = bufferLength;
  usbTelecommandDecoder = ESAT_KISSStream(Serial,
                                          usbTelecommandBuffer,
                                          usbTelecommandBufferLength);
}

void ESAT_OnBoardDataHandlingClass::enableUSBTelemetry()
{
  usbTelemetryEnabled = true;
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  while (telecommandSubsystem != nullptr)
  {
    const bool successfulRead =
      telecommandSubsystem->readTelecommand(packet);
    if (successfulRead)
    {
      packet.rewind();
      return true;
    }
    else
    {
      telecommandSubsystem = telecommandSubsystem->nextSubsystem;
    }
  }
  if (usbTelecommandsEnabled)
  {
    return readTelecommandFromUSB(packet);
  }
  (void) packet;
  return false;
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommandFromUSB(ESAT_CCSDSPacket& packet)
{
  const boolean gotFrame = usbTelecommandDecoder.receiveFrame();
  if (!gotFrame)
  {
    return false;
  }
  const boolean gotPacket = packet.readFrom(usbTelecommandDecoder);
  if (!gotPacket)
  {
    return false;
  }
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return false;
  }
  return true;
}

boolean ESAT_OnBoardDataHandlingClass::readSubsystemsTelemetry(ESAT_CCSDSPacket& packet)
{
  while (telemetrySubsystem != nullptr)
  {
    if (telemetrySubsystem->telemetryAvailable())
    {
      packet.flush();
      const boolean successfulRead =
        telemetrySubsystem->readTelemetry(packet);
      packet.rewind();
      const ESAT_CCSDSPrimaryHeader primaryHeader =
        packet.readPrimaryHeader();
      if (successfulRead
          && (primaryHeader.packetType == primaryHeader.TELEMETRY))
      {
        return true;
      }
    }
    else
    {
      telemetrySubsystem = telemetrySubsystem->nextSubsystem;
    }
  }
  return false;
}

void ESAT_OnBoardDataHandlingClass::registerSubsystem(ESAT_Subsystem& subsystem)
{
  if (lastSubsystem == nullptr)
  {
    firstSubsystem = &subsystem;
    lastSubsystem = &subsystem;
  }
  else
  {
    lastSubsystem->nextSubsystem = &subsystem;
    lastSubsystem = &subsystem;
  }
}

void ESAT_OnBoardDataHandlingClass::updateSubsystems()
{
  for (ESAT_Subsystem* subsystem = firstSubsystem;
       subsystem != nullptr;
       subsystem = subsystem->nextSubsystem)
  {
    subsystem->update();
  }
  telecommandSubsystem = firstSubsystem;
  telemetrySubsystem = firstSubsystem;
}

void ESAT_OnBoardDataHandlingClass::writeTelemetry(ESAT_CCSDSPacket& packet)
{
  for (ESAT_Subsystem* subsystem = firstSubsystem;
       subsystem != nullptr;
       subsystem = subsystem->nextSubsystem)
  {
    packet.rewind();
    subsystem->writeTelemetry(packet);
  }
  if (usbTelemetryEnabled)
  {
    packet.rewind();
    writeTelemetryToUSB(packet);
  }
}

void ESAT_OnBoardDataHandlingClass::writeTelemetryToUSB(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  ESAT_KISSStream encoder(Serial);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

ESAT_OnBoardDataHandlingClass ESAT_OnBoardDataHandling;
