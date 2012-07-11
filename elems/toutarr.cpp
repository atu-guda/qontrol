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

TDataInfo TOutArr::toutarr_d_i[19] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 440, 200, 0,  0.0, 0.0, 0, 0, "OutArr",  "",   "Output array"},
 { dtpInt, dtpsList,   4,   20,  30, 110,  20, 2,  0.0, 0.0, 0, 0, "type",   "output type", toutarr_list },
 { dtpLabel,      0,   0,   20,  60, 120,  20, 0,  0.0, 0.0, 0, 0, "l_oname",   "",   "Output elem. name"},
 { dtpStr,        0,  MAX_NAMELEN,   20,  80, 110,  20, 2,  0.0, 0.0, 0, 0, "name",   "Name of elem wrom which putput stored",   ""},
 { dtpLabel,      0,   0,   20, 110, 120,  20, 0,  0.0, 0.0, 0, 0, "l_label", "",   "Output Label"},
 { dtpStr,        0,  MAX_LABELLEN,   20, 130, 110,  20, 2,  0.0, 0.0, 0, 0, "label",   "Output label for graph", ""},
 { dtpLabel,      0,   0,  150,  10, 160,  20, 0,  0.0, 0.0, 0, 0, "l_nq", "", "Collect every N tick"},
 { dtpInt,        0,   0,  150,  30, 100,  20, 2,  1.0, 1e5, 0, 0, "nq",   "Collect every N tick", ""},
 { dtpLabel,      0,   0,  150,  60, 160,  20, 0,  0.0, 0.0, 0, 0, "l_lnq", "", "Latch counter value"},
 { dtpInt,        0,   0,  150,  80, 100,  20, 2,  0.0, 1e5, 0, 0, "lnq",   "Latch counter value", ""},
 { dtpButton,     0,   0,  340,  20,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_ok",   "",   "OK"},
 { dtpButton,     1,   0,  340,  60,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_can",   "",   "Cancel"},
 { dtpButton,     2,   0,  340, 100,  80,  30, 0,  0.0, 0.0, 0, 0, "btn_help",   "",   "Help"}, 
 { dtpInt,        0,   0,    0,   0,   0,   0, 52,  0.0, 0.0, 0, 0, "arrsize",   "Array size", ""},
 { dtpDbl,        0,   0,    0,   0,   0,   0, 52,  0.0, 0.0, 0, 0, "dmin",   "Min value", ""},
 { dtpDbl,        0,   0,    0,   0,   0,   0, 52,  0.0, 0.0, 0, 0, "dmax",   "Max value", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 52,  0.0, 0.0, 0, 0, "n", "Fill size", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 52,  0.0, 0.0, 0, 0, "ny", "y-size for 3D", ""},
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};


TOutArr::TOutArr( TDataSet* apar )
        :TDataSet( apar ), 
	 fake_so(0), so( &fake_so ) 
{
  int i;
  arrsize = 0; dmin = 0; dmax = 1; n = ny = 0; nq = 1; cnq = lnq = 0;
  type = 0; // name[0] = 0; label[0] = 0;
  d_i = toutarr_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &type; ptrs[3] = &name; ptrs[5] = &label;
  ptrs[7] = &nq;
  ptrs[9] = &lnq;
  ptrs[13] = &arrsize; ptrs[14] = &dmin; ptrs[15] = &dmax; 
  ptrs[16] = &n; ptrs[17] = &ny;
}

TOutArr::~TOutArr()
{
  n = ny = 0;
}

TDataSet* TOutArr::create( TDataSet* apar )
{
  return new TOutArr( apar );
}

int TOutArr::getClassId(void) const 
{
  return CLASS_ID_TOutArr;
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
  so = parent->getDoublePtr( name );
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
  return ElemFactory::theFactory().registerElemType( &class_info  );
}



// end of toutarr.cpp

