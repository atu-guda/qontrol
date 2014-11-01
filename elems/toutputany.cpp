/***************************************************************************
                          toutputany.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000-2014 by atu
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
#include "tmodel.h"
#include "toutputany.h"

const char* TOutputAny::helpstr = "<H1>TOutputAny</H1>\n"
 "<h2>Obsoleted element, dont work at all: use links</h2>\n"
 "Puts signal to given target: <br>\n"
 "1-st form: #12 - put models var #12 <br>\n"
 "2-nd form: name relative to model: source.u <br>\n"
 "Have 1 parameter: <b>name</b>.";

STD_CLASSINFO(TOutputAny,clpElem);

CTOR(TOutputAny,TMiso)
{
}



double TOutputAny::f( double /* t */ )
{
  return in_u;
}

DEFAULT_FUNCS_REG(TOutputAny)


// end of toutputany.cpp


