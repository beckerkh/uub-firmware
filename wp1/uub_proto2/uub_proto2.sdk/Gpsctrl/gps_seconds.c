/********************************************************

#! /bin/bash

# Philip Tuckey 9 March 2001

# This routine takes a GPS date and time in the form of six integers:
# y M d h m s
# and calculates the number of seconds elapsed between this date and the
# beginning of the GPS epoch.

# (The beginning of) The GPS Epoch is the transition from 23:59:59 UTC on
# Saturday January 5 to 00:00:00 UTC on Sunday January 6th 1980. Thus the
# GPS time 1980 1 6 0 0 0 corresponds to zero seconds elapsed.

# NOTE THAT THE SIMPLIFIED LEAP YEAR CHECKS MEAN THIS ROUTINE IS ONLY GOOD
# UP TO 2099.

# The routine also prints the number of full days elapsed since the
# beginning of the GPS epoch, that is, 6 Jan 1980 corresponds to zero days
# elapsed.

y=$1; M=$2; d=$3; h=$4; m=$5; s=$6

# No. of days in each month. (Don't use nm[0].)
nm=(0 31 0 31 30 31 30 31 31 30 31 30 31)
if [ $((y%4)) = 0 ]; then nm[2]=29; else nm[2]=28; fi

# No. of days in full months elapsed within the current year.
sum=0; i=1
while [ $i -lt $M ]; do
    sum=$((sum+nm[i]))
    i=$((i+1))
done

# Add days elapsed within the current month, correct for 6 Jan start, add
# days in full years elapsed, correct for intervening leap years.
days=$((sum + d - 6 + 365*(y-1980) + (y-1980+3)/4))
echo days=$days

# And now the rest is easy...
secs=$((s + 60*(m + 60*(h + 24*days))))
echo secs=$secs


**********************************************************/

#include <stdio.h>
#include <stdlib.h>

static int Nm[] = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
} ;

unsigned int
gps_seconds( int yy, int mm, int dd, int hh, int mn, int ss )
{
  int sum =0, i = 1, days, leap = 0 ;

  if ( (yy % 4) == 0 ) leap = 1 ;
  for ( ; i < mm ; i++ ) {
    sum += Nm[i] ;
    if ( i == 2 && leap ) sum++ ;
  }

  days = sum + dd - 6 + 365*(yy - 1980) + (yy - 1980 + 3)/4 ;
  return ss + 60*(mn + 60*(hh + 24*days)) ;
}
