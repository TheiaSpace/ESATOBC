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

#include <ESATADCSSubsystem.h>
#include <ESATCOMMSSubsystem.h>
#include <ESATEPSSubsystem.h>
#include <ESATOBCSubsystem.h>
#include <ESATOnBoardDataHandling.h>
#include <ESATTimer.h>
#include <USBSerial.h>
#include <Wire.h>

class ESATExampleSubsystem: public ESATSubsystem
{
  public:
    void begin()
    {
    }

    word getApplicationProcessIdentifier()
    {
      return 4;
    }

    void handleTelecommand(ESATCCSDSPacket& packet)
    {
    }

    boolean readTelecommand(ESATCCSDSPacket& packet)
    {
      return false;
    }

    boolean readTelemetry(ESATCCSDSPacket& packet)
    {
      return false;
    }

    boolean telemetryAvailable()
    {
      return false;
    }

    void update()
    {
    }

    void writeTelemetry(ESATCCSDSPacket& packet)
    {
    }
};

ESATExampleSubsystem ExampleSubsystem;

void setup()
{
  Serial.begin(115200);
  USB.begin();
  Wire.begin();
  OnBoardDataHandling.registerSubsystem(OBCSubsystem);
  OnBoardDataHandling.registerSubsystem(EPSSubsystem);
  OnBoardDataHandling.registerSubsystem(ADCSSubsystem);
  OnBoardDataHandling.registerSubsystem(COMMSSubsystem);
  OnBoardDataHandling.registerSubsystem(ExampleSubsystem);
  OnBoardDataHandling.beginSubsystems();
  Timer.begin(1000);
}

void loop()
{
  Timer.waitUntilNextCycle();
  const word bufferLength = 256;
  byte buffer[bufferLength];
  ESATCCSDSPacket packet(buffer, bufferLength);
  while (OnBoardDataHandling.readTelecommand(packet))
  {
    OnBoardDataHandling.dispatchTelecommand(packet);
  }
  OnBoardDataHandling.updateSubsystems();
  while (OnBoardDataHandling.readSubsystemsTelemetry(packet))
  {
    OnBoardDataHandling.writeTelemetry(packet);
  }
}
