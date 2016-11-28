*** ../../../LSv0r9b0p0/Src/gpsctrl/gpsprocess.c	Wed Mar 12 09:57:14 2003
--- gpsprocess.c	Sat Oct 25 12:18:37 2003
***************
*** 131,135 ****
--- 131,141 ----
  }
  
+ const char *gps_serial_string = "SERIAL #   ";
+ 
  void traiteC() {
+   /*
+    *  PSA: Change for serial number output to gps.sn
+    */
+   FILE *fout;
  
  #if defined(GPSCTRL_DEBUG)
***************
*** 137,143 ****
--- 143,180 ----
    if ( IOBuffer[3] == 'j' ) {
      int i = 0 ;
+     char *tempbuff, *snptr;
+ 
+     if (!(fout = fopen("gps.sn", "w")))
+       {
+ 	PrintLog(LOG_ERROR, " Can't open gps.sn for writing!\n");
+ 	IOCurrent=0;
+ 	IOState=0;
+ 	return;
+       }
+     /*
      for( ; i < IOCurrent ; i++ ) fputc( IOBuffer[i], stderr ) ;
+     */
+     for(;i<IOCurrent;i++) fputc(IOBuffer[i], fout);
+     fclose(fout);
+     tempbuff = (char *) malloc(sizeof(char)*IOCurrent+1);
+     memcpy(tempbuff, IOBuffer, IOCurrent);
+     tempbuff[IOCurrent] = 0x0; /* Null terminate so strstr has a string */
+     snptr = strstr(tempbuff, gps_serial_string);
+     if (snptr)
+       {
+ 	snptr += strlen(gps_serial_string);
+ 	snptr[NUM_GPS_SN_CHARS] = 0x0;
+ 	strncpy(CurrGPSSn, snptr, NUM_GPS_SN_CHARS);
+ 	PrintLog(LOG_INFO, "  GPS Serial Number is %s\n", CurrGPSSn);
+       }
+     else
+       {
+ 	PrintLog(LOG_ERROR, "  Couldn't find GPS serial no. in string!\n");
+       }
+     free(tempbuff);
    }
+   /*
    if ( Debug ) SendGenericMsg( "Gpsctrl: Got C msg", Pproc->mbx_out, 0 ) ;
+   */
  #endif
    IOCurrent = 0 ;
