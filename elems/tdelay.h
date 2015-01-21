/***************************************************************************
                          tdelay.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2015 by atu
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

#ifndef TDELAY_H
#define TDELAY_H

#include <memory>

#include "tmiso.h"
class TCircBuf;


/**delays signal for given time
  *@author atu
  */

class TDelay : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TDelay);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TDelay() override;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** calc values & alloc buf */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** deallocates buf */
   virtual int do_postRun( int good ) override;
   /** reimplemented from TMiso to reset array */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** max delay */
   PRM_DOUBLE( mdelay, efNRC, "Max. delay", "Maximum delay", "min=0\nmax=1e20\ndef=1.5");
   /** current delay */
   PRM_DOUBLE( cdelay, 0, "Current delay", "Current delay", "min=0\nmax=1e20\ndef=1.0\nsep=col" );

   PRM_INPUT( in_u, 0, "input", "Single input",  "sep=block" );

   double v1 = 0;
   double v2 = 0;
   double u00 = 0;
   /** max and current delays indexes ... */
   int imd = 0, icd = 0;
   /** data container */
   std::unique_ptr<TCircBuf> buf;
   DCL_DEFAULT_STATIC;
};

#endif


