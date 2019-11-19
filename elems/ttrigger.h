#pragma once
/***************************************************************************
                          ttrigger.h  -  description
                             -------------------
    begin                : Sun Sep 3 2000
    copyright            : (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <tmiso.h>

/**different kinds if triggers
  *@author atu
  */

class TTrigger : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TTrigger);
   DCL_CREATE;
   DCL_STD_INF;

   enum TriggType {
     tr_rs = 0, tr_shmitt, tr_crise, tr_cdown, tr_clev
   };
   Q_ENUM(TriggType);
   Q_CLASSINFO( "enum_TriggType_0", "RS"            ); // tr_rs
   Q_CLASSINFO( "enum_TriggType_1", "(X)"           ); // tr_shmitt -- really in_x
   Q_CLASSINFO( "enum_TriggType_2", "CountRise(X)"  ); // tr_crise
   Q_CLASSINFO( "enum_TriggType_3", "CountDown(X)"  ); // tr_cdown
   Q_CLASSINFO( "enum_TriggType_4", "CountLevel(X)" ); // tr_clev   
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_LIST(     type, efNRC, "&Type", "Type of trigger", "enum=TriggType" );
   PRM_PARAMD(     t0,     0, "t0",    "Autoreset time (if enabled)", "def=2" );

   PRM_SWITCH( useEnable, efNRC, "use Enable", "Use in_ena as enable signal", "sep=col" );
   PRM_SWITCH( usePulse,  efNRC, "&Pulse output",  "Output is pulse, no level", "sep=col" );
   PRM_SWITCH( useMinus,  efNRC, "&Negative pulse", "Drop pulse is negative, not 0", "" );
   PRM_SWITCH( useT0,     efNRC, "&Autoreset (t0)", "Use autoreset after t0 time elapsed", "");

   PRM_DOUBLE( et, efInner, "et",    "Elapsed time",  "" );
   PRM_INT(   cst, efInner, "state", "current state", "" );

   PRM_LOGICIN( in_r,      0, "&R",  "Reset input",    "sep=block" );
   PRM_LOGICIN( in_s,      0, "&S",  "Set input",      "sep=col" );
   PRM_LOGICIN( in_x,      0, "&X",  "X input",        "sep=col" );
   PRM_LOGICIN( in_ena,    0, "&EN", "'enable' input", "sep=col" );

   double u2_old = 0;

   Q_CLASSINFO( "nameHintBase",  "trig_" );
   DCL_DEFAULT_STATIC;
};





