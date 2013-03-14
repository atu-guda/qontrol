/***************************************************************************
                          toutarr.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#include <cstdlib>
#include <cstring>
#include "miscfun.h"
#include "toutarr.h"


const char* TOutArr::helpstr = "<H1>TOutArr</H1>\n"
 "Collector of output during simulation.\n"
 "Stores output from element, given by name\n"
 "Allowed special TModel output names like :t, #25, :parm1 \n"
 "Type selects when output be collected.";

TClassInfo TOutArr::class_info = {
 CLASS_ID_TOutArr, "TOutArr", TOutArr::create,
 &TDataSet::class_info, helpstr, clpSpecial };


TOutArr::TOutArr( TDataSet* apar )
        :TDataSet( apar ), 
	 fake_so(0), so( &fake_so ) 
{
  allow_add = 0;
  arrsize = 0; dmin = 0; dmax = 1; n = ny = 0; nq = 1; cnq = lnq = 0;
  type = 0; // name[0] = 0; label[0] = 0;
}

TOutArr::~TOutArr()
{
  n = ny = 0;
  qDebug( "dbg: TOutArr::~TOutArr(): %s", qPrintable( getFullName() ));
}

TDataSet* TOutArr::create( TDataSet* apar )
{
  return new TOutArr( apar );
}


const TClassInfo* TOutArr::getClassInfo(void) const
{
  return &class_info;
}

const char *TOutArr::getHelp(void) const
{
  return helpstr;
}

const double* TOutArr::getArray(void)
{
  return &arr[0];
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
  gi->row = gi->col = 0; gi->title[0] = 0; gi->ny = 1;
  for( i=0; i<7; i++ ) gi->label[i][0] = 0;
  strncat( gi->title, qPrintable( objectName() ), 
           sizeof( gi->title )-1 ); // TODO: real QString
  strncat( gi->label[0], label.toLocal8Bit().constData(), sizeof( gi->label[0] )-1 );
  if( n < 1 )
    return -2;
  gi->row = n; gi->col = 1; gi->ny = ny;
  gi->dat[0] = &arr[0];
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

int TOutArr::registered = reg();

int TOutArr::reg()
{
  return EFACT.registerElemType( &class_info  );
}



// end of toutarr.cpp

