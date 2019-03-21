#ifndef ESAT_ThermalPayloadSubsystem_h
#define ESAT_ThermalPayloadSubsystem_h

#include <Arduino.h>
#include <ESAT_OnBoardDataHandling.h>

class ESAT_ThermalPayloadSubsystemClass: public ESAT_Subsystem
{
  public:
    // Start this subsystem.
    void begin();

    // Return the identifier of this subsystem.
    word getApplicationProcessIdentifier();

    // Handle a telecommand.
    void handleTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telecommand packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet);

    // Fill a packet with the next telemetry packet available.
    // Return true if the operation was successful;
    // otherwise return false.
    boolean readTelemetry(ESAT_CCSDSPacket& packet);

    // Deprecated method; don't use it.
    // Return true if there is new telemetry available;
    // Otherwise return false.
    boolean telemetryAvailable() __attribute__((deprecated));

    // Update the subsystem.
    void update();

   // Send a telemetry packet to this subsystem.
    void writeTelemetry(ESAT_CCSDSPacket& packet);

};

extern boolean UseThermalPayload;

// Global instance of ESAT_ThermalPayloadSubsystemClass.  Register
// ESAT_ThermalPayloadSubsystem on the on-board data handling module with
// ESAT_OnBoardDataHandling.registerSubsystem(ESAT_ThermalPayloadSubsystem).
extern ESAT_ThermalPayloadSubsystemClass ESAT_ThermalPayloadSubsystem;

#endif /* ESAT_ThermalPayloadSubsystem_h */
