/***************************************************************************
                          tlatch.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#ifndef TLATCH_H
#define TLATCH_H

#include <tmiso.h>

/**Remembers given value by signal
  *@author atu
  */

class TLatch : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLatch);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of */
   PRM_LIST( type, efNRC, "Type", "Latch type", "", "Time\nSignal u[1]");
   /** time to latch */
   PRM_DOUBLE( t0, efNRC, "Time", "Time to latch" , "sep=col" );
   /** start value */
   PRM_DOUBLE( v_st, efNRC, "Start val", "Start value", "" );
   /** current value */
   PRM_DOUBLE( v, efInner, "v", "Current value", "" );
   /** old input  */
   double u_old;
   /** flags, state */
   PRM_SWITCH( usePulse, efNRC, "use Pulse", "latch on jump of u[1] (>0.5), not level", "sep=col" );
   PRM_SWITCH( useFirst, efNRC, "use First", "count only first signal to latch", "" );
   PRM_SWITCH( useAdd,   efNRC, "use Add", "add current u[0] to value, not set", "" );

   PRM_INPUT( in_u, 0, "input", "Main input",  "sep=block" );
   PRM_INPUT( in_latch, 0, "latch in", "Latch input", "sep=col" );

   int wasLatch = -1;

   DCL_DEFAULT_STATIC;
};

#endif


