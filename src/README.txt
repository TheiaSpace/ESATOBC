On-Board Computer (OBC) for ESAT.

See the example OBC program.


# ESAT_OnBoardDataHandling

On-Board Data Handling (OBDH), which is the main function of the OBC.
This library provides the general OBDH library functionality.


# ESAT_Subsystem

The OBDH library manages subsystems by means of objects that implement
the ESAT_Subsystem interface.


# ESAT_ADCSSubsystem

Attitude Determination and Control Subsystem (library backend).  A
wrapper to the ESAT_ADCS library running in the On-Board Computer.


# ESAT_EPSSubsystem

Electrical Power Subsystem.  Communicates with the EPS board.


# ESAT_OBCSubsystem

On-Board Computer subsystem.


# ESAT_StandaloneADCSSubsystem

Attitutde Determination and Control Subsystem (standalone backend).
Communicates with the standalone ADCS board.


# ESAT_WifiSubsystem

Wifi communications subsystem.  Communicates with the Wifi board.


# ESAT_OBCClock

Access to the real-time clock mounted on the OBC board.


# ESAT_TelemetryStorage

Access to the memory card mounted on the OBC board for telemetry
storage.
