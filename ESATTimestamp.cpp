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
 
  
 #include "ESATTimestamp.h"
 #include <ESATUtil.h>
 #include <string.h>
 
 
 ESATTimestamp::ESATTimestamp()
{
  hours = 0;
  minutes = 0;
  seconds = 0;
  year = 0;
  month = 0;
  day = 0;  
}

void ESATTimestamp::update(byte Year, byte Month, byte Day, 
                          byte Hour, byte Minute, byte Second)
{
  hours = Hour;
  minutes = Minute;
  seconds = Second;
  year = Year;
  month = Month;
  day = Day;  
}

byte ESATTimestamp::update(char time[])
{
  int theYear, theMonth, theDay, theHours, theMinutes, theSeconds;
  int n = sscanf(time, "20%2u-%2u-%2uT%2u:%2u:%2u",&theYear, &theMonth, &theDay, &theHours, &theMinutes, &theSeconds);
  if(n < 6)
  {
    return INVALID_TIMESTAMP;
  }
  year = (byte)theYear;
  month = (byte)theMonth;
  day = (byte)theDay;
  hours = (byte)theHours;
  minutes = (byte)theMinutes;
  seconds = (byte)theSeconds;
  return VALID_TIMESTAMP;
}
void ESATTimestamp::update(ESATTimestamp timestamp)
{
  year = timestamp.year;
  month = timestamp.month;
  day = timestamp.day;
  hours = timestamp.hours;
  minutes = timestamp.minutes;
  seconds = timestamp.seconds;
}

void ESATTimestamp::incrementDay(){
  day ++;
  if(day > 31)
  {
    day = 1;
    month ++;
    if(month > 12)
    {
      month = 1;
      year ++;
    }
  }
  hours = 0;
  minutes = 0;
  seconds = 0;
}


byte ESATTimestamp::compare(ESATTimestamp timestamp)
{
  if(timestamp.year > year)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.year < year)
  {
    return THIS_IS_HIGHER;
  }
  
  if(timestamp.month > month)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.month < month)
  {
    return THIS_IS_HIGHER;
  }
  
  if(timestamp.day > day)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.day < day)
  {
    return THIS_IS_HIGHER;
  }
  
  if(timestamp.hours > hours)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.hours < hours)
  {
    return THIS_IS_HIGHER;
  }
  
  if(timestamp.minutes > minutes)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.minutes < minutes)
  {
    return THIS_IS_HIGHER;
  }
  
  if(timestamp.seconds > seconds)
  {
    return THIS_IS_LOWER;
  }
  else if(timestamp.seconds < seconds)
  {
    return THIS_IS_HIGHER;
  }
  else
  {
    return THIS_IS_EQUAL;
  }
}


boolean ESATTimestamp::operator==(ESATTimestamp timestamp)
{
  if(compare(timestamp) == THIS_IS_EQUAL)
  {
    return true;
  }
  else
  {
    return false;
  }
}
boolean ESATTimestamp::operator>(ESATTimestamp timestamp)
{
   if(compare(timestamp) == THIS_IS_HIGHER)
  {
    return true;
  }
  else
  {
    return false;
  }
}
boolean ESATTimestamp::operator<(ESATTimestamp timestamp)
{
  if(compare(timestamp) == THIS_IS_LOWER)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ESATTimestamp::toStringTimeStamp(char timestamp[])
{
  sprintf(timestamp, "20%02u-%02u-%02uT%02u:%02u:%02u", year % 100, month % 100, day % 100, hours % 100, minutes % 100, seconds % 100);
}

void ESATTimestamp::getDateWithoutDashes(char timestamp[]){
  sprintf(timestamp, "20%02u%02u%02u", year % 100, month % 100, day % 100);
}