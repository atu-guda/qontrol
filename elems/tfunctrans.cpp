/***************************************************************************
                          tfunctrans.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#include <cmath>
#include "miscfun.h"
#include "tfunctrans.h"

using namespace std;


const char* TFuncTrans::helpstr = "<H1>TFuncTrans</H1>\n"
 "Harmonic-alike functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

TClassInfo TFuncTrans::class_info = {
  CLASS_ID_TFuncTrans, "TFuncTrans", TFuncTrans::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TFuncTrans::TFuncTrans( TDataSet* aparent )
        :TMiso( aparent )
{
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 
}

TFuncTrans::~TFuncTrans()
{
}

TDataSet* TFuncTrans::create( TDataSet* apar )
{
  return new TFuncTrans( apar );
}


const TClassInfo* TFuncTrans::getClassInfo(void) const
{
  return &class_info;
}

const char *TFuncTrans::getHelp(void) const
{
  return helpstr;
}

const char** TFuncTrans::getIcon(void) const
{
  return icon;
}

double TFuncTrans::f( double /* t */ )
{
  double v, y, by, tm;
  y = *in_so[0] - *in_so[1] - x0; by = y * b;
  switch( type ) {
    case 0:   v = a * sin( by ); break;
    case 1:   v = a * sign( sin( by ) + c ); break;
    case 2:   v = a * tanh( by ); break;
    case 3:   v = a * atan2( *in_so[0], *in_so[1] ); break;
    case 4:   v = a * exp( by ); break;
    case 5:   v = a * exp( -by * y ); break;
    case 6:   v = a * waveWave( by ); break;
    case 7:   v = a * waveMhat( by ); break;
    case 8:   v = ( by > 0 ) ? ( a * log( by ) ) : 0; break;
    case 9:   v = a * y * exp( -by ); break;
    case 10:  v = a * y * exp( -by * y ); break;
    case 11:  tm = sin( by ); v = a * tm * tm; break;
    case 12:  v = a * cos( by ); break;
    case 13:  tm = cos( by ); v = a * tm * tm; break;
    case 14:  v = a * tan( by ); break;
    case 15:  tm = sin( d * y );  tm = 1 - c * tm*tm;
              v = a * exp( -by*y ) * tm; break;
    case 16:  tm = sin( d * y );  tm = 1 - c * tm*tm;
              v = a * (1-exp( -by*y )) * tm; break;
    default:  v = 0;
  };
  v += g;
  return v; 
}

int TFuncTrans::registered = reg();

int TFuncTrans::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TFuncTrans::icon[] = {
"32 32 5 1",
"c c #808080",
"b c #000000",
"a c #0000ff",
". c #dcdcdc",
"# c #ff0000",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"..............###...............",
"............##.##...............",
"............#..##...............",
"...........##...................",
"...........##...................",
"..........###...................",
"..........##....................",
".......#########................",
"..........##....................",
".........###....................",
".........###....................",
".........##...........#.........",
".........##..........#.#........",
".........##.........#...#.......",
"........###.........#...#.......",
"........###.........#...#.......",
"........##.....aa...#...#....aa.",
"........##....a..a.#.....#..a...",
"........##...a....#.......#a....",
".......##..cacccc#caccccccac#c..",
".......##...........a....a......",
"...##.##.............a..a.......",
"...#####..............aa........",
"....###.........................",
"................................"};

// end of tfunctrans.cpp


