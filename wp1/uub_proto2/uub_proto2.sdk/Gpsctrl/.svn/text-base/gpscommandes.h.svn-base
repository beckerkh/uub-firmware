#if !defined(_GPSCOMMANDES_H_)

#define _GPSCOMMANDES_H_


/***************************************************************************/
/* QUELQUES COMMANDES (MOTOROLA BINARY FORMAT)*/

#if defined(MAIN)

comm C_heure={"",
  "@@Aa",
  10,10};  //Has been added to @@Gb command, which contains @@Ac data, then @@Aa data then GMT offset data, which is unused

comm C_ap_type={"",
  "@@AB",
  8,8};     //Completely unused, not listed in Oncore manual

comm C_date={"",
  "@@Ac",
  11,11}; //Has been added to @@Gb command

comm C_lat={"",
            "@@Ad",  //Has been added to @@Ga command, @@Ad + @@Ae + @@Af
            11,11};

comm C_lon={"",
            "@@Ae",
            11,11};    //Has been added to @@Ga

comm C_align={"",
  "@@AE",
  8,8};       //Not listed in Oncore Manual

comm C_height={"",
       "@@Af",
       12,15};       //Has been added to @@Ga

comm C_mask_angle={"",
  "@@Ag",
  8,8};

comm C_satsel_option={"",
  "@@Ah",
  8,8};     //Not listed in Oncore Manual

comm C_satsel={"",
  "@@Ai",
  9,9};      //Not listed in Oncore Manual

comm C_ignore={"",
  "@@Am",
  12,12};

comm C_datum={"",
  "@@Ao",
  8,25};

comm C_ion={"",
  "@@Aq",
  8,8};

comm C_position_holdp={"",
  "@@As",
  20,20};

#ifdef(OS9000)
comm C_position_hold={"",
  "@@At",
  8,8};
#else
comm C_position_hold={"",
  "@@Gd",
  8,8};
#endif

comm C_corr_utc={"",
  "@@Aw",
  8,8};

comm C_epoch_offset={"",
  "@@Ax",
  9,9};      //Not listed in Oncore Manual

comm C_pps_time_offset={"",
  "@@Ay",
  11,11};

comm C_ppsdelay={"",
  "@@Az",
  11,11};

comm C_pos6={"",
  "@@Ba",
  8,68};    //Needs to become @@Ha

comm C_visiblesat= {"",
    "@@Bb",
    8,92};

comm C_almoutout={"", /* reponse dans Cb */
  "@@Be",
  8,33};

comm C_satrange= {"",
  "@@Bg",
  8,122};        //Not listed in Oncore Manual

comm C_broadmess= {"",
  "@@Bl",
  8,41};

comm C_Traim6={"",
  "@@Bn",
  22,59};    //needs to be @@Hn

comm C_almoutin={"", /* en reponse a Be */
  "@@Cb",
  8,33};

comm C_outpsdcor={"", /* en reponse a Bh */
  "@@Ce",
  7,52};

comm C_set_default={"",
  "@@Cf",
  7,7};

comm C_position_fix={"",
  "@@Cg",
  8,8};       //Not listed in Oncore Manual

comm C_id=  {"",
 "@@Cj",
 7,294};

comm C_pos8={"",
 "@@Ea",
  8,76};     //Also @@Ha

comm C_pos12={"",
 "@@Ha",
  8,154};     //for m12m receiver

comm C_satrange8= {"",
 "@@Eg",
  8,158};    //Not listed in Oncore Manual

comm C_extpos8={"",
 "@@Ek",
  8,71};     //Not listed in Oncore Manual

comm C_Traim8={"",
 "@@En",
  22,69};   //Again, must be @@Hn

comm C_PPSEnable={"",
 "@@Gc",
  8,8};   //for m12m receiver

comm C_TraimEnable={"",
 "@@Ge",
  8,8};   //for m12m receiver

comm C_TraimAlarm={"",
 "@@Gf",
  9,9};   //for m12m receiver

comm C_Traim12={"",
 "@@Hn",
  8,78};   //for m12m receiver

comm C_measure={"",        /* Fake message to allow recording of counter data */
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
extern comm C_pos12 ;
extern comm C_satrange8 ;
extern comm C_extpos8 ;
extern comm C_Traim8 ;
extern comm C_Traim12 ;
extern comm C_measure ;
extern comm C_PPSEnable ;
extern comm C_TraimAlarm ;
extern comm C_TraimEnable ;
#endif

#endif
