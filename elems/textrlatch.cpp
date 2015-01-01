/***************************************************************************
                          textrlatch.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#include <cmath>
#include "textrlatch.h"

const char* TExtrLatch::helpstr = "<H1>TExtrLatch</H1>\n"
 "Extremum latch: <br>\n"
 "Latches value or time of given extremum type<br>\n"
 "-<B>type</B> - type of extremum; <br>\n"
 "-<B>useFirst </B> - latch only first extremum; <br>\n"
 "-<B>useLocal </B> - latch local extremums; <br>\n"
 "-<B>outT </B> - output time of latching; <br>\n"
 "-<B>useReset </B> - reset on u[1] strobe; <br>\n"
 "-<B>tStart </B> - start time; <br>\n"
 "-<B>fuzzy </B> - fuzzy level; <br>\n"
 "";

STD_CLASSINFO(TExtrLatch,clpElem);

CTOR(TExtrLatch,TMiso)
{
}


double TExtrLatch::f( double t )
{
  double v, u, ua, uc = in_u;
  int k;
  static const int mybits[6] = { 1, 2, 4, 7, 7, 0 };
  if( useLocal && fuzzy > 0 && isStart == 0 ) { // TODO: more robust need
    u = ( uc + fuzzy * u_old + 0.1 * fuzzy * u_old2 ) / ( 1 + 1.1 * fuzzy );
  } else {
    u = uc;
  };
  ua = fabs( u );
  if( t < tStart )                 // time before work
     return u;
  if( useReset && in_rst > 0.1 ) {   // reset on u[1] signal
    isStart = 1; wasExtr = 0; // for first only
    u_max = u_min = u_abs = u_ex = t_max = t_min = t_abs = t_ex = 0;
    u_old = u_old2 = t_old = 0;
  }
  if( useFirst && wasExtr )        // keep first extremum
    return outT ? t_ex : u_ex;
  if( isStart == 1 ) {
    isStart = 2;
    u_min = u_max = u; u_abs = ua; t_ex = t_min = t_max = t_abs = t;
    if( outT ) return t;
    switch( (int)type ) {
      case extrMax:
        u_ex = (double)u_max; break;
      case extrMin:
        u_ex = (double)u_min; break;
      case extrModMax:
        u_ex = (double)u_abs; break;
      case extrAver:
        u_ex = (double)u_max; break;
      case extrAmpl:
        u_ex = 0; break;
      default: u_ex = 0;
    };
    u_old = uc;
    return outT ? t : u;
  };
  if( isStart == 2 ) {
    isStart = 0;
    if( useLocal ) {
      u_old2 = u_old; u_old = uc;
      return u;
    };
  };
  if( !useLocal ) {
    k = 0;
    if( u > u_max )
      { u_max = u; t_max = t; k  = 1; };
    if( u < u_min )
      { u_min = u; t_min = t; k |= 2; };
    if( ua > u_abs )
      { u_abs = ua; t_abs = t; k |= 4; };
    if( k & mybits[ type ] ) {
      switch( (int)type ) {
        case extrMax:
          u_ex = (double)u_max; t_ex = (double)t_max; break;
        case extrMin:
          u_ex = (double)u_min; t_ex = (double)t_min; break;
        case extrModMax:
          u_ex = (double)u_abs; t_ex = (double)t_abs; break;
        case extrAver:
          u_ex = 0.5 * ( u_min + u_max );
          t_ex = ( (double)t_min > (double)t_max ) ? (double)t_min : (double)t_max;
          break;
        case extrAmpl:
          u_ex = 0.5 * ( (double)u_max - (double)u_min );
          t_ex = ( (double)t_min > (double)t_max ) ? (double)t_min : (double)t_max;
          break;
        default: u_ex = 0; t_ex = t;
      };
    } else {
      if( useFirst )
        wasExtr = 1;
    };
  } else {  // -------------------------- useLocal
    k = 0;
    if( u < u_old && u_old >= u_old2 ) {
      u_max = u_old; t_max = t_old; k = 1;
      if( u_max > u_abs ) {
        u_abs = (double)u_max; t_abs = (double)t_max; k |= 4;
      };
    };
    if( u > u_old && u_old <= u_old2 ) {
      u_min = u_old; t_min = t_old; k |= 2;
      if( fabs( u_min ) > u_abs ) {
        u_abs = fabs( u_min ); k |= 4;
      };
    };
    if( k & mybits[ type ] ) {
      switch( (int)type ) {
        case extrMax:
          u_ex = (double)u_max; t_ex = (double)t_max; break;
        case extrMin:
          u_ex = (double)u_min; t_ex = (double)t_min; break;
        case extrModMax:
          u_ex = (double)u_abs; t_ex = (double)t_abs; break;
        case extrAver:
          u_ex = 0.5 * ( (double)u_max + u_min );
          t_ex = ( (double)t_min > (double)t_max ) ? (double)t_min : (double)t_max;
          break;
        case extrAmpl:
          u_ex = 0.5 * ( (double)u_max - u_min );
          t_ex = ( (double)t_min > (double)t_max ) ? (double)t_min : (double)t_max;
          break;
        default: u_ex = 0; t_ex = t; break;
      };
    };
  };

  v = outT ? t_ex : u_ex;
  u_old2 = u_old; u_old = u; t_old = t;
  return v;
}

int TExtrLatch::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  isStart = 1; wasExtr = 0; // for first only
  u_max = u_min = u_abs = u_ex = t_max = t_min = t_abs = t_ex = 0;
  u_old = u_old2 = t_old = 0;
  return 1;
}

DEFAULT_FUNCS_REG(TExtrLatch)


// end of textrlatch.cpp


