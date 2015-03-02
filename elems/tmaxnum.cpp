/***************************************************************************
                          tmaxnum.cpp  -   find maximum input
                             -------------------
    begin                : 2015.03.02
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

#include "tmaxnum.h"

const char* TMaxNum::helpstr = "<H1>TMaxNum</H1>\n"
 "<p>Selects maximum (min) of {u_0-u_7} inputs, and output Number of maximum <br>\n</p>";

STD_CLASSINFO(TMaxNum,clpElem);

CTOR(TMaxNum,TMiso)
  , ins{ &in_0, &in_1, &in_2, &in_3, &in_4, &in_5, &in_6, &in_7 }
{
}


double TMaxNum::f( double /* t */ )
{
  bool need_min = ( type == et_MIN ) || ( type == et_MINMOD );
  bool need_mod = ( type == et_MAXMOD ) || ( type == et_MINMOD );

  double vs[m_n_in];
  for( int i=0; i<n_in; ++i ) {
    double v = *(ins[i]);
    if( need_mod ) { v = fabs(v); };
    if( need_min ) { v = -v; };
    vs[i] = v;
  }
  double mm = vs[0];
  int n_m = 0;
  for( int i=0; i<n_in; ++i ) {
    if( vs[i] > mm ) {
      mm = vs[i]; n_m = i;
    }
  }

  if( n_m > n_max ) { n_m = n_max; }
  if( n_m < n_min ) { n_m = n_min; }

  int i_im_prev = n_m - 1;
  int i_im_next = n_m + 1;

  if( i_im_prev < n_min ) { i_im_prev = n_min; }
  if( i_im_next > n_max ) { i_im_next = n_max; }

  im = n_m; im_prev = i_im_prev; im_next = i_im_next;
  vm = vs[n_m]; vm_prev = vs[i_im_prev]; vm_next = vs[i_im_next];

  if( need_min ) {
    vm = -vm; vm_prev = -vm_prev; vm_next = -vm_next;
  }

  return n_m;
}

DEFAULT_FUNCS_REG(TMaxNum)


// end of tmaxnum.cpp

