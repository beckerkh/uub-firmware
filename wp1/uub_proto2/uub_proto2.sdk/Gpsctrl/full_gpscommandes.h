#if !defined(_GPSCOMMANDES_H_)

#define _GPSCOMMANDES_H_


/***************************************************************************/
/* QUELQUES COMMANDES (MOTOROLA BINARY FORMAT)*/

#if defined(MAIN)

comm C_heure={"Time of Day",
  "@@Aa",
  10,10};

comm C_ap_type={"Application Type ",
  "@@AB",
  8,8};

comm C_date={"Date ",
  "@@Ac",
  11,11};

comm C_lat={"Latitude",
            "@@Ad",
            11,11};

comm C_lon={"Longitude",
            "@@Ae",
            11,11};

comm C_align={"Align output ",
  "@@AE",
  8,8};

comm C_height={"Receiver height",
       "@@Af",
       12,15};

comm C_mask_angle={"Satellite mask angle",
  "@@Ag",
  8,8};

comm C_satsel_option={"Satellite select Option (Automatic/Manual/Highest-In-Sky",
  "@@Ah",
  8,8};

comm C_satsel={"Assignement de satellite au canal",
  "@@Ai",
  9,9};

comm C_ignore={"satellite ignore list",
  "@@Am",
  12,12};

comm C_datum={"Demande Parametres Geoide",
  "@@Ao",
  8,25};

comm C_ion={"Correction ionospherique (enable/Disable)",
  "@@Aq",
  8,8};

comm C_position_holdp={"Position Hold Parameters LAT LON HGT msl/gps",
  "@@As",
  20,20};

comm C_position_hold={"Position Hold Option (Enable/Disable) ",
  "@@At",
  8,8};

comm C_corr_utc={"Correction UTC (enable/disable)",
  "@@Aw",
  8,8};

comm C_epoch_offset={"Epoch Offset Option ",
  "@@Ax",
  9,9};

comm C_pps_time_offset={"1pps Time Offset Option)",
  "@@Ay",
  11,11};

comm C_ppsdelay={"Compensation du delai de cable d'antenne)",
  "@@Az",
  11,11};

comm C_pos6={"Demande de position 6 canaux",
  "@@Ba",
  8,68};

comm C_visiblesat= {"Visiblite Satellite",
    "@@Bb",
    8,92};

comm C_almoutout={"output almanach", /* reponse dans Cb */
  "@@Be",
  8,33};

comm C_satrange= {"Satellite range",
  "@@Bg",
  8,122};

comm C_broadmess= {"Broadcast message",
  "@@Bl",
  8,41};

comm C_Traim6={"time RAIM Setup 6 canaux",
  "@@Bn",
  22,59};

comm C_almoutin={"output psd correction", /* en reponse a Be */
  "@@Cb",
  8,33};

comm C_outpsdcor={"output psd correction", /* en reponse a Bh */
  "@@Ce",
  7,52};

comm C_set_default={"Set to Default !",
  "@@Cf",
  7,7};

comm C_position_fix={"Position Fix/Idle mode ",
  "@@Cg",
  8,8};

comm C_id=  {"ID message",
 "@@Cj",
 7,294};

comm C_pos8={"Demande de position 8 canaux",
 "@@Ea",
  8,76};

comm C_satrange8= {"8 Satellite range",
 "@@Eg",
  8,158};

comm C_extpos8={"Extended  position 8 canaux",
 "@@Ek",
  8,71};

comm C_Traim8={"Time RAIM Setup 8 canaux",
 "@@En",
  22,69};

comm C_measure={"Measure ",        /* Fake message to allow recording of counter data */
 "@@Es",
  32,32};

#else

extern comm C_heure ;

extern comm C_ap_type ;
extern comm C_date ;
extern comm C_lat ;
extern comm C_lon ;
extern comm C_align ;
extern comm C_height ;
extern comm C_mask_angle ;
extern comm C_satsel_option ;
extern comm C_satsel ;
extern comm C_ignore ;
extern comm C_datum ;
extern comm C_ion ;
extern comm C_position_holdp ;
extern comm C_position_hold ;
extern comm C_corr_utc ;
extern comm C_epoch_offset ;
extern comm C_pps_time_offset ;
extern comm C_ppsdelay ;
extern comm C_pos6 ;
extern comm C_visiblesat ;
extern comm C_almoutout ;
extern comm C_satrange ;
extern comm C_broadmess ;
extern comm C_Traim6 ;
extern comm C_almoutin ;
extern comm C_outpsdcor ;
extern comm C_set_default ;
extern comm C_position_fix ;
extern comm C_id ;
extern comm C_pos8 ;
extern comm C_satrange8 ;
extern comm C_extpos8 ;
extern comm C_Traim8 ;
extern comm C_measure ;

#endif

#endif
