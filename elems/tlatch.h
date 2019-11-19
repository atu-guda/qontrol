#pragma once
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
   Q_ENUM(LatchType);
   Q_CLASSINFO( "enum_LatchType_0", "Time" );        // latchTime
   Q_CLASSINFO( "enum_LatchType_1", "in_latch" );   // latchSignal

 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_LIST( type, efNRC, "&Type", "Latch type", "enum=LatchType" );
   PRM_PARAMD( t0, 0, "Ti&me", "Time to latch" , "sep=col" );

   PRM_DOUBLE( v, efInner, "v", "Current value", "" );

   /** flags, state */
   PRM_SWITCH( useFirst, efNRC, "use &First", "count only first signal to latch", "sep=col" );
   PRM_SWITCH( useAdd,   efNRC, "use Add", "add current in_u to value, not set", "" );

   PRM_DOUBLE( lt, efInner, "Latch Time", "Time whan latch occurs" , "" );

   PRM_INPUT(       in_u, 0, "&in_u",      "Main input",  "sep=block" );
   PRM_LOGICIN( in_latch, 0, "in_{latch}", "Latch input", "sep=col" );

   bool wasLatch { false };

   Q_CLASSINFO( "nameHintBase",  "latch_" );
   DCL_DEFAULT_STATIC;
};



