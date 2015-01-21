/***************************************************************************
         tfilesource.c  - element to read text data from file (implementation)
                          -------------------
    begin                : 2015.01.21
    copyright            : (C) 2015-2015 by atu
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

#include <time.h>
#include <cmath>
#include "miscfun.h"
#include "scheme.h"
#include "tfilesource.h"

const char* TFileSource::helpstr = "<H1>TFileSource</H1>\n"
 "<p>Reads data from file in textual form<br>\n";

STD_CLASSINFO(TFileSource,clpElem);

CTOR(TFileSource,TMiso)
{
}



double TFileSource::f( double t )
{
  double v = t;
  return v;
}

int TFileSource::do_preRun( int /*run_tp*/, int /*an*/,
                     int /*anx*/, int /*any*/, double /*adt*/ )
{
  return 1;
}


int TFileSource::do_startLoop( int acnx, int acny )
{
  return 1;
}

int TFileSource::do_endLoop()
{
  file.close();
  return 1;
}

DEFAULT_FUNCS_REG(TFileSource)


// end of tfilesource.cpp

