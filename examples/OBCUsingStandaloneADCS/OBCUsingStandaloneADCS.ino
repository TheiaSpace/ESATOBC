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

#include <ESAT_StandaloneADCSSubsystem.h>
#include <ESAT_EPSSubsystem.h>
#include <ESAT_KISSStream.h>
#include <ESAT_OBCSubsystem.h>
#include <ESAT_WifiSubsystem.h>
#include <ESAT_OnBoardDataHandling.h>
#include <ESAT_Timer.h>
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
class ESAT_ExampleSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start the subsystem.
    void begin()
    {
    }

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process
    // identifier.
    // ESAT_OnBoardDataHandling uses this to identify the subsystem.
    word getApplicationProcessIdentifier()
    {
      return 4;
    }

    // Handle a telecommand.
    // ESAT_OnBoardDataHandling.dispatchTelecommand() forwards
    // telecommands to their respective ESATSubsystem modules,
    // identified by the value returned by
    // getApplicationProcessIdentifier().
    void handleTelecommand(ESAT_CCSDSPacket& packet)
    {
    }

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from ESAT_OnBoardDataHandling.readTelecommand().
    boolean readTelecommand(ESAT_CCSDSPacket& packet)
    {
      return false;
    }

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful; otherwise return
    // false.  Called from
    // ESAT_OnBoardDataHandling.readSubsystemsTelemetry().
    boolean readTelemetry(ESAT_CCSDSPacket& packet)
    {
      return false;
    }

    // Return true if there is new telemetry available;
    // Otherwise return false.
    // Called from ESAT_OnBoardDataHandling.readSubsystemsTelemetry().
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
    // Called from ESAT_OnBoardDataHandling.writeTelemetry().
    void writeTelemetry(ESAT_CCSDSPacket& packet)
    {
    }
};

// Unique global instance of ESAT_ExampleSubsystemClass.
ESAT_ExampleSubsystemClass ESAT_ExampleSubsystem;

// Cycle period in milliseconds.
const word PERIOD = 1000;

// Maximum packet data length we will handle.
const word PACKET_DATA_BUFFER_LENGTH = 256;

// Maximum whole packet length we will handle.
const word WHOLE_PACKET_BUFFER_LENGTH =
  ESAT_CCSDSPrimaryHeader::LENGTH + PACKET_DATA_BUFFER_LENGTH;

// Accumulate incoming USB telecommands in this buffer.
byte usbTelecommandBuffer[WHOLE_PACKET_BUFFER_LENGTH];

// Accumulate incoming Wifi telecommands in this buffer.
byte wifiTelecommandBuffer[WHOLE_PACKET_BUFFER_LENGTH];

// LED pin
static const byte LED = 60;

// Start peripherals and do the initial bookkeeping here:
// - Activate the reception of telecommands from the USB interface.
// - Activate the emission of telemetry through the USB interface.
// - Register the available subsystems for use by the on-board data
//   handling module.
// - Begin the subsystems.
// - Begin the timer that keeps a precise timing of the main loop.
// This is the first function of the program to be run at it runs only
// once.
void setup()
{
  Serial.begin(9600);
  USB.begin();
  Wire.begin();
  SD.begin(SS1);
  delay(1000);
  ESAT_OBCSubsystem.begin();
  ESAT_EPSSubsystem.begin();
  ESAT_StandaloneADCSSubsystem.begin();
  ESAT_WifiSubsystem.begin(wifiTelecommandBuffer,
                           sizeof(wifiTelecommandBuffer));
  ESAT_ExampleSubsystem.begin();
  ESAT_OnBoardDataHandling.enableUSBTelecommands(usbTelecommandBuffer,
                                                 sizeof(usbTelecommandBuffer));
  ESAT_OnBoardDataHandling.enableUSBTelemetry();
  ESAT_OnBoardDataHandling.registerSubsystem(ESAT_OBCSubsystem);
  ESAT_OnBoardDataHandling.registerSubsystem(ESAT_EPSSubsystem);
  ESAT_OnBoardDataHandling.registerSubsystem(ESAT_StandaloneADCSSubsystem);
  ESAT_OnBoardDataHandling.registerSubsystem(ESAT_WifiSubsystem);
  ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ExampleSubsystem);
  ESAT_Timer.begin(PERIOD);
  pinMode(LED,OUTPUT);
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
  ESAT_Timer.waitUntilNextCycle();
  byte buffer[PACKET_DATA_BUFFER_LENGTH];
  ESAT_CCSDSPacket packet(buffer, sizeof(buffer));
  while (ESAT_OnBoardDataHandling.readTelecommand(packet))
  {
    ESAT_OnBoardDataHandling.dispatchTelecommand(packet);
  }
  ESAT_OnBoardDataHandling.updateSubsystems();
  while (ESAT_OnBoardDataHandling.readSubsystemsTelemetry(packet))
  {
    ESAT_OnBoardDataHandling.writeTelemetry(packet);
  }
  digitalWrite(LED,!digitalRead(LED));
}
