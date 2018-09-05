Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid

This file is part of Theia Space's ESAT OBC library.

Theia Space's ESAT OBC library is free software: you can
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Theia Space's ESAT OBC library is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Theia Space's ESAT OBC library.  If not, see
<http://www.gnu.org/licenses/>.


On-Board Computer (OBC) for ESAT.

See the example OBC programs.


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
