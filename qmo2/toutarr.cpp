/***************************************************************************
                          toutarr.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#include <cstdlib>
#include <cstring>
#include "miscfun.h"
#include "toutarr.h"


const char* TOutArr::helpstr = "<H1>TOutArr</H1>\n"
 "Collector of output during simulation.\n"
 "Stores output from element, given by name\n"
 "Allowed special TModel output names like :t, #25, :parm1 \n"
 "Type selects when output be collected.";

STD_CLASSINFO(TOutArr,clpSpecial);

CTOR(TOutArr,TDataSet), 
   fake_so(0), so( &fake_so ) 
{
}

TOutArr::~TOutArr()
{
  n = ny = 0;
  // qDebug( "dbg: TOutArr::~TOutArr(): %s", qPrintable( getFullName() ));
}


const dvector* TOutArr::getArray(void)
{
  return &arr;
}

int TOutArr::alloc( int sz, int a_ny )
{
  if( sz == arrsize ) {
    n = 0; dmin = 0; dmax = 1; state = 2;
    return 0;
  };
  ny = a_ny;
  if( sz < 1 ) sz = 1; 
  if( ny < 1 ) ny = 1;
  arr.resize( sz ); 
  arrsize = sz; n = 0; dmin = 0; dmax = 1; cnq = 0;
  so = par->getDoublePtr( name );
  if( !so )
    so = &fake_so;
  return 0;
}

int TOutArr::free(void)
{
  // no real delete
  arrsize = 0;  n = 0; state = 1;
  return 0;
}

int TOutArr::reset( int level )
{
  if( level >= type ) {
    n = 0; dmin = 0; dmax = 1; state = 2;
  };
  return 0;
}


int TOutArr::push_val( double v, int level )
{
  if( n >= arrsize )
    return -1;
  if( level < type )
    return 0;
  if( cnq == lnq ) {
    if( n == 0 ) {
     dmin = dmax = v;
    } else {
      if( v > dmax ) dmax = v;
      if( v < dmin ) dmin = v;
    };  
    arr[n] = v;  
    n++; 
  };
  cnq++; 
  if( cnq >= nq ) 
    cnq = 0;
  return n;
}

int TOutArr::take_val( int level )
{
  double v = *so;
  if( n >= arrsize )
    return -1;
  if( level < type )
    return 0;
  if( cnq == lnq ) {
    if( n == 0 ) {
     dmin = dmax = v;
    } else {
      if( v > dmax ) dmax = v;
      if( v < dmin ) dmin = v;
    };  
    arr[n] = v;  
    n++; 
  };
  cnq++; 
  if( cnq >= nq ) 
    cnq = 0;
  return n;
}


int TOutArr::fillGraphInfo( GraphInfo *gi ) const
{
  int i;
  if( gi == 0 )
    return -1;
  gi->row = gi->col = 0; gi->ny = 1;
  for( i=0; i<7; i++ ) 
    gi->label[i] = "";
  gi->title = objectName();
  gi->label[0] = label.cval();
  if( n < 1 )
    return -2;
  gi->row = n; gi->col = 1; gi->ny = ny;
  gi->dat[0] = &arr;
  return 0;
}  

int TOutArr::dump( const char *fn, char delim )
{
  int k;
  GraphInfo gi;
  k = fillGraphInfo( &gi );
  if ( k != 0 )
    return k;
  k = dumpDatas( fn, &gi, delim );
  return k;
}

DEFAULT_FUNCS_REG(TOutArr)



// end of toutarr.cpp

