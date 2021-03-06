#pragma once
/***************************************************************************
                          tdelay.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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


#include "tmiso.h"
#include "circbuf.h"


/**delays signal for given time
  *@author atu
  */

class TDelay : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TDelay);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   // virtual int do_preRun() override;
   // virtual int do_postRun( int good ) override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( cdelay,     0, "&Delay",     "Current delay", "min=0\nmax=1e20\ndef=1.0" );
   PRM_PARAMD( mdelay, efNRC, "Max. delay", "Maximum delay", "min=0\nmax=1e20\ndef=1.1\nsep=col" );

   PRM_DOUBLE( aver, efInner, "avarage",    "Average value", "" );

   PRM_INPUT( in_u,        0, "&in_u",      "Input",  "sep=block" );

   double v1 = 0;
   double v2 = 0;
   /** max and current delays indexes ... */
   size_t imd = 0, icd = 0;
   /** data container */
   TCircBuf buf;

   Q_CLASSINFO( "nameHintBase",  "dly_" );
   DCL_DEFAULT_STATIC;
};



