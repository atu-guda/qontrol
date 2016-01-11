/***************************************************************************
                          tsourcelin.h  -  description
                             -------------------
    begin                : Thu Aug 24 2003
    copyright            : (C) 2003-2016 by atu
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

#ifndef TSOURCELIN_H
#define TSOURCELIN_H

#include "tmiso.h"

/** This element provide source, which consist of
  * linear elements.
  * @author atu
  */

constexpr int def_n_slopes=16; // keep with init strings

class TSourceLin : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSourceLin);
   DCL_CREATE;
   DCL_STD_INF;
   /** called before each inner param loop. Unused param - -1 */
   void recalc(void);
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** times of lines, <=0 - end */
   virtual int do_startLoop( int acnx, int acny ) override;

   PRM_PARAMD( a,  0, "a", "Common factor", "sep=col\ndef=1" );

   // time 0 means end of sequence - to start
   PRM_DOUBLE_ARR( t_int, efNoRunChange, "t_int", "Time intervals", "N=16\ndef=0\nmin=0\ndefs=1 1 1  1  1 0\nsep=tab\ntabname=Arrays" );
   PRM_DOUBLE_ARR( vs, efNoRunChange, "vs", "start value",                 "N=16\ndef=0\ndefs=0 0 1 -1 -1 0\nsep=col" );
   PRM_DOUBLE_ARR( ve, efNoRunChange, "ve", "end value",                   "N=16\ndef=0\ndefs=0 1 1 -1  0 0\nsep=col" );

   /** slope values of each line */
   dvector slopes;
   /** number of lines, current line */
   int n_lin = 0, curr_lin = 0;
   /** start time of current line */
   double t_start = 0;

   Q_CLASSINFO( "nameHintBase",  "solin_" );
   DCL_DEFAULT_STATIC;
};

#endif

