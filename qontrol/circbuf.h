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
   using sz_type = dvector::size_type;
   explicit TCircBuf( sz_type nn, double v = 0 );
   TCircBuf( const TCircBuf &r ) = default;
   // ~TCircBuf();
   TCircBuf& operator=( const TCircBuf &r ) = default;
   void reset();
   void resize( sz_type n, double v = 0 ); // reset implied
   void add( double a );
   sz_type getN() const { return nf; }
   double at( sz_type i ) const;
   double operator[]( sz_type i ) const {
     ptrdiff_t j = (ptrdiff_t)s - i - 1;
     if( j < 0 ) { j += nb;  }
     return d[j];
   }
   double sum() const { return su; }
   double aver() const { return (nf>0) ? ( su / nf ) : 0; }
   double sumCalc(); // force recalc sum
 protected:
   sz_type nb; //* buffer size
   sz_type s;  //* start index
   sz_type nf; //* number of inserted points [0;nb-1]
   sz_type ni; //* number of insertion after sum recalc
   double su;   //* current sum
   double dfl;  //* default value
   std::vector<double> d;
   static const constexpr sz_type recalc_after = 10000;
};

