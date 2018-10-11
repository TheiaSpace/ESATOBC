/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
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
  // An empty CCSDS-packet-from-KISS-frame reader just fails to produce
  // packets, so a way to disable USB telecommands is to make the USB
  // reader an empty CCSDS-packet-from-KISS-frame reader.
  usbReader = ESAT_CCSDSPacketFromKISSFrameReader();
}

void ESAT_OnBoardDataHandlingClass::disableUSBTelemetry()
{
  // An empty CCSDS-packet-to-KISS-frame writer just fails to write
  // packets, so a way to disable USB telemetry is to make the USB
  // writer an empty CCSDS-packet-from-KISS-frame writer.
  usbWriter = ESAT_CCSDSPacketToKISSFrameWriter();
}

void ESAT_OnBoardDataHandlingClass::dispatchTelecommand(ESAT_CCSDSPacket& packet)
{
  // Send the telecommand packet to the first subsystem that matches
  // its application process identifier.  No two subsystems should
  // have the same application process identifier, so sending the packet
  // to the first match means sending it to the only match.
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
  {
    return;
  }
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
  // A non-empty CCSDS-packet-from-KISS-frame reader can produce
  // packets from its input Stream, so a way to enable USB
  // telecommands is to make the USB reader a non-empty
  // CCSDS-packet-from-KISS-frame reader.
  usbReader = ESAT_CCSDSPacketFromKISSFrameReader(Serial,
                                                  buffer,
                                                  bufferLength);
}

void ESAT_OnBoardDataHandlingClass::enableUSBTelemetry()
{
  // A non-empty CCSDS-packet-from-KISS-frame writer can write packets
  // to its input Stream, so a way to enable USB telemetry is to make
  // the USB writer a non-empty CCSDS-packet-from-KISS-frame writer.
  usbWriter = ESAT_CCSDSPacketToKISSFrameWriter(Serial);
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  // Subsystems are visited in a first-in, first-out basis, from the
  // first subsystem to the last subsystem.
  // The main program calls this method many times on each on-board
  // data handling cycle, so it is neccessary to keep track of the
  // currently-visited subsystem with telecommandSubsystem.
  // Each subsystem is asked for new telecommand packets until
  // it fails to produce a new telecommand packet; then it's the turn
  // of the next subsystem.
  // After the last subsystem has been visited, it's time to read
  // telecommands from the USB interface.  The USB reader will fail
  // to produce a telecommand if USB telecommands are disabled, so
  // it is correct to always ask it for a telecommand and let it
  // decide what to do.
  while (telecommandSubsystem != nullptr)
  {
    const boolean gotTelecommand =
      readTelecommandFromSubsystem(packet, *telecommandSubsystem);
    if (gotTelecommand)
    {
      return true;
    }
    else
    {
      telecommandSubsystem = telecommandSubsystem->nextSubsystem;
    }
  }
  return readTelecommandFromUSB(packet);
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommandFromSubsystem(ESAT_CCSDSPacket& packet,
                                                                    ESAT_Subsystem& subsystem)
{
  packet.flush();
  const boolean gotPacket = subsystem.readTelecommand(packet);
  if (gotPacket && packet.isTelecommand())
  {
    packet.rewind();
    return true;
  }
  else
  {
    packet.flush();
    return false;
  }
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommandFromUSB(ESAT_CCSDSPacket& packet)
{
  packet.flush();
  const boolean gotPacket = usbReader.read(packet);
  if (gotPacket && packet.isTelecommand())
  {
    packet.rewind();
    return true;
  }
  else
  {
    packet.flush();
    return false;
  }
}

boolean ESAT_OnBoardDataHandlingClass::readTelemetryFromSubsystem(ESAT_CCSDSPacket& packet,
                                                                  ESAT_Subsystem& subsystem)
{
  packet.flush();
  const boolean gotPacket = subsystem.readTelemetry(packet);
  if (gotPacket && packet.isTelemetry())
  {
    packet.rewind();
    return true;
  }
  else
  {
    packet.flush();
    return false;
  }
}

boolean ESAT_OnBoardDataHandlingClass::readSubsystemsTelemetry(ESAT_CCSDSPacket& packet)
{
  // Subsystems are visited in a first-in, first-out basis, from the
  // first subsystem to the last subsystem.
  // The main program calls this method many times on each on-board
  // data handling cycle, so it is neccessary to keep track of the
  // currently-visited subsystem with telemetrySubsystem.
  // Each subsystem is asked for new telemetry packets until it fails
  // to produce a new telemetry packet; then it's the turn of the next
  // subsystem.
  // The packet is rewound after the subsystem fills it so that
  // the main program can use it directly.
  while (telemetrySubsystem != nullptr)
  {
    const boolean gotTelemetry =
      readTelemetryFromSubsystem(packet, *telemetrySubsystem);
    if (gotTelemetry)
    {
      return true;
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
  // Subsystems are appended to the end of the list so that it is easy
  // to visit them in a first-in, first-out basis.
  // If the list is empty, then it is necessary to assign both the
  // first subsystem pointer and the last subsystem pointer; otherwise
  // it is necessary to update the last subsystem pointer.
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
  // Subsystems are updated in a first-in, first-out basis, from the
  // first subsystem to the last subsystem.
  // After that, the currently-visited subsystems of readTelecommand()
  // and readTelemetry() are assigned to the first subsystem so that
  // the next series of calls to those methods can work from the
  // start of the list.
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
  // The telemetry packe is written to the subsystems and to the USB
  // packet writer.  The packet is passed to the subsystems as a
  // reference, so the subsystems can modify the read/write pointer;
  // to ensure that it is at the start of the packet data, the packet
  // is rewound before passing it to each subsystem.
  // The USB writer will just drop the packet if USB telemetry output
  // is disabled, so it is correct to always pass it the packet and
  // let it decide what to do.
  if (!packet.isTelemetry())
  {
    return;
  }
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
