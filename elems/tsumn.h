/***************************************************************************
                          tsumn.h  - N-input linear summator
                             -------------------
    begin                : 2015.10.31
    copyright            : (C) 2015-2015 by atu
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

#ifndef TSUMN_H
#define TSUMN_H

#include "tmiso.h"

/** N-input summator, like TLinear, with common factor
  *@author atu
  */

class TSumN : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSumN);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;

   PRM_DOUBLE( a, 0, "a", "common factor", "def=1\nsep=colend" );
   PRM_DOUBLE( b,  0, "b", "static shift", "sep=blockend" );

   QVector<InputSimple*> inputs;

   Q_CLASSINFO( "nameHintBase",  "sumn_" );
   DCL_DEFAULT_STATIC;
};

#endif
