
#include "ESAT_ThermalPayloadSubsystem.h"

    boolean UseThermalPayload = true;
    // We have to keep track of some state.
    // Should we be blinking?
    boolean blink;

    // Was the LED switched on or off?
    boolean haveToPowerTheLedOn;
    
    // We decide which pin to use to drive the LED.
    // We will use GPIO1.
    const int PIN = GPIO1;
  
    void ESAT_ThermalPayloadSubsystemClass::begin()
    {
      // We need to set up the pin.
      pinMode(PIN, OUTPUT);
      // We will start with the LED switched off.
      digitalWrite(PIN, LOW);
      // We will start with blinking enabled.
      blink = true;
      // Let’s remember the next state of the LED (it will be
      // on because we start with the LED switched off).
      haveToPowerTheLedOn = true;
    }

    // Return the application process identifier of this subsystem.
    // Each subsystem must have a unique 11-bit application process
    // identifier.
    // OnBoardDataHandling uses this to identify the subsystem.
    word ESAT_ThermalPayloadSubsystemClass::getApplicationProcessIdentifier()
    {
      return 5;
    }

    // Handle a telecommand.
    // OnBoardDataHandling.dispatchTelecommand() forwards telecommands
    // to their respective ESATSubsystem modules, identified by the
    // value returned by getApplicationProcessIdentifier().
    void ESAT_ThermalPayloadSubsystemClass::handleTelecommand(ESAT_CCSDSPacket& packet)
    {
      // We will see how to handle a telecommand packet in coming sessions.
      // A command to stop and start the blinking of the LED would be
      // interesting, isn't it?
    }

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    boolean ESAT_ThermalPayloadSubsystemClass::readTelemetry(ESAT_CCSDSPacket& packet)
    {
      // Here we should fill a telemetry packet.
      // We will see how to do it in coming sessions.
      // At the moment we are going to show information through the
      // USB as a debugging tool.
      return false;
    }

    // Return true if there is new telemetry available;
    // Otherwise return false.
    // Called from OnBoardDataHandling.readSubsystemsTelemetry().
    boolean ESAT_ThermalPayloadSubsystemClass::telemetryAvailable()
    {
      // Nothing to do here.
      return false;
    }

    // Update the subsystem.
    // Called from OnBoardDataHandling.updateSubsystems().
    void ESAT_ThermalPayloadSubsystemClass::update()
    {
      // Toggle the LED on the update method, but only when we are told to blink.
      if (blink)
      {
        if (haveToPowerTheLedOn)
        {
          digitalWrite(PIN, HIGH);
          haveToPowerTheLedOn = false;
        }
        else
        {
          digitalWrite(PIN, LOW);
          haveToPowerTheLedOn = true;
        }
      }
      // We show the current value of blink
      Serial.println();
      Serial.print(String("LED status: "));
      if(blink == true)
      {
        Serial.print(String("blinking."));
      }
      else
      {
        Serial.print(String("idle."));
      }
      Serial.println();
      // if (ActivateThermalPayload == true) {
      //   Serial.print(String("thermal active"));
      // }
      
    }

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    // Called from OnBoardDataHandling.readTelecommand().
    boolean ESAT_ThermalPayloadSubsystemClass::readTelecommand(ESAT_CCSDSPacket& packet)
    {
      // Nothing to do here.
      return false;
    }

    // Send a telemetry packet to this subsystem.
    // Called from OnBoardDataHandling.writeTelemetry().
    void ESAT_ThermalPayloadSubsystemClass::writeTelemetry(ESAT_CCSDSPacket& packet)
    {
      // Nothing to do here.
    }


// Unique global instance of ESAT_ThermalPayloadSubsystemClass.
ESAT_ThermalPayloadSubsystemClass ESAT_ThermalPayloadSubsystem;