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

#include "ESAT_OnBoardDataHandling.h"
#include <ESAT_KISSStream.h>
#include <USBSerial.h>

ESAT_OnBoardDataHandlingClass::ESAT_OnBoardDataHandlingClass():
  numberOfSubsystems(0),
  telecommandIndex(0),
  telemetryIndex(0)
{
}

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
  for (unsigned int i = 0; i < numberOfSubsystems; ++i)
  {
    const word subsystemsApplicationProcessIdentifier =
      subsystems[i]->getApplicationProcessIdentifier();
    if (subsystemsApplicationProcessIdentifier
        == primaryHeader.applicationProcessIdentifier)
    {
      subsystems[i]->handleTelecommand(packet);
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
  usbTelecommandDecoder = ESAT_KISSStream(USB,
                                          usbTelecommandBuffer,
                                          usbTelecommandBufferLength);
}

void ESAT_OnBoardDataHandlingClass::enableUSBTelemetry()
{
  usbTelemetryEnabled = true;
}

boolean ESAT_OnBoardDataHandlingClass::readTelecommand(ESAT_CCSDSPacket& packet)
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
  packet.clear();
  while (telemetryIndex < numberOfSubsystems)
  {
    if (subsystems[telemetryIndex]->telemetryAvailable())
    {
      packet.clear();
      const boolean successfulRead =
        subsystems[telemetryIndex]->readTelemetry(packet);
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
      telemetryIndex = telemetryIndex + 1;
    }
  }
  return false;
}

void ESAT_OnBoardDataHandlingClass::registerSubsystem(ESAT_Subsystem& subsystem)
{
  subsystems[numberOfSubsystems] = &subsystem;
  numberOfSubsystems = numberOfSubsystems + 1;
}

void ESAT_OnBoardDataHandlingClass::updateSubsystems()
{
  for (int i = 0; i < numberOfSubsystems; ++i)
  {
    subsystems[i]->update();
  }
  telecommandIndex = 0;
  telemetryIndex = 0;
}

void ESAT_OnBoardDataHandlingClass::writeTelemetry(ESAT_CCSDSPacket& packet)
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

void ESAT_OnBoardDataHandlingClass::writeTelemetryToUSB(ESAT_CCSDSPacket& packet)
{
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  const unsigned long encoderBufferLength =
    ESAT_KISSStream::frameLength(packet.length());
  byte encoderBuffer[encoderBufferLength];
  ESAT_KISSStream encoder(USB, encoderBuffer, encoderBufferLength);
  (void) encoder.beginFrame();
  (void) packet.writeTo(encoder);
  (void) encoder.endFrame();
}

ESAT_OnBoardDataHandlingClass ESAT_OnBoardDataHandling;
