/***************************************************************************
                          tcorralalysis.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by atu
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
  CLASS_ID_TCorrAnalysis, "TCorrAnalysis", TCorrAnalysis::create,
  &TMiso::class_info, helpstr };

TDataInfo TCorrAnalysis::tcorranalysis_d_i[64] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 420, 0,  0.0, 0.0, 0, 0, "corr_dia", "", "Dialog for TCorrAnalysis"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Collect"},
 { dtpInt, dtpsList,   4,   20,  70, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", "All\nt0<t<t1\nu2>0\narrays"},
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_t0", "", "t0"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  0.0, 1e300, 0, 0, "t0", "t0", ""},
 { dtpLabel,      0,   0,   30, 140,  50,  20, 0,  0.0, 0.0, 0, 0, "l_t1", "", "t1"},
 { dtpDou,        0,   0,   20, 160, 120,  20, 0,  0.0, 1e300, 0, 0, "t1", "t1", ""},
 { dtpInt,   dtpsSw,   0,   20, 190, 150,  20, 2,  0.0, 0.0, 0, 0, "useCalc", "", "Use u3>0 as Calc"},
 { dtpInt,   dtpsSw,   0,   20, 210, 150,  20, 2,  0.0, 0.0, 0, 0, "useReset", "", "Use u3<0 as Reset"},
 { dtpLabel,      0,   0,   20, 250, 120,  20, 0,  0.0, 0.0, 0, 0, "l_x_in", "", "X input array"},
 { dtpStr,        0,  32,   20, 270, 120,  20, 2,  0.0, 0.0, 0, 0, "x_in", "","name of X input array"},
 { dtpLabel,      0,   0,   20, 300, 120,  20, 0,  0.0, 0.0, 0, 0, "l_y_in", "", "Y input array"},
 { dtpStr,        0,  32,   20, 320, 120,  20, 2,  0.0, 0.0, 0, 0, "y_in", "","name of Y input array"},
 { dtpLabel,      0,   0,  240,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_outs", "", "Outputs:"},
 { dtpLabel,      0,   0,  220,  70,  50,  20, 0,  0.0, 0.0, 0, 0, "l_out_a", "", "a"},
 { dtpInt,        0,   0,  250,  70,  80,  20, 2,  0.0, 0.0, 0, 0, "out_a", "index for a out", ""},
 { dtpLabel,      0,   0,  220, 100,  50,  20, 0,  0.0, 0.0, 0, 0, "l_out_b", "", "b"},
 { dtpInt,        0,   0,  250, 100,  80,  20, 2,  0.0, 0.0, 0, 0, "out_b", "index for b out", ""},
 { dtpLabel,      0,   0,  220, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "l_out_corr", "", "corr"},
 { dtpInt,        0,   0,  250, 130,  80,  20, 2,  0.0, 0.0, 0, 0, "out_corr", "index for corr out", ""},
 { dtpLabel,      0,   0,  220, 160,  50,  20, 0,  0.0, 0.0, 0, 0, "l_out_ok", "", "ok"}, 
 { dtpInt,        0,   0,  250, 160,  80,  20, 2,  0.0, 0.0, 0, 0, "out_ok", "index for ok out", ""},
 { dtpLabel,      0,   0,  220, 220, 120,  20, 0,  0.0, 0.0, 0, 0, "l_x_name", "", "Using array as x:"},
 { dtpStr,        0,  32,  220, 240, 120,  20, 2,  0.0, 0.0, 0, 0, "x_name", "name of x array",""},
 { dtpInt,   dtpsSw,   0,  220, 260, 150,  20, 2,  0.0, 0.0, 0, 0, "useFill", "", "Fill output array"},
 { dtpStr,        0,  32,  220, 280, 120,  20, 2,  0.0, 0.0, 0, 0, "y_name", "name of y array to fill",""},
 { dtpLabel,      0,   0,  220, 300, 120,  20, 0,  0.0, 0.0, 0, 0, "l_c_name", "", "Compare with:"},
 { dtpStr,        0,  32,  220, 320, 120,  20, 2,  0.0, 0.0, 0, 0, "c_name", "name of array to compare",""},
 { dtpInt, dtpsList,  17,  220, 350, 120,  20, 2,  0.0, 0.0, 0, 0, "mainOutput", "main Output", 
   "summ(x)\n"      //  0
   "summ(x^2)\n"    //  1
   "summ(y)\n"      //  2
   "summ(y^2)\n"    //  3 
   "summ(xy)\n"     //  4
   "a\n"            //  5
   "b\n"            //  6
   "corr\n"         //  7
   "cov\n"          //  8
   "disp(x)\n"      //  9
   "dips(y)\n"      // 10
   "sigma(x)\n"     // 11
   "sigma(y)\n"     // 12
   "aver(x)\n"      // 13
   "aver(y)\n"      // 14
   "aver(x^2)\n"    // 15
   "aver(y^2)"      // 16
 },
 { dtpButton,     0,   0,   20, 380,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 380,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 380,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"}, 
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "corr", "corr", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cov", "cov", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "dis_x", "dis_x", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "dis_y", "dis_y", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sigma_x", "sigma_x", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sigma_y", "sigma_y", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ave_x", "ave_x", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ave_y", "ave_y", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ave_x2", "ave_x2", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ave_y2", "ave_y2", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sum_x",  "sum_x", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sum_y",  "sum_y", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sum_x2", "sum_x2", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sum_y2", "sum_y2", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "sum_xy", "sum_xy", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "nc", "nc", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "n", "n", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "ok", "ok", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_ms", "mean sqare", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_min", "min(e)", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_max", "max(e)", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_ampl", "ampl(e)", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_tmin", "time(min)", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "cmp_tmax", "time(max)", ""},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TCorrAnalysis::TCorrAnalysis( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  type = 0; 
  ok = n = ii = nc = 0;
  useCalc = useReset = useFill = 0;
  t0 = 0; t1 = 10000; out_a = out_b = out_corr = out_ok = -1;
  x_oname[0] = y_oname[0] = c_oname[0] = 0;
  x_in[0] = y_in[0] = 0;
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
  d_i = tcorranalysis_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &t0; ptrs[8] = &t1;
  ptrs[9] = &useCalc; ptrs[10] = &useReset;
  ptrs[12] =& x_in; ptrs[14] = &y_in;

  ptrs[17] = &out_a; ptrs[19] = &out_b; 
  ptrs[21] = &out_corr; ptrs[23] = &out_ok;
  ptrs[25] = x_oname; ptrs[26] = &useFill; ptrs[27] = y_oname;
  ptrs[29] = c_oname;
  ptrs[30] = &mainOutput;
  // out values
  ptrs[34] = &a; ptrs[35] = &b; ptrs[36] = &corr; ptrs[37] = &cov; 
  ptrs[38] = &dis_x; ptrs[39] = &dis_y; 
  ptrs[40] = &sigma_x; ptrs[41] = &sigma_y; 

  ptrs[42] = &ave_x;  ptrs[43] = &ave_y;
  ptrs[44] = &ave_x2; ptrs[45] = &ave_y2;
  
  ptrs[46] = &s_x;  ptrs[47] = &s_y;
  ptrs[48] = &s_x2; ptrs[49] = &s_y2;
  ptrs[50] = &s_xy; 
  ptrs[51] = &nc; 

  ptrs[52] = &n; ptrs[53] = &ok; 
  ptrs[54] = &cmp_ms; ptrs[55] = &cmp_min; ptrs[56] = &cmp_max; 
  ptrs[57] = &cmp_ampl; ptrs[58] = &cmp_tmin; ptrs[59] = &cmp_tmax; 
  // from TMiso 
  ptrs[60] = links;
  ptrs[61] = &vis_x; ptrs[62] = &vis_y;
}

TCorrAnalysis::~TCorrAnalysis()
{
}

TDataSet* TCorrAnalysis::create( TDataSet* apar )
{
  return new TCorrAnalysis( apar );
}

int TCorrAnalysis::getClassId(void) const 
{
  return CLASS_ID_TCorrAnalysis;
}

const char* TCorrAnalysis::getClassName(void) const 
{
  return "TCorrAnalysis";
}

const TClassInfo* TCorrAnalysis::getClassInfo(void) const
{
  return &class_info;
}

const char *TCorrAnalysis::getHelp(void) const
{
  return helpstr;
}

const char** TCorrAnalysis::getIcon(void) const
{
  return icon;
}

int TCorrAnalysis::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int rc = TMiso::preRun( run_tp, an, anx, any, adt );
  return rc;
}

void TCorrAnalysis::reset_data()
{
  ok = n = ii = nc = 0;
  s_x = s_x2 = s_y = s_y2 = s_xy = corr = cov = a = b = dis_x = dis_y = 0;
  sigma_x = sigma_y = 0;
  ave_x = ave_y = ave_x2 = ave_y2 = 0;
}

int TCorrAnalysis::startLoop( int acnx, int acny )
{
  reset_data();
  return TMiso::startLoop( acnx, acny );
}

int TCorrAnalysis::endLoop(void)
{
  double yy, e, se2;
  int arrxn, arryn, arrcn, j, nx, nx_c, rc, do_cmp, do_fill;
  const double *xdat, *cdat;
  TOutArr *arrx, *arry = 0, *arrc;
  cmp_ms = cmp_min = cmp_max = cmp_ampl = cmp_tmin = cmp_tmax = se2 = 0; 
  do_cmp = do_fill = 0;
  rc = TMiso::endLoop();
  if( parent == 0 )
    return rc;
  arrxn = model->outname2out_nu( x_oname );
  arrx = model->getOutArr( arrxn );
  if( arrx == 0 ) 
    return rc;
  nx = nx_c = 0;
  arrx->getDataSI( "n", &nx, 0 );
  xdat = arrx->getArray();
  if( xdat == 0 || nx < 2 ) 
    return rc;
  arrcn = model->outname2out_nu( c_oname );
  arrc = model->getOutArr( arrcn );  cdat = 0;
  if( arrc != 0 ) {
    cdat = arrc->getArray();
    arrc->getDataSI( "n", &nx_c, 0 );
  };
  if( cdat != 0 && nx_c == nx )
    do_cmp = 1;
  if( useFill ) {
    arryn = model->outname2out_nu( y_oname );
    arry = model->getOutArr( arryn );
    if( arry != 0 ) {
      arry->alloc( nx, 1 );
      do_fill = 1;
    };
  };
  if( do_fill == 0 && do_cmp == 0 )
    return rc;

  for( j=0; j<nx; j++ ) {
    yy = a * xdat[j] + b;
    if( do_cmp ) {
      e = cdat[j] - yy;
      se2 += e * e;
      if( e > cmp_max ) {
        cmp_max = e; cmp_tmax = xdat[j]; 
      };
      if( e < cmp_min && j > 0 ) {
	cmp_min = e; cmp_tmin = xdat[j];
      };
    };
    if( do_fill )
      arry->push_val( yy, 100000 ); // ignore level
  };
  if( do_cmp ) {
    cmp_ms = sqrt( se2 / nx );
    cmp_ampl = 0.5 * ( cmp_max - cmp_min );
  };
  return rc; 
}


double TCorrAnalysis::f( const double* u, double t )
{
  int add;
  double x = u[0], y = u[1];
  ii++;
  if( useReset && u[3] < 0.1 /* sic: < */ ) {
    reset_data();
  };
  switch( type ) {
    case 0: add = 1; break;
    case 1: add = ( t >= t0 ) && ( t <= t1 ); break;
    case 2: add = ( u[2] > 0.1 ); break;
    case 3: add = 0; // dont add, use arrays
    default: add = 0;
  };
  if( add ) {
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y; 
    n++;
  };
  if( ( ii >= model_nn-1 || ( useCalc && u[3] > 0.1 ))   ) {
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
  const double *xdat, *ydat;
  int nx = 0, ny = 0;
  
  reset_data();
  // get x array
  int arrxn = model->outname2out_nu( x_in );
  TOutArr *arrx = model->getOutArr( arrxn );
  if( arrx != 0 ) {
    arrx->getDataSI( "n", &nx, 0 );
    xdat = arrx->getArray();
  } else {
    xdat = 0; nx = 0;
    // DEBUG:
    // cerr << __PRETTY_FUNCTION__ << ": fail to find x: " << x_in << '\n';
  };
  // get x array
  int arryn = model->outname2out_nu( y_in );
  TOutArr *arry = model->getOutArr( arryn );
  if( arry != 0 ) {
    arry->getDataSI( "n", &ny, 0 );
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
      x = xdat[i];
    if( ydat )
      y = ydat[i];
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

/* XPM */
const char* TCorrAnalysis::icon[] = {
/* width height num_colors chars_per_pixel */
"32 32 5 1",
". c #ff00ff",
"# c #800080",
"a c #ffff00",
"c c #ffffff",
"b c #ff0000",
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
"##aaa###########################",
"#a###a##########################",
"#a##############################",
"#a######a##aaa#aaa##############",
"#a#####a#a#a###a##############b#",
"#a###a#a#a#a###a###########bbb##",
"##aaa###a##a###a##ccc###bbbb####",
"#################c###bbbb#######",
"##########c######cbbbb#c########",
"##########c####bbb#####c########",
"#########c#cbbbbc######c########",
"#####c###bbb###c########c####c##",
"####c#bbbb#cccc#########c###c###",
"###bbb#c#################ccc####",
"####c###########################",
"###c############################",
"################################",
"################################",
"################################",
"################################"
};


// end of tcorranalysis.cpp

