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
#include <ESATOnBoardDataHandling.h>
#include <ESATTimer.h>
#include <MspFlash.h>
#include <SD.h>
#include <SPI.h>
#include <USBSerial.h>
#include <Servo.h>

class ESATExampleSubsystem: public ESATSubsystem
{
public:
  virtual void begin()
  {
  }

  virtual byte getStartOrder()
  {
    return 5;
  }

  virtual byte getSubsystemIdentifier()
  {
    return 5;
  }

  virtual void handleCommand(byte commandCode, String parameters)
  {
  }

  virtual String readTelemetry()
  {
    return "";
  }

  virtual void update()
  {
  }
};

ESATExampleSubsystem ExampleSubsystem;

void setup()
{
  OnBoardDataHandling.registerDefaultSubsystems();
  OnBoardDataHandling.registerSubsystem(&ExampleSubsystem);
  OnBoardDataHandling.beginSubsystems();
  Timer.begin(1000);
}

void loop()
{
  Timer.waitUntilNextCycle();
  ESATCommand command = OnBoardDataHandling.readCommand();
  if (command.valid)
  {
    OnBoardDataHandling.sendTelemetry("ACK",
                                      OnBoardDataHandling.EVENT_TELEMETRY,
                                      OBCSubsystem.getSubsystemIdentifier());
    OnBoardDataHandling.dispatchCommand(command.subsystemIdentifier,
                                        command.commandCode,
                                        command.parameters);
  }
  OnBoardDataHandling.updateSubsystems();
  String telemetry = OnBoardDataHandling.readSubsystemsTelemetry();
  if (OBCSubsystem.storeTelemetry)
  {
    OnBoardDataHandling.storeTelemetry(telemetry);
  }
  OnBoardDataHandling.sendTelemetry(telemetry,
                                    OnBoardDataHandling.HOUSEKEEPING_TELEMETRY,
                                    OBCSubsystem.getSubsystemIdentifier());
}
