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

#include "ESATOnBoardDataHandling.h"
#include <ESATKISSStream.h>
#include <USBSerial.h>

ESATOnBoardDataHandling::ESATOnBoardDataHandling():
  numberOfSubsystems(0),
  telecommandIndex(0),
  telemetryIndex(0)
{
}

void ESATOnBoardDataHandling::beginSubsystems()
{
  telemetryIndex = 0;
  for (unsigned i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->begin();
  }
}

void ESATOnBoardDataHandling::disableUSBTelecommands()
{
  usbTelecommandsEnabled = false;
}

void ESATOnBoardDataHandling::disableUSBTelemetry()
{
  usbTelemetryEnabled = false;
}

void ESATOnBoardDataHandling::dispatchTelecommand(ESATCCSDSPacket& packet)
{
  const word applicationProcessIdentifier =
    packet.readApplicationProcessIdentifier();
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    const word subsystemsApplicationProcessIdentifier =
      subsystems[i]->getApplicationProcessIdentifier() & 0x07FF;
    if (subsystemsApplicationProcessIdentifier == applicationProcessIdentifier)
    {
      subsystems[i]->handleTelecommand(packet);
      return;
    }
  }
}

void ESATOnBoardDataHandling::enableUSBTelecommands()
{
  usbTelecommandsEnabled = true;
}

void ESATOnBoardDataHandling::enableUSBTelemetry()
{
  usbTelemetryEnabled = true;
}

boolean ESATOnBoardDataHandling::readTelecommand(ESATCCSDSPacket& packet)
{
  while (telecommandIndex < numberOfSubsystems)
  {
    const bool successfulRead =
      subsystems[telecommandIndex]->readTelecommand(packet);
    if (successfulRead)
    {
      return true;
    }
    else
    {
      telecommandIndex = telecommandIndex + 1;
    }
  }
  if (usbTelecommandsEnabled)
  {
    return readTelecommandFromUSB(packet);
  }
}

boolean ESATOnBoardDataHandling::readTelecommandFromUSB(ESATCCSDSPacket& packet)
{
  if (USB.available() < 1)
  {
    return false;
  }
  const unsigned long bufferLength =
    packet.PRIMARY_HEADER_LENGTH + packet.packetDataBufferLength;
  byte buffer[bufferLength];
  ESATKISSStream input(USB, buffer, bufferLength);
  const boolean gotPacket = packet.readFrom(input);
  if (!gotPacket)
  {
    return false;
  }
  if (packet.readPacketType() != packet.TELECOMMAND)
  {
    return false;
  }
  return true;
}

boolean ESATOnBoardDataHandling::readSubsystemsTelemetry(ESATCCSDSPacket& packet)
{
  packet.clear();
  while (telemetryIndex < numberOfSubsystems)
  {
    if (subsystems[telemetryIndex]->telemetryAvailable())
    {
      const boolean successfulRead =
        subsystems[telemetryIndex]->readTelemetry(packet);
      if (successfulRead && (packet.readPacketType() == packet.TELEMETRY))
      {
        return true;
      }
    }
    else
    {
      telemetryIndex = telemetryIndex + 1;
    }
  }
  return false;
}

void ESATOnBoardDataHandling::registerSubsystem(ESATSubsystem& subsystem)
{
  subsystems[numberOfSubsystems] = &subsystem;
  numberOfSubsystems = numberOfSubsystems + 1;
}

void ESATOnBoardDataHandling::updateSubsystems()
{
  for (int i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->update();
  }
  telecommandIndex = 0;
  telemetryIndex = 0;
}

void ESATOnBoardDataHandling::writeTelemetry(ESATCCSDSPacket& packet)
{
  for (unsigned int i = 0; i < numberOfSubsystems; i++)
  {
    subsystems[i]->writeTelemetry(packet);
  }
  if (usbTelemetryEnabled)
  {
    writeTelemetryToUSB(packet);
  }
}

void ESATOnBoardDataHandling::writeTelemetryToUSB(ESATCCSDSPacket& packet)
{
  ESATKISSStream output(USB, nullptr, 0);
  (void) output.beginFrame();
  (void) packet.writeTo(output);
  (void) output.endFrame();
}

ESATOnBoardDataHandling OnBoardDataHandling;
