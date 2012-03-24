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
  &TMiso::class_info, helpstr };

TDataInfo TFourier::tfourier_d_i[50] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 480, 420, 0,  0.0, 0.0, 0, 0, "fourier_dia", "", "Dialog for TFourier"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_omega",   "", "omega"},
 { dtpDou,        0,   0,   20,  70, 120,  20, 2,  1e-3, 1e300, 0, 0, "omega", "base frequency", ""},
 { dtpLabel,      0,   0,   30, 100, 100,  20, 0,  0.0, 0.0, 0, 0, "l_ng",   "", "N of garmonics"},
 { dtpInt,        0,   0,   20, 120, 120,  20, 2,  1.0, 1e4, 0, 0, "ng", "", ""},
 { dtpLabel,      0,   0,   30, 150, 100,  20, 0,  0.0, 0.0, 0, 0, "l_t0",   "", "start time"},
 { dtpDou,        0,   0,   20, 170, 120,  20, 2,  -1e300, 1e300, 0, 0, "t0", "start time", ""},
 { dtpLabel,      0,   0,   30, 200, 100,  20, 0,  0.0, 0.0, 0, 0, "l_t1",   "", "stop time"},
 { dtpDou,        0,   0,   20, 220, 120,  20, 2,  -1e300, 1e300, 0, 0, "t1", "stop time", ""},
 { dtpInt,   dtpsSw,   0,   20, 250, 120,  20, 2,  0.0, 0.0, 0, 0, "useOptim", "", "Optimise t1"},
 { dtpLabel,      0,   0,  180,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_out", "", "Outputs:"},
 { dtpLabel,      0,   0,  240,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_vars", "", "Vars"},
 { dtpLabel,      0,   0,  340,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_outs", "", "Out arrays:"},
 { dtpLabel,      0,   0,  180,  70, 100,  20, 0,  0.0, 0.0, 0, 0, "l_out_a", "", "a0..a_ng"},
 { dtpInt,        0,   0,  240,  70, 100,  20, 2,  0.0, 1e4, 0, 0, "out_a", "start index for a[i]", ""},
 { dtpStr,        0,  32,  340,  70, 100,  20, 2,  0.0, 0.0, 0, 0, "a_oname", "output array for a[i]",""},
 { dtpLabel,      0,   0,  180, 100, 100,  20, 0,  0.0, 0.0, 0, 0, "l_out_b", "", "b0..b_ng"},
 { dtpInt,        0,   0,  240, 100, 100,  20, 2,  0.0, 1e4, 0, 0, "out_b", "start index for b[i]", ""},
 { dtpStr,        0,  32,  340, 100, 100,  20, 2,  0.0, 0.0, 0, 0, "b_oname", "output array for b[i]",""},
 { dtpLabel,      0,   0,  180, 130, 100,  20, 0,  0.0, 0.0, 0, 0, "l_out_am", "", "A0..A_ng"},
 { dtpInt,        0,   0,  240, 130, 100,  20, 2,  0.0, 1e4, 0, 0, "out_am", "start index for A[i]", ""},
 { dtpStr,        0,  32,  340, 130, 100,  20, 2,  0.0, 0.0, 0, 0, "am_oname", "output array for A[i]",""},
 { dtpLabel,      0,   0,  180, 160, 100,  20, 0,  0.0, 0.0, 0, 0, "l_out_om", "", "omega"},
 { dtpInt,        0,   0,  240, 160, 100,  20, 2,  0.0, 1e4, 0, 0, "out_om", "start index for omega[i]", ""},
 { dtpStr,        0,  32,  340, 160, 100,  20, 2,  0.0, 0.0, 0, 0, "om_oname", "output array for omega[i]",""},
 { dtpInt,   dtpsSw,   0,  180, 200, 180,  20, 2,  0.0, 0.0, 0, 0, "useFill", "", "Fill compare array:"},
 { dtpStr,        0,  32,  180, 230, 100,  20, 2,  0.0, 0.0, 0, 0, "y_oname", "out array name",""},
 { dtpLabel,      0,   0,  180, 250, 100,  20, 0,  0.0, 0.0, 0, 0, "l_x_oname", "", "Using as x:"},
 { dtpStr,        0,  32,  180, 270, 100,  20, 2,  0.0, 0.0, 0, 0, "x_oname", "in array name",""},
 { dtpButton,     0,   0,   20, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "a0", "a0", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "a1", "a1", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "b1", "b1", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ampl1", "A1", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "phi", "phi", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ampl", "A", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "qpow", "qual. on power crit", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "qpow1", "", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "s_x2", "input power", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "tp", "period", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "np", "np", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "nn", "nn", ""},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TFourier::TFourier( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  omega = 1; t0 = 0; t1 = 1e6; tp = 2 * M_PI / omega; 
  np = n_st = n_en = ii = 0; nn = nper = 1;
  ng = 3; useOptim = 1; useFill = 0;
  out_a = out_b = out_am = out_om = -1;
  s_x2 = a0 = a1 = b1 = ampl = ampl1 = phi = qpow = qpow1 = 0;
  aa = bb = am = 0; 
  d_i = tfourier_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &omega; ptrs[6] = &ng; ptrs[8] = &t0;
  ptrs[10] = &t1; ptrs[11] = &useOptim;
  ptrs[16] = &out_a; ptrs[17] = &a_oname; 
  ptrs[19] = &out_b; ptrs[20] = &b_oname; 
  ptrs[22] = &out_am; ptrs[23] = &am_oname; 
  ptrs[25] = &out_om; ptrs[26] = &om_oname; 
  ptrs[27] = &useFill; ptrs[28] = &y_oname; ptrs[30] = &x_oname;
  ptrs[34] = &a0; ptrs[35] = &a1; ptrs[36] = &b1; ptrs[37] = &ampl1; 
  ptrs[38] = &phi; ptrs[39] = &ampl; ptrs[40] = &qpow; ptrs[41] = &qpow1; 
  ptrs[42] = &s_x2; ptrs[43] = &tp; ptrs[44] = &np; ptrs[45] = &nn; 
  // from TMiso 
  ptrs[46] = links;
  ptrs[47] = &vis_x; ptrs[48] = &vis_y;
}

TFourier::~TFourier()
{
  delete[] aa; delete[] bb; delete[] am;
  aa = bb = am = 0;
}

TDataSet* TFourier::create( TDataSet* apar )
{
  return new TFourier( apar );
}

int TFourier::getClassId(void) const 
{
  return CLASS_ID_TFourier;
}

const char* TFourier::getClassName(void) const 
{
  return "TFourier";
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

int TFourier::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int rc = TMiso::preRun( run_tp, an, anx, any, adt );
  aa = new double[ ng+2 ];  bb = new double[ ng+2 ];
  am = new double[ ng+2 ];
  return rc;
}

int TFourier::postRun( int good )
{
  delete[] aa; delete[] bb; delete[] am;
  aa = bb = am = 0;
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
  if( parent == 0 ) return rc;
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
  arrx->getDataSI( "n", &nx, 0 );
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

double TFourier::f( const double* u, double t )
{
  int i;
  double av;
  if( ii < n_st || ii > n_en ) { ii++; return ampl1; };
  for( i=0; i<=ng; i++ ) {
    aa[i] += u[0] * cos( i * omega * t );
    bb[i] += u[0] * sin( i * omega * t );
  };
  s_x2 += u[0] * u[0];
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
  model->getDataSD( "tt", &tt, 0 );
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

