/***************************************************************************
                          tlogic.cpp  -  description
                             -------------------
    begin                : Mon Sep 4 2000
    copyright            : (C) 2000-2013 by atu
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

#include "tlogic.h"

const char* TLogic::helpstr = "<H1>TLogic</H1>\n"
 "Simple logic element: <br>\n"
 "Parameters: <br>\n"
 " - <b>type</b> determinated type of logic: ANR, OR, XOR ... <br>\n"
 " - <b>level</b> -- level of logical 1 on input; <br>\n"
 " - <b>useNInp0 - useNInp3</b> -- inverse inputs; <br>\n"
 " - <b>useNOut</b> -- inverse output; <br>\n"
 " - <b>useMinus</b> -- negative output == -1, not 0.  <br>\n"
 "For AND unused inputs link to ':one' .";

TClassInfo TLogic::class_info = {
  "TLogic", TLogic::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TLogic::TLogic( TDataSet* aparent )
        :TMiso( aparent )
{
  level = 0.1; type = 1; useNInp0 = useNInp1 = useNInp2 = useNInp3 = 0;
  useNOut = useMinus = 0;
}

TLogic::~TLogic()
{
}


double TLogic::f( double /* t */ )
{
  int i, iu[4], ou;
  for( i=0; i<4; i++ ) {
    iu[i] = ( *in_so[i] > level );
  };
  if( useNInp0 ) iu[0] = ! iu[0];
  if( useNInp1 ) iu[1] = ! iu[1];
  if( useNInp2 ) iu[2] = ! iu[2];
  if( useNInp3 ) iu[3] = ! iu[3];
  ou = 0;
  switch( type ) {
    case 0: if( iu[0] > 0 && iu[1] > 0 && iu[2] > 0 && iu[3] > 0 ) 
	      ou = 1;
	    break;
    case 1: if( iu[0] > 0 || iu[1] > 0 || iu[2] > 0 || iu[3] > 0 ) 
	      ou = 1;
	    break;
    case 2: for( i=0; i < 4; i++ )
	      if( iu[i] > 0 )
		ou = !ou;
	    break;
    case 3: ou = iu[0]; break;
    case 4: ou = 0; break;
    case 5: ou = 1; break;
    default: ;
  };
  if( useNOut ) ou = !ou;
  if( ou ) return 1;
  return useMinus ? -1 : 0;
}

DEFAULT_FUNCS_REG(TLogic)


// end of tlogic.cpp



