/***************************************************************************
                          tfourier.cpp  -  description
                             -------------------
    begin                : Sat Sep 9 2000
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
#include "tfourier.h"

const char* TFourier::helpstr = "<H1>TFourier</H1>\n"
 "Perform Fourier analisys on input: <br>\n"
 "Parameters see in object info.";

TClassInfo TFourier::class_info = {
  CLASS_ID_TFourier, "TFourier", TFourier::create,
  &TMiso::class_info, helpstr, clpElem };


TFourier::TFourier( TDataSet* aparent )
        :TMiso( aparent )
{
  omega = 1; t0 = 0; t1 = 1e6; tp = 2 * M_PI / omega; 
  np = n_st = n_en = ii = 0; nn = nper = 1;
  ng = 3; useOptim = 1; useFill = 0;
  out_a = out_b = out_am = out_om = -1;
  s_x2 = a0 = a1 = b1 = ampl = ampl1 = phi = qpow = qpow1 = 0;
}

TFourier::~TFourier()
{
}

TDataSet* TFourier::create( TDataSet* apar )
{
  return new TFourier( apar );
}


const TClassInfo* TFourier::getClassInfo(void) const
{
  return &class_info;
}

const char *TFourier::getHelp(void) const
{
  return helpstr;
}

const char** TFourier::getIcon(void) const
{
  return icon;
}

int TFourier::do_preRun( int /*run_tp*/, int /*an*/, 
                         int /*anx*/, int /*any*/, double /*adt*/ )
{
  aa.resize( ng+2 );  bb.resize( ng+2 );  am.resize( ng+2 );
  return 0;
}

int TFourier::postRun( int good )
{
  TMiso::postRun( good );
  return 0;
}

int TFourier::startLoop( int acnx, int acny )
{
  int i;
  int rc = TMiso::startLoop( acnx, acny );
  for( i=0; i<=ng; i++ )
  aa[i] = bb[i] = am[i] = 0;
  s_x2 = a0 = a1 = b1 = ampl = ampl1 = phi = qpow = qpow1 = 0;
  ii = 0; n_st = 0; n_en = model_nn;
  initVars();
  return rc;
}

int TFourier::endLoop(void)
{
  int i, j, arrxn, arryn, arran, arrbn, arramn, arromn, nx, rc;
  TOutArr *arra, *arrb, *arram, *arrom, *arrx, *arry;
  const double *xdat;
  double v, vx;
  rc = TMiso::endLoop();
  if( !par ) return rc;
  if( out_a >= 0 ) { // ------- fill model vars 
    for( i=0; i<=ng; i++ ) 
      model->setVar( out_a + i, aa[i]);
  };
  if( out_b >= 0 ) {
    for( i=0; i<=ng; i++ ) 
      model->setVar( out_b + i, bb[i]);
  };
  if( out_am >= 0 ) {
    for( i=0; i<=ng; i++ ) 
      model->setVar( out_am + i, am[i]);
  };
  if( out_om >= 0 ) {
    for( i=0; i<=ng; i++ ) 
      model->setVar( out_om + i, omega * i );
  };
  arran = model->outname2out_nu( a_oname.toLocal8Bit().constData() );// ---- fill a,b,am output arrays
  arra = model->getOutArr( arran );
  if( arra != 0 ) {
    arra->alloc( ng+1, 1 );
    for( i=0; i<=ng; i++ )
      arra->push_val( aa[i], 10000 ); // ignore level
  };
  arrbn = model->outname2out_nu( b_oname.toLocal8Bit().constData() );
  arrb = model->getOutArr( arrbn );
  if( arrb != 0 ) {
    arrb->alloc( ng+1, 1 );
    for( i=0; i<=ng; i++ )
      arrb->push_val( bb[i], 10000 ); // ignore level
  };
  arramn = model->outname2out_nu( am_oname.toLocal8Bit().constData() );
  arram = model->getOutArr( arramn );
  if( arram != 0 ) {
    arram->alloc( ng+1, 1 );
    for( i=0; i<=ng; i++ )
      arram->push_val( am[i], 10000 ); // ignore level
  };
  arromn = model->outname2out_nu( om_oname.toLocal8Bit().constData() );
  arrom = model->getOutArr( arromn );
  if( arrom != 0 ) {
    arrom->alloc( ng+1, 1 );
    for( i=0; i<=ng; i++ )
      arrom->push_val( omega * i, 10000 ); // ignore level
  };

  if( ! useFill ) return rc;
  arrxn = model->outname2out_nu( x_oname.toLocal8Bit().constData() ); // -- fill compare out array
  arryn = model->outname2out_nu( y_oname.toLocal8Bit().constData() );
  arrx = model->getOutArr( arrxn );
  arry = model->getOutArr( arryn );
  if( arrx == 0 || arry == 0 ) return rc;
  xdat = arrx->getArray();
  nx = -1;
  arrx->getData( "n", &nx );
  if( xdat == 0 || nx < 2 ) return rc;
  arry->alloc( nx, 1 ); 
  for( i=0; i<nx; i++ ) {
    v = 0; vx = omega * xdat[i];
    for( j=0; j<=ng; j++ )
      v += aa[j] * cos( j * vx ) +  bb[j] * sin( j * vx );
    arry->push_val( v, 10000 ); // ignore level
  };
  return rc;
}

double TFourier::f( double t )
{
  int i;
  double av;
  if( ii < n_st || ii > n_en ) { ii++; return ampl1; };
  double xx = *in_so[0];
  for( i=0; i<=ng; i++ ) {
    aa[i] += xx * cos( i * omega * t );
    bb[i] += xx * sin( i * omega * t );
  };
  s_x2 += xx * xx;
  if( ii == n_en ) {
    aa[0] /= nn; bb[0] = 0; am[0] = fabs( aa[0] ); ampl = am[0] * am[0];
    for( i=1; i<=ng; i++ ) {
      aa[i] /= nn / 2;
      bb[i] /= nn / 2;
      av = aa[i] * aa[i] + bb[i] * bb[i];
      am[i] = sqrt( av );
      ampl += av;
    };
    s_x2 /= nn; ampl = sqrt( ampl );
    a0 = aa[0]; a1 = aa[1];  b1 = bb[1]; ampl1 = am[1];
    phi = atan2( a1, b1 ); 
    if( s_x2 > 0 ) {
      qpow = ampl * ampl / s_x2;
      qpow1 = ampl1 * ampl1 / s_x2;
    } else {
      qpow = qpow1 = 1; // no source - quality max 
    };
  };
  ii++;
  return ampl1;
}

void TFourier::initVars(void)
{
  double t_st, t_en, tt, dt;
  tp = 2 * M_PI / omega; 
  np = int( tp / tdt );
  tt = 1; // safe value
  model->getData( "tt", &tt );
  t_st = 0; t_en = tt;
  if( t0 > 0 && t0 < tt-tp ) 
    t_st = t0;
  if( t1 > tp && t1 < tt ) 
    t_en = t1;
  dt = t_en - t_st; nper = int( dt / tp );
  n_st = int( t_st / tdt ); 
  if( useOptim ) {
    dt = nper * tp;
  };
  nn = int( 0.5 + dt / tdt );
  n_en = n_st + nn;
  if( n_en >= model_nn ) n_en = model_nn-1;
}

int TFourier::registered = reg();

int TFourier::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TFourier::icon[] = {
/* width height num_colors chars_per_pixel */
"32 32 6 1",
". c #ff00ff",
"b c #00ff00",
"# c #800080",
"c c #ffff00",
"d c #ffffff",
"a c #ff0000",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
".............................###",
"################################",
"####################aaa#########",
"################b###a###########",
"###############bbb##aa#aaa#a#a##",
"###############b#b##a##a#a#a#a##",
"##############b###b#a##aaa#aaaa#",
"##############b###b#############",
"######cc#####b#####b####cc######",
"#####c##c####b#####b###c##c#####",
"####c####c##b#######b#c####c####",
"###c######c#b#######bc######c###",
"#dcddddddddbddddddddcbdddddddcd#",
"###########bc######c##b#######c#",
"##########b##c####c###b#######b#",
"##########b###c##c#####b######b#",
"#########b#####cc######b#####b##",
"####b####b##############b####b##",
"####b###b###############b###b###",
"#####b##b################b##b###",
"#####bbb#################bbb####",
"################################"
};

// end of tfourier.cpp

