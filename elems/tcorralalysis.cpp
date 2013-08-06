/***************************************************************************
                          tcorralalysis.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
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
#include "tmodel.h"
#include "toutarr.h"
#include "tcorralalysis.h"


const char* TCorrAnalysis::helpstr = "<H1>TCorrAnalysis</H1>\n"
 "Correlational analysis for x=u0; y = u1: <br>\n"
 "Unstable: Information about parameters see in info.";

TClassInfo TCorrAnalysis::class_info = {
  "TCorrAnalysis", TCorrAnalysis::create,
  &TMiso::class_info, helpstr, clpElem };


TCorrAnalysis::TCorrAnalysis( TDataSet* aparent )
        :TMiso( aparent )
{
  type = 0; 
  ok = n = ii = nc = 0;
  useCalc = useReset = useFill = 0;
  t0 = 0; t1 = 10000; out_a = out_b = out_corr = out_ok = -1;
  mainOutput = 0;
  reset_data();
  cmp_ms = cmp_min = cmp_max = cmp_ampl = cmp_tmin = cmp_tmax = 0;
  out_source[0] = &s_x; 
  out_source[1] = &s_x2; 
  out_source[2] = &s_y; 
  out_source[3] = &s_y2; 
  out_source[4] = &s_xy; 
  out_source[5] = &a; 
  out_source[6] = &b; 
  out_source[7] = &corr; 
  out_source[8] = &cov; 
  out_source[9] = &dis_x; 
  out_source[10] = &dis_y; 
  out_source[11] = &sigma_x; 
  out_source[12] = &sigma_y; 
  out_source[13] = &ave_x; 
  out_source[14] = &ave_y; 
  out_source[15] = &ave_x2; 
  out_source[16] = &ave_y2; 
  out_source[17] = 0; out_source[18] = 0; out_source[19] = 0; 
}

TCorrAnalysis::~TCorrAnalysis()
{
}

TDataSet* TCorrAnalysis::create( TDataSet* apar )
{
  return new TCorrAnalysis( apar );
}

const TClassInfo* TCorrAnalysis::getClassInfo(void) const
{
  return &class_info;
}

const char *TCorrAnalysis::getHelp(void) const
{
  return helpstr;
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
  arrx = model->getOutArr( x_oname );
  if( !arrx ) 
    return 1;
  nx = nx_c = 0;
  arrx->getData( "n", &nx );
  xdat = arrx->getArray();
  if( xdat == 0 || nx < 2 ) 
    return 1;
  cdat = 0;
  arrc = model->getOutArr( c_oname );  
  if( arrc ) {
    cdat = arrc->getArray();
    arrc->getData( "n", &nx_c );
  };
  if( cdat != 0 && nx_c == nx )
    do_cmp = 1;
  if( useFill ) {
    arry = model->getOutArr( y_oname );
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
  double x = *in_so[0], y = *in_so[1];
  ii++;
  if( useReset && *in_so[3] < 0.1 /* sic: < */ ) {
    reset_data();
  };
  switch( type ) {
    case 0: add = 1; break;
    case 1: add = ( t >= t0 ) && ( t <= t1 ); break;
    case 2: add = ( *in_so[2] > 0.1 ); break;
    case 3: add = 0; // dont add, use arrays
    default: add = 0;
  };
  if( add ) {
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y; 
    n++;
  };
  if( ( ii >= model_nn-1 || ( useCalc && *in_so[3] > 0.1 ))   ) {
    if( type >= 3 )
      getDataFromArrays();
    calc();
    putModelVars();
  };
  return *out_source[ mainOutput ];
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
    // DEBUG:
    // cerr << __PRETTY_FUNCTION__ << ": fail to find x: " << x_in << '\n';
  };
  // get x array
  TOutArr *arry = model->getOutArr( y_in );
  if( arry ) {
    arry->getData( "n", &ny );
    ydat = arry->getArray();
  } else {
    ydat = 0; ny = 0;
    // DEBUG:
    // cerr << __PRETTY_FUNCTION__ << ": fail to find y: " << x_in << '\n';
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

  nc = n;
    
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
  
void TCorrAnalysis::putModelVars()
{
  // model = static_cast<TModel*>(parent);
  if( out_a >= 0 )
    model->setVar( out_a, a );
  if( out_b >= 0 )
    model->setVar( out_b, b );
  if( out_corr >= 0 )
    model->setVar( out_corr, corr );
  if( out_ok >= 0 )
    model->setVar( out_ok, ok );
}

int TCorrAnalysis::registered = reg();

int TCorrAnalysis::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tcorranalysis.cpp

