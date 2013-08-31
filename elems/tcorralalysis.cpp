/***************************************************************************
                          tcorralalysis.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
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

#include <cmath>
#include "tmodel.h"
#include "toutarr.h"
#include "tcorralalysis.h"


const char* TCorrAnalysis::helpstr = "<H1>TCorrAnalysis</H1>\n"
 "Correlational analysis for x=u0; y = u1: <br>\n"
 "Unstable: Information about parameters see in info.";

STD_CLASSINFO(TCorrAnalysis,clpElem );

CTOR(TCorrAnalysis,TMiso)
{
}



void TCorrAnalysis::reset_data()
{
  ok = n = ii = nc = 0;
  s_x = s_x2 = s_y = s_y2 = s_xy = corr = cov = a = b = dis_x = dis_y = 0;
  sigma_x = sigma_y = 0;
  ave_x = ave_y = ave_x2 = ave_y2 = 0;
}

int TCorrAnalysis::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  reset_data();
  return 0;
}

int TCorrAnalysis::do_endLoop()
{
  double yy, e, se2;
  int j, nx, nx_c, do_cmp, do_fill;
  const dvector *xdat, *cdat;
  TOutArr *arrx, *arry = 0, *arrc;
  cmp_ms = cmp_min = cmp_max = cmp_ampl = cmp_tmin = cmp_tmax = se2 = 0; 
  do_cmp = do_fill = 0;
  if( ! par )
    return 1;
  QString tmp = x_oname;
  arrx = model->getOutArr( tmp );
  if( !arrx ) 
    return 1;
  nx = nx_c = 0;
  arrx->getData( "n", &nx );
  xdat = arrx->getArray();
  if( xdat == 0 || nx < 2 ) 
    return 1;
  cdat = 0;
  tmp = c_oname;
  arrc = model->getOutArr( tmp );  
  if( arrc ) {
    cdat = arrc->getArray();
    arrc->getData( "n", &nx_c );
  };
  if( cdat != 0 && nx_c == nx )
    do_cmp = 1;
  if( useFill ) {
    tmp = y_oname;
    arry = model->getOutArr( tmp );
    if( arry ) {
      arry->alloc( nx, 1 );
      do_fill = 1;
    };
  };
  if( do_fill == 0 && do_cmp == 0 )
    return 1;

  for( j=0; j<nx; j++ ) {
    yy = a * (*xdat)[j] + b;
    if( do_cmp ) {
      e = (*cdat)[j] - yy;
      se2 += e * e;
      if( e > cmp_max ) {
        cmp_max = e; cmp_tmax = (*xdat)[j]; 
      };
      if( e < cmp_min && j > 0 ) {
	cmp_min = e; cmp_tmin = (*xdat)[j];
      };
    };
    if( do_fill )
      arry->push_val( yy, 100000 ); // ignore level
  };
  if( do_cmp ) {
    cmp_ms = sqrt( se2 / nx );
    cmp_ampl = 0.5 * ( cmp_max - cmp_min );
  };
  return 0; 
}


double TCorrAnalysis::f( double t )
{
  int add;
  double x = in_x, y = in_y;
  ii++;
  if( useReset && in_rst > 0.1 ) { // history was < 0.1
    reset_data();
  };
  switch( (int)type ) {
    case 0: add = 1; break;
    case 1: add = ( t >= t0 ) && ( t <= t1 ); break;
    case 2: add = ( in_add > 0.1 ); break;
    case 3: add = 0; // dont add, use arrays
    default: add = 0;
  };
  if( add ) {
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y; 
    n++;
  };
  if( ( ii >= model_nn-1 || ( useCalc && in_calc > 0.1 ))   ) {
    if( type >= 3 )
      getDataFromArrays();
    calc();
  };
  return s_x;
}

int TCorrAnalysis::getDataFromArrays()
{
  double x, y;
  const dvector *xdat, *ydat;
  int nx = 0, ny = 0;
  
  reset_data();
  // get x array
  TOutArr *arrx = model->getOutArr( x_in );
  if( arrx ) {
    arrx->getData( "n", &nx );
    xdat = arrx->getArray();
  } else {
    xdat = 0; nx = 0;
  };
  // get x array
  TOutArr *arry = model->getOutArr( y_in );
  if( arry ) {
    arry->getData( "n", &ny );
    ydat = arry->getArray();
  } else {
    ydat = 0; ny = 0;
  };
  if( nx < 1 && ny < 1 )
    return 0;
  if( nx < 1 )
    n = ny;
  else if( ny < 1 )
    n = nx;
  else
    n = ( nx < ny ) ? nx : ny;
  x = y = 0;
  for( int i=0; i<n; i++ ) {
    if( xdat )
      x = (*xdat)[i];
    if( ydat )
      y = (*ydat)[i];
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y;
  };
  return n;
}

int TCorrAnalysis::calc()
{
  double dd;
  if( n < 1 ) {
    ok = 0;
    return 0;
  };

  nc = (int)n;
    
  ave_x = s_x / n; ave_y = s_y / n;
  ave_x2 = s_x2 / n; ave_y2 = s_y2 / n;
  dis_x = ( s_x2 * n - s_x * s_x ) / ( double(n) * n );
  sigma_x = sqrt( dis_x );
  dis_y = ( n * s_y2 - s_y * s_y ) / ( n * n );
  sigma_y = sqrt( dis_y );

  dd = n * s_x2 - s_x * s_x;
  cov = ( n * s_xy - s_x * s_y ) / ( n * n );
  if( dd > 0 ) {
    a = ( n * s_xy - s_x * s_y ) / dd;
    b = ( s_y * s_x2 - s_x * s_xy ) / dd; 
    corr = ( n * s_xy - s_x * s_y ) 
      / sqrt( ( n*s_x2 - s_x*s_x ) * ( n*s_y2 - s_y*s_y ) ); 
    ok = 1;
  } else {
    a = b = corr = 0; ok = 0;
  };
  return ok;
}  
  

DEFAULT_FUNCS_REG(TCorrAnalysis)


// end of tcorranalysis.cpp

