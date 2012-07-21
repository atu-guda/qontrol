/***************************************************************************
                          textrlatch.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
    copyright            : (C) 2000-2012 by atu
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

TClassInfo TExtrLatch::class_info = {
  CLASS_ID_TExtrLatch, "TExtrLatch", TExtrLatch::create,
  &TMiso::class_info, helpstr, clpElem };


TExtrLatch::TExtrLatch( TDataSet* aparent )
        :TMiso( aparent )
{
  type = 0; useLocal = useFirst = outT = useReset = 0; tStart = fuzzy = 0;
  u_min = u_max = t_min = t_max = 0; isStart = wasExtr = 0; 
}

TExtrLatch::~TExtrLatch()
{
}

TDataSet* TExtrLatch::create( TDataSet* apar )
{
  return new TExtrLatch( apar );
}


const TClassInfo* TExtrLatch::getClassInfo(void) const
{
  return &class_info;
}

const char *TExtrLatch::getHelp(void) const
{
  return helpstr;
}

const char** TExtrLatch::getIcon(void) const
{
  return icon;
}

double TExtrLatch::f( double t )
{
  double v, u, ua;
  int k;
  static const int mybits[6] = { 1, 2, 4, 7, 7, 0 };
  if( useLocal && fuzzy > 0 && isStart == 0 ) { // TODO: more robust need
    u = ( *in_so[0] + fuzzy * u_old + 0.1 * fuzzy * u_old2 ) / ( 1 + 1.1 * fuzzy );
  } else {
    u = *in_so[0]; 
  };
  ua = fabs( u );
  if( t < tStart )                 // time before work
     return u;
  if( useReset && *in_so[1] > 0.1 ) {   // reset on u[1] signal
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
    switch( type ) {
      case 0: u_ex = u_max; break; 
      case 1: u_ex = u_min; break;
      case 2: u_ex = u_abs; break;
      case 3: u_ex = u_max; break;
      case 4: u_ex = 0; break;
      default: u_ex = 0;
    };
    u_old = *in_so[0];
    return outT ? t : u;
  };
  if( isStart == 2 ) {
    isStart = 0;
    if( useLocal ) {
      u_old2 = u_old; u_old = *in_so[0]; 
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
      switch( type ) {
	case 0: u_ex = u_max; t_ex = t_max; break;
	case 1: u_ex = u_min; t_ex = t_min; break;
	case 2: u_ex = u_abs; t_ex = t_abs; break;
	case 3: u_ex = 0.5 * ( u_min + u_max ); 
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	case 4: u_ex = 0.5 * ( u_max - u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
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
	u_abs = u_max; t_abs = t_max; k |= 4;
      };
    }; 
    if( u > u_old && u_old <= u_old2 ) {
      u_min = u_old; t_min = t_old; k |= 2;
      if( fabs( u_min ) > u_abs ) {
	u_abs = fabs( u_min ); k |= 4;
      };
    }; 
    if( k & mybits[ type ] ) {
      switch( type ) {
	case 0: u_ex = u_max; t_ex = t_max; break;
	case 1: u_ex = u_min; t_ex = t_min; break;
	case 2: u_ex = u_abs; t_ex = t_abs; break;
	case 3: u_ex = 0.5 * ( u_max + u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	case 4: u_ex = 0.5 * ( u_max - u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	default: u_ex = 0; t_ex = t; break;
      };
    };
  };

  v = outT ? t_ex : u_ex; 
  u_old2 = u_old; u_old = u; t_old = t;
  return v;
}

int TExtrLatch::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  isStart = 1; wasExtr = 0; // for first only
  u_max = u_min = u_abs = u_ex = t_max = t_min = t_abs = t_ex = 0;
  u_old = u_old2 = t_old = 0;
  return rc;
}

int TExtrLatch::registered = reg();

int TExtrLatch::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TExtrLatch::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjiiiggggggggb",
"jjjjjjjjjjjjjjjjjjiiijjgjiibbbbb",
"jjjjjjjjjjjjjjjjjiijjjjgjjiibbbb",
"jjjjjjjjjjjjjjjjiijjjjjgjjjiibbb",
"jjjjjjjjjjjjjjjjijjjjjjgjjjjibbb",
"jjjjjjjjjjjjjjjijjjjjjjgjjjjbibb",
"jjjjjjjjjjjjjjijjjjjjjjgjjjjbiib",
"bbbbbbbbbbbbbibbbbbbbbbgbbbbbbib",
"bbbbbbbbbbbbiibbbbbbbbbgbbbbbbbb",
"bbbbbbbbbbbiibbbbbbbbbbgbbbbbbbb",
"bbbbbbbbbbbibbbbbbbbbbbgbbbbbbbb",
"ffffffffffiifffffffffffffffffffb",
"bbbbbbbbbiibbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbibbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbiibbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbibbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbibbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbiibbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbibbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"biibbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
};

// end of textrlatch.cpp


