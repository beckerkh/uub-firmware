*** ../../../LSv0r9b0p0/Src/gpsctrl/gpsio.c	Thu May  1 20:32:30 2003
--- gpsio.c	Sat Oct 25 11:22:26 2003
***************
*** 135,138 ****
--- 135,152 ----
  
      if ( IOState==4 ) {
+       /* PSA - added because Motorola's protocol is awful - the gps sn contains CR/LF pairs
+ 	 everywhere. You just need to know that it's 294 bytes */
+ 
+ #define GPS_SN_CUTOFF 290
+ 
+       if (IOBuffer[2] == 'C' && IOBuffer[3] == 'j')
+ 	{
+ 	  if (IOCurrent < GPS_SN_CUTOFF)
+ 	    {
+ 	      IOState = 2;
+ 	      i = IOCurrent;
+ 	      continue;
+ 	    }
+ 	}
        /*fprintf( stderr, "<<<<< ioread: found after %d [%x,%d]\n",
  	IOCurrent, CurInBuff, CharInBuff ) ;*/
