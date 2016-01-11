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

#ifndef TTRIGGER_H
#define TTRIGGER_H

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
   Q_ENUMS(TriggType);
   Q_CLASSINFO( "enum_TriggType_0", "RS"            ); // tr_rs
   Q_CLASSINFO( "enum_TriggType_1", "Shmitt(X)"     ); // tr_shmitt
   Q_CLASSINFO( "enum_TriggType_2", "CountRise(X)"  ); // tr_crise
   Q_CLASSINFO( "enum_TriggType_3", "CountDown(X)"  ); // tr_cdown
   Q_CLASSINFO( "enum_TriggType_4", "CountLevel(X)" ); // tr_clev   
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   PRM_LIST( type, efNRC, "Type", "Type of trigger", "enum=TriggType" );
   PRM_INT( cst, efInner, "state", "current state", "" );
   PRM_PARAMD( level0, 0, "Level of 0", "Level of '0' signal", "def=0.1" );
   PRM_PARAMD( level1, 0, "Level of 1", "Level of '1' signal (for shmidt)", "def=0.5"  );
   PRM_PARAMD( t0, 0, "t0", "Autoreset time (if enabled)", "def=2" );

   PRM_DOUBLE( et, efInner, "et", "Elapsed time", "" );
   PRM_SWITCH( useInit1, efNRC, "Set on start", "Set triggers value to 1 of start", "sep=col" );
   PRM_SWITCH( useEnable, efNRC, "use Enable", "Use in_ena as enable signal", "" );
   PRM_SWITCH( usePulse, efNRC, "Pulse output",  "Output is pulse, no level", "sep=col" );
   PRM_SWITCH( useMinus, efNRC, "Negative pulse", "Drop pulse is negative, not 0", "" );
   PRM_SWITCH( useT0, efNRC, "Autoreset (t0)", "Use autoreset after t0 time elapsed", "");

   PRM_INPUT( in_r, 0, "R", "Reset input",  "sep=block" );
   PRM_INPUT( in_s, 0, "S", "Set input", "sep=col" );
   PRM_INPUT( in_x, 0, "X", "X input",  "sep=col" );
   PRM_INPUT( in_ena, 0, "EN", "enable input", "sep=col" );

   /** old u[2] value */
   double u2_old = 0;

   Q_CLASSINFO( "nameHintBase",  "trig_" );
   DCL_DEFAULT_STATIC;
};



#endif


