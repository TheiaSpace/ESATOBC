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
 
#include <Energia.h>

#ifndef ESATTimestamp_h
#define ESATTimestamp_h

class ESATTimestamp
{
  public:
    // the timestamp length + 1
    static const byte charTimestampLength = 20;
    // the date length + 1
    static const byte charDateLength = 9;
    ESATTimestamp();
    void update(byte Year, byte Month, byte Day, 
                byte Hour, byte Minute, byte Second);
    byte update(char timestamp[]);
    void update(ESATTimestamp timeStamp);
    void incrementDay();
    byte compare(ESATTimestamp timestamp);
    boolean operator>(ESATTimestamp timeStamp);
    boolean operator>=(ESATTimestamp timeStamp);
    boolean operator<(ESATTimestamp timeStamp);
    boolean operator<=(ESATTimestamp timeStamp);
    boolean operator==(ESATTimestamp timeStamp);
    void toStringTimeStamp(char timestamp[]);    
    // util to get the date from the timestamp
    void getDateWithoutDashes(char date[]);
    byte hours;
    byte minutes;
    byte seconds;
    byte year;
    byte month;
    byte day;
    static const byte INVALID_TIMESTAMP = 1;
    static const byte VALID_TIMESTAMP = 2;
  private:  
    static const byte THIS_IS_HIGHER = 1;
    static const byte THIS_IS_LOWER = 2;
    static const byte THIS_IS_EQUAL = 3;

    
};

#endif
