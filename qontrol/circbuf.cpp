/***************************************************************************
                          circbuf.c - circular buffer class realization
                             -------------------
    begin                : 2015.01.14 (move from miscfun.cpp)
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

#include "miscfun.h"
#include "circbuf.h"

TCircBuf::TCircBuf( sz_type nn, double v ) :
  nb(nn), s(0), nf(0), ni(0), su(0), dfl( v ),
  d( nb, v )
{
}

void TCircBuf::reset()
{
  s = nf = ni = 0; su = 0;
}

void TCircBuf::resize( sz_type n, double v )
{
  reset();
  d.resize( n, v );
  dfl = v;
  nb = n;
}

void TCircBuf::push_back( double a )
{
  if( nf >= nb ) {
    su -= d[s];
  }

  d[s++] = a; su += a;
  if( s >= nb ) {
    s = 0;
  }
  if( nf < nb ) {
    nf++;
  }
  ni++;

  if( ni > recalc_after ) {
    (void) sumCalc();
  }
}


double TCircBuf::at( sz_type i ) const
{
  if( ! isInBounds( 0, i, nf-1 ) ) { // first comparison rejects bad values for second
    return dfl;
  }
  return operator[]( i );
}


double TCircBuf::sumCalc()
{
  su = 0; ni = 0;
  for( sz_type i=0; i<nf; ++i ) {
    su += d[i];
  }
  return su;
}

