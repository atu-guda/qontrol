/***************************************************************************
                          tdiffin.cpp - diff between input signals
                             -------------------
    begin                : 2015.02.25
    copyright            : (C) 2015-2022 by atu
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

#include "tdiffin.h"

const char* TDiffIn::helpstr = "<H1>TDiffIn</H1>\n"
 "Estimate derivative (1,2) on x and y inputs: <br>\n"
 "";

STD_CLASSINFO(TDiffIn,clpElem);

CTOR(TDiffIn,TMiso)
{
}


double TDiffIn::f() noexcept
{
  double x_ce, y_ce;
  if( emulC ) {
    x_ce = ( x_r + x_l ) / 2;
    y_ce = ( y_r + y_l ) / 2;
  } else {
    x_ce = x_c;
    y_ce = y_c;
  }

  dlt_x_rc = x_r  - x_ce;
  dlt_x_rl = x_r  - x_l;
  dlt_x_cl = x_ce - x_l;

  dlt_y_rc = y_r  - y_ce;
  dlt_y_rl = y_r  - y_l;
  dlt_y_cl = y_ce - y_l;

  // no check not 0 - no good reaction for it - NaN is good!
  d_rc = dlt_y_rc  /  dlt_x_rc;
  d_rl = dlt_y_rl  /  dlt_x_rl;
  d_cl = dlt_y_cl  /  dlt_x_cl;
  d2 = 2 * ( d_rc - d_cl ) / dlt_x_rl;

  return d_rl;
}


DEFAULT_FUNCS_REG(TDiffIn)


// end of tdiffin.cpp


