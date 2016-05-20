#pragma once
/***************************************************************************
                          circbuf.h - circular buffer class description
                             -------------------
    begin                : 2015.01.14 (move from miscfun.h)
    copyright            : (C) 2015-2016 by atu
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

#include "defs.h"
// ------------- miscelanios classes -----------------------------

class TCircBuf {
 public:
   explicit TCircBuf( unsigned nn );
   TCircBuf( const TCircBuf &r ) = default;
   ~TCircBuf();
   TCircBuf& operator=( const TCircBuf &r ) = default;
   void reset();
   void resize( unsigned n ); // reset implied
   void add( double a );
   int getN() const { return nf; }
   double operator[]( int i ) const;
   double sum() const { return su; }
   double aver() const { return (nf>0) ? ( su / nf ) : 0; }
   double sumCalc(); // force recalc sum
 protected:
   unsigned nb; //* buffer size
   unsigned s;  //* start index
   unsigned nf; //* number of inserted points [0;nb-1]
   unsigned ni; //* number of insertion after sum recalc
   double su;   //* current sum
   std::vector<double> d;
   static const constexpr unsigned recalc_after = 10000;
};

