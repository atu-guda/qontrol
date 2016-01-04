/***************************************************************************
                          tlatch.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

   enum LatchType {
     latchTime = 0, latchSignal
   };
   Q_ENUMS(LatchType);
   Q_CLASSINFO( "enum_LatchType_0", "Time" );        // latchTime
   Q_CLASSINFO( "enum_LatchType_1", "in_latch" );   // latchSignal

 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of */
   PRM_LIST( type, efNRC, "Type", "Latch type", "enum=LatchType" );
   /** time to latch */
   PRM_DOUBLE( t0, efNRC, "Time", "Time to latch" , "sep=col" );
   /** current value */
   PRM_DOUBLE( v, efInner, "v", "Current value", "" );
   /** old input  */
   double u_old = 0;
   /** flags, state */
   PRM_SWITCH( usePulse, efNRC, "use Pulse", "latch on jump of (in_latch >0.5), not level", "sep=col" );
   PRM_SWITCH( useFirst, efNRC, "use First", "count only first signal to latch", "" );
   PRM_SWITCH( useAdd,   efNRC, "use Add", "add current in_u to value, not set", "" );
   PRM_DOUBLE( lt, efInner, "Latch Time", "Time whan latch occurs" , "" );

   PRM_INPUT( in_u, 0, "in_u", "Main input",  "sep=block" );
   PRM_INPUT( in_latch, 0, "latch", "Latch input", "sep=col" );

   int wasLatch = -1;

   Q_CLASSINFO( "nameHintBase",  "latch_" );
   DCL_DEFAULT_STATIC;
};

#endif


