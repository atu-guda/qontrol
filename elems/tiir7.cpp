/*
 * =====================================================================================
 *
 *       Filename:  tiir7.cpp
 *
 *    Description:  IIR filter with 7 stored values implementation
 *
 *        Version:  1.0
 *        License:  GPLv2+
 *        Created:  2019-06-10 22:51:48
 *         Author:  Anton Guda (atu), atu@nmetau.edu.ua
 * =====================================================================================
 */

#include <miscfun.h>
#include <tiir7.h>

using namespace std;

const char* TIIR7::helpstr = "<H1>TIIR7</H1>\n"
 "<p>Simple IIR filer with limited (7) number of stored values. \n"
 "</p>\n"
;

STD_CLASSINFO(TIIR7,clpElem);

CTOR(TIIR7,TMiso)
{
}



double TIIR7::f() noexcept
{
  double uc = clamp( u.cval(), u_min.cval(), u_max.cval() );

  if( on_start ) {
    for( unsigned i=0; i<n_val; ++i ) {
      xo[i] = 0;
      uo[i] = 0;
    }
    on_start = false;
  }
  if( hold ) {
    return xo[0];
  }

  for( unsigned i=n_val-1; i>0; --i ) {
    uo[i] = uo[i-1];
    xo[i] = xo[i-1];
  }
  uo[0] = uc; xo[0] = 0;

  double v = 0;
  for( unsigned i=0; i<n_val; ++i ) {
    v += kb[i] * uo[i];
    v += ka[i] * xo[i];
  }

  v = clamp( v, x_min.cval(), x_max.cval() );
  xo[0] = v;

  return v;
}



int TIIR7::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  on_start = true;
  return 1;
}


DEFAULT_FUNCS_REG(TIIR7)


