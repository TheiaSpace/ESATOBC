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

void ESAT_OnBoardDataHandlingClass::disableUSBTelecommands()
{
  usbReader = ESAT_CCSDSPacketFromKISSFrameReader();
}

void ESAT_OnBoardDataHandlingClass::disableUSBTelemetry()
{
  usbWriter = ESAT_CCSDSPacketToKISSFrameWriter();
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
  usbReader = ESAT_CCSDSPacketFromKISSFrameReader(Serial,
                                                  buffer,
                                                  bufferLength);
}

void ESAT_OnBoardDataHandlingClass::enableUSBTelemetry()
{
  usbWriter = ESAT_CCSDSPacketToKISSFrameWriter(Serial);
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
  return usbReader.read(packet);
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
  (void) usbWriter.unbufferedWrite(packet);
}

ESAT_OnBoardDataHandlingClass ESAT_OnBoardDataHandling;
