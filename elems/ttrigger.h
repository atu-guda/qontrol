/***************************************************************************
                          ttrigger.h  -  description
                             -------------------
    begin                : Sun Sep 3 2000
    copyright            : (C) 2000-2014 by atu
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
   Q_CLASSINFO( "enum_TriggType_0", "RS"               ); // tr_rs
   Q_CLASSINFO( "enum_TriggType_1", "Shmitt(in_x)"     ); // tr_shmitt
   Q_CLASSINFO( "enum_TriggType_2", "CountRise(in_x)"  ); // tr_crise
   Q_CLASSINFO( "enum_TriggType_3", "CountDown(in_x)"  ); // tr_cdown
   Q_CLASSINFO( "enum_TriggType_4", "CountLevel(in_x)" ); // tr_clev   
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of trigger */
   PRM_LIST( type, efNRC, "Type", "Type of trigger", "enum=TriggType" );
   /**  current state */
   PRM_INT( cst, efInner, "state", "current state", "" );
   /** level of 0  */
   PRM_DOUBLE( level0, 0, "Level of 0", "Level of '0' signal", "def=0.1" );
   /** level of 1 (shmidt) */
   PRM_DOUBLE( level1, 0, "Level of 1", "Level of '1' signal (for shmidt)", "def=0.5"  );
   /** autoreset time */
   PRM_DOUBLE( t0, 0, "t0", "Autoreset time (if enabled)", "def=2" );
   /** elapsed time */
   PRM_DOUBLE( et, efInner, "et", "Elapsed time", "" );
   /** set to 1 on start */
   PRM_SWITCH( useInit1, efNRC, "Set on start", "Set triggers value to 1 of start", "sep=col" );
   /** use u[3] as enable */
   PRM_SWITCH( useEnable, efNRC, "use Enable", "Use in_ena as enable signal", "" );
   /** pulse output */
   PRM_SWITCH( usePulse, efNRC, "Pulse output",  "Output is pulse, no level", "sep=col" );
   /** use -1 as negative output */
   PRM_SWITCH( useMinus, efNRC, "Negative pulse", "Drop pulse is negative, not 0", "" );
   /** autoreset after t0 */
   PRM_SWITCH( useT0, efNRC, "Autoreset (t0)", "Use autoreset after t0 time elapsed", "");

   PRM_INPUT( in_r, 0, "R input", "Reset input",  "sep=block" );
   PRM_INPUT( in_s, 0, "S input", "Set input", "sep=col" );
   PRM_INPUT( in_x, 0, "X input", "X input",  "sep=col" );
   PRM_INPUT( in_ena, 0, "enable in", "enable input", "sep=col" );

   /** old u[2] value */
   double u2_old = 0;

   DCL_DEFAULT_STATIC;
};



#endif


