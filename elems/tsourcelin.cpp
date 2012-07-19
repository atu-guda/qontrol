/***************************************************************************
                          tsourcelin.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h> 
#include "miscfun.h" 
#include "tsourcelin.h"

const char* TSourceLin::helpstr = "<H1>TSourceLin</H1>\n"
 "Source of signal, which consist of some linear segments. \n"
 "Each segment have time: <B>t_inr[i]</B>, \n"
 "start value: <B>vs[i]</B>, \n"
 "end value: <B>ve[i]</B>, <BR>\n"
 "List terminated, in t_int &lt=; 0.\n"
;

TClassInfo TSourceLin::class_info = {
  CLASS_ID_TSourceLin, "TSourceLin", TSourceLin::create,
  &TMiso::class_info, helpstr, clpElem };


TSourceLin::TSourceLin( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  for( i=0; i<16; i++ ) t_int[i] = vs[i] = ve[i] = 0;
  t_int[0] = 1; vs[0] = 0; ve[0] = 0;
  t_int[1] = 1; vs[1] = 0; ve[1] = 1;
  t_int[2] = 1; vs[2] = 1; ve[2] = 1;
  t_int[3] = 1; vs[3] = 1; ve[3] = 0;
  t_int[4] = 0; vs[4] = 0; ve[4] = 0;  // end marker
  recalc();

}

TSourceLin::~TSourceLin()
{
}

TDataSet* TSourceLin::create( TDataSet* apar )
{
  return new TSourceLin( apar );
}

const TClassInfo* TSourceLin::getClassInfo(void) const
{
  return &class_info;
}

const char *TSourceLin::getHelp(void) const
{
  return helpstr;
}

const char** TSourceLin::getIcon(void) const
{
  return icon;
}

double TSourceLin::f( double t )
{
  double tr, v;
  tr = t - t_start;
  while( tr > t_int[curr_lin] ) { // find next line (may be not the next)
    tr -= t_int[curr_lin]; t_start += t_int[curr_lin];
    curr_lin++;
    if( curr_lin >= n_lin )
      curr_lin = 0;
  };
  v = vs[curr_lin] + slopes[curr_lin] * tr;
  return v; 
}


int TSourceLin::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  recalc();
  t_start = 0; curr_lin = 0;
  return rc;
}

void TSourceLin::recalc(void)
{
  int i;
  // fail-safe
  if( t_int[0] <= 0 )
    t_int[0] = 1;
  for( i=0; i<16; i++ ) {
    if( t_int[i] <= 0 ) {
      n_lin = i; 
      break;
    };
    slopes[i] = ( ve[i] - vs[i] ) / t_int[i];
  };
}


int TSourceLin::registered = reg();

int TSourceLin::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TSourceLin::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
". c #000000",
"` c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #408080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"lllllll..........lllllllllllllll",
"lllllll..........lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"l`lllll..llllll......lllllllllll",
"```llll..lllllllllll..llllllllll",
"l`lllll..llllllllllll..lllllllll",
"l`lllll..llllllllllll..lllllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..llllllllllllllll..lllll",
"l`......lllllllllllllllll.....ll",
"l`......``````````````````....`l",
"llllllllllllllllllllllllllllllll"
};

// end of tsourcelin.cpp

