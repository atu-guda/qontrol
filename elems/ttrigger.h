/***************************************************************************
                          ttrigger.h  -  description
                             -------------------
    begin                : Sun Sep 3 2000
    copyright            : (C) 2000-2013 by atu
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

static const char *ttrigger_list = 
   "RS\n"             // 0
   "Shmitt(u2)\n"     // 1
   "CountRise(u2)\n"  // 2
   "CountDown(u2)\n"  // 3
   "CountLevel(u2)"   // 4 
;

/**different kinds if triggers
  *@author atu
  */

class TTrigger : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TTrigger);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** type of trigger */
   PRM_LIST( type, efNRC, "Type", "Type of trigger", "", ttrigger_list );
   /**  current state */
   PRM_INT( cst, efInner, "state", "current state", "" );
   /** level of 0  */ 
   PRM_DOUBLE( level0, 0, "Level of 0", "Level of '0' signal", "" );
   /** level of 1 (shmidt) */
   PRM_DOUBLE( level1, 0, "Level of 1", "Level of '1' signal (sor shmidt)", ""  );
   /** autoreset time */
   PRM_DOUBLE( t0, 0, "t0", "Autoreset time (if enabled)", "" );
   /** elapsed time */
   PRM_DOUBLE( et, efInner, "et", "Elapsed time", "" );
   /** set to 1 on start */
   PRM_SWITCH( useInit1, efNRC, "Set on start", "Set triggers value to 1 of start", "sep=col" );
   /** use u[3] as enable */
   PRM_SWITCH( useEnable, efNRC, "u3 is enable", "Use u[3] as enable signal", "" );
   /** pulse output */
   PRM_SWITCH( usePulse, efNRC, "Pulse output",  "Output is pulse, no level", "sep=col" );
   /** use -1 as negative output */
   PRM_SWITCH( useMinus, efNRC, "Negative pulse", "Drop pulse is negative, not 0", "" );
   /** autoreset after t0 */
   PRM_SWITCH( useT0, efNRC, "Autoreset (t0)", "Use autoreset after t0 time elapsed", "");
   /** old u[2] value */
   double u2_old;
   
   DCL_DEFAULT_STATIC;
};



#endif


