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

#include "circbuf.h"

TCircBuf::TCircBuf( unsigned nn ) :
  nb(nn), s(0), nf(0), ni(0), su(0),
  d( nb, 0 )
{
  reset();
}

TCircBuf::~TCircBuf()
{
  reset();
}

void TCircBuf::reset()
{
  s = nf = ni = 0; su = 0;
}

void TCircBuf::resize( unsigned n )
{
  reset();
  d.resize( n, 0 );
  nb = n;
}

void TCircBuf::add( double a )
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

double TCircBuf::operator[]( int i ) const
{
  int j;
  if( i < 0 || (unsigned)i >= nf ) { // first comparison rejects bad values for second
    return 0;
  }
  j = s - i - 1;
  if( j < 0 ) {
    j += nb;
  }
  return d[j];
}


double TCircBuf::sumCalc()
{
  su = 0; ni = 0;
  for( unsigned i=0; i<nf; ++i ) {
    su += d[i];
  }
  return su;
}

