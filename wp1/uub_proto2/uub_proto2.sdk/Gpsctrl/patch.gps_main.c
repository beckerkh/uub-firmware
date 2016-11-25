*** ../../../LSv0r9b0p0/Src/gpsctrl/gps_main.c	Thu May  1 20:41:14 2003
--- gps_main.c	Sat Oct 25 12:21:36 2003
***************
*** 478,481 ****
--- 478,525 ----
  }
  
+ /*
+  * This is the message that needs to be sent to the
+  * GPS to request the serial number, in hex.
+  * There's no need to constantly recalculate it
+  * since it's the same, once and always.
+  */
+ 
+ u_int8 sn_msg[] = {0x40,0x40,0x43,0x6A,0x29,0xD,0xA,'\0'};
+ u_int32 sn_msg_len = 7;
+ 
+ /**************************
+   PSA - 25/10/2003
+   Request GPS serial number and write it to gps.sn
+ **************************/
+ 
+ void DumpGpsSerial()
+ {
+   error_code ok;
+   PrintLog(LOG_INFO, " Requesting GPS serial number.\n");
+   *CurrGPSSn = 0x0;
+   ok = compatWrite(GpsPathOut, sn_msg, &sn_msg_len);
+   if (ok != SUCCESS ) {
+     PrintLog(LOG_ERROR, "Cant write to GPS\n" );
+   }
+ }
+ 
+ /**************************
+   PSA - 25/10/2003
+   Send GPS serial to CDAS
+ **************************/
+ 
+ #define GPS_SN_STR_LEN 32
+ 
+ void SendGpsSerial()
+ {
+   char *gpssnstring;
+ 
+   gpssnstring = (char *) malloc(sizeof(char)*GPS_SN_STR_LEN);
+ 
+   sprintf(gpssnstring, "GPS SN: %s", CurrGPSSn);
+   SendGenericMsg(gpssnstring, Pproc->mbx_out, GENERIC_MSG_VERSION);
+   PrintLog(LOG_INFO, "Sending GPS SN: %s\n", CurrGPSSn);
+ }
+   
  main( int argc, char **argv )
  {
***************
*** 499,503 ****
  
    InitGps() ;
! 
  #if GPSCTRL_VERSION<7
    SetGpsState() ;
--- 543,551 ----
  
    InitGps() ;
!   /*
!    * PSA v0r9b2p4. Dump serial number on boot, and when requested
!    * via SIG_GPS_SN. Writes to "gps.sn".
!    */
!   DumpGpsSerial();
  #if GPSCTRL_VERSION<7
    SetGpsState() ;
***************
*** 609,612 ****
--- 657,671 ----
        case SIGINT: byebye( sig ) ;
  	break ;
+       case SIG_GPS_SN_GET:
+ 	/*
+ 	 * PSA v0r9b2p4. Dump serial number on boot, and when requested
+ 	 * via SIG_GPS_SN_GET. Writes to "gps.sn".
+ 	 * CDAS can request the GPS SN via SIG_GPS_SN_SEND.
+ 	 */
+ 	DumpGpsSerial();
+ 	break;
+       case SIG_GPS_SN_SEND:
+ 	SendGpsSerial();
+ 	break;
        default: PrintLog( LOG_INFO, " Unexpected signal %d\n", sig ) ;
        }
