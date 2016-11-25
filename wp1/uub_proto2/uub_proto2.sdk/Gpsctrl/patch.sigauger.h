*** ../../../LSv0r9b0p0/Defs/sigauger.h	Sun Apr 20 20:25:51 2003
--- ../../Defs/sigauger.h	Sat Oct 25 19:57:25 2003
***************
*** 104,107 ****
--- 104,112 ----
  #define SIG_GPS_RESET 3020
  #define SIG_GPS_RESTART 3021
+ /* Request GPS SN - PSA 25/10/2003 */
+ #define SIG_GPS_SN_GET 3022
+ /* Send GPS SN back to CDAS - PSA 25/10/2003 */
+ #define SIG_GPS_SN_SEND 3023
+ 
  /* Send a generic message including UTM position */
  #define SIG_GPS_SEND_POSITION 4000
