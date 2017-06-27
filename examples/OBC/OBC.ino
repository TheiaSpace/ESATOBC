/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ESATADCS.h>
#include <ESATADCSSubsystem.h>
#include <ESATCommand.h>
#include <ESATCOMMSSubsystem.h>
#include <ESATEPSSubsystem.h>
#include <ESATOBCSubsystem.h>
#include <ESATSubsystem.h>
#include <ESATSubsystemManager.h>
#include <ESATTelemetryManager.h>
#include <ESATTimer.h>
#include <MspFlash.h>
#include <SD.h>
#include <SPI.h>
#include <USBSerial.h>
#include <Servo.h>

void setup()
{
  SubsystemManager.registerDefaultSubsystems();
  SubsystemManager.beginSubsystems();
  Timer.begin(1000);
}

void loop()
{
  Timer.waitUntilNextCycle();
  ESATCommand command = ESATSubsystemManager.readCommand();
  if (command.valid)
  {
    TelemetryManager.send("ACK",
                          TelemetryManager.EVENT_TELEMETRY,
                          COMMSSubsystem.getSubsystemIdentifier());
    SubsystemManager.dispatchCommand(command.subsystemIdentifier,
                                     command.commandCode,
                                     command.parameters);
  }
  SubsystemManager.updateSubsystems();
  String telemetry = SubsystemManager.readSubsystemsTelemetry();
  if (OBCSubsystem.storeTelemetry)
  {
    TelemetryManager.store(telemetry);
  }
  TelemetryManager.send(telemetry,
                        TelemetryManager.HOUSEKEEPING_TELEMETRY,
                        OBCSubsystem.getSubsystemIdentifier());
}
