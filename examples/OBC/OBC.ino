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
#include <ESATEPSSubsystem.h>
#include <ESATOBCSubsystem.h>
#include <ESATWifiSubsystem.h>
#include <ESATOnBoardDataHandling.h>
#include <ESATTimer.h>
#include <USBSerial.h>
#include <SD.h>
#include <Wire.h>

// Main program of the on-board computer.  It performs some initial
// peripheral setup and bookkeeping in setup() and then it runs the
// main on-board data handling loop.

// The on-board data handling (OnBoardDataHandling) module operates on
// ESATSubsystem objects which are a common interface to subsystem
// functionality from the point of view of the on-board data handling
// function.

// Example subsystem.  Use it as the skeleton of your own subsystem.
class ESATExampleSubsystem: public ESATSubsystem
{
  public:
    // Start the subsystem.
    // Called from OnBoardDataHandling.beginSubsystems().
    void begin()
    {
    }

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process
    // identifier.
    // OnBoardDataHandling uses this to identify the subsystem.
    word getApplicationProcessIdentifier()
    {
      return 4;
    }

    // Handle a telecommand.
    // OnBoardDataHandling.dispatchTelecommand() forwards telecommands
    // to their respective ESATSubsystem modules, identified by the
    // value returned by getApplicationProcessIdentifier().
    void handleTelecommand(ESATCCSDSPacket& packet)
    {
    }

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readTelecommand().
    boolean readTelecommand(ESATCCSDSPacket& packet)
    {
      return false;
    }

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    boolean readTelemetry(ESATCCSDSPacket& packet)
    {
      return false;
    }

    // Return true if there is new telemetry available;
    // Otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    boolean telemetryAvailable()
    {
      return false;
    }

    // Update the subsystem.
    // Called from OnBoardDataHandling.updateSubsystems().
    void update()
    {
    }

    // Send a telemetry packet to this subsystem.
    // Called from OnBoardDataHandling.writeTelemetry().
    void writeTelemetry(ESATCCSDSPacket& packet)
    {
    }
};

// Unique global instance of ESATExampleSubsystem.
ESATExampleSubsystem ExampleSubsystem;

// Start peripherals and do the initial bookkeeping here:
// - Register the available subsystems for use by the on-board data
//   handling module.
// - Begin the subsystems.
// - Begin the timer that keeps a precise timing of the main loop.
// This is the first function of the program to be run at it runs only
// once.
void setup()
{
  Serial.begin(115200);
  USB.begin();
  Wire.begin();
  SD.begin(SS1);
  OnBoardDataHandling.registerSubsystem(OBCSubsystem);
  OnBoardDataHandling.registerSubsystem(EPSSubsystem);
  OnBoardDataHandling.registerSubsystem(ADCSSubsystem);
  OnBoardDataHandling.registerSubsystem(WifiSubsystem);
  OnBoardDataHandling.registerSubsystem(ExampleSubsystem);
  OnBoardDataHandling.beginSubsystems();
  Timer.begin(1000);
}

// Body of the main loop of the program:
// - Retrieve the incoming telecommands.
// - Dispatch the incoming telecommands on their target subsystems.
// - Update the subsystems.
// - Retrieve the telemetry packets from the subsystems.
// - Forward the retrieved telemetry packets to the subsystems so that
//   they can use them (for example, a subsystem may send telemetry
//   packets to the ground station or it can store them for later use).
// This function is run in an infinite loop that starts after setup().
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
