/***************************************************************************
                          tmodel.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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
#include <ctime>
#include <cmath>
#include <unistd.h>
#include "miscfun.h"
#include "tmodel.h"

using namespace std;

const char* TModel::helpstr = "<H1>TModel</H1>\n"
 "Hold all active elements, output arrays and graph descriptions";

static const int RESERVED_OUTS = 1024;

TClassInfo TModel::class_info = {
 CLASS_ID_TModel, "TModel", TModel::create,
 &TDataContainer::class_info, helpstr };

TModel::TModel( TDataSet* aparent )/*{{{1*/
       :TDataContainer( aparent ), 
         vars( MODEL_NVAR, 0 ),
	 outs( RESERVED_OUTS, 0 )
{
  n_el = n_out = n_graph = 0; end_loop = 0;
  tt = 100; nn = n_tot= 100000; nl1 = 1; nl2 = 1; n_steps=100; use_sync = 0;
  prm0s = prm1s = prm2s = prm3s = 0.1; 
  prm0d = prm1d = 0.01; xval1 = xval2 = 0;
  seed = 117; useSameSeed = 1; seedType = 0;
  ic_mouse = ic_joy = ic_sound = ic_key = ic_aux = -1;
  oc_0 = oc_1 = oc_2 = oc_3 = oc_4 = oc_5 = -1;
  oct_0 = oct_1 = oct_2 = oct_3 = oct_4 = oct_5 = 0;
  long_descr[0] = 0;
  ii = il1 = il2 = i_tot = 0;
  rtime = t = 0; tdt = tt / nn; 
  prm0 = prm1 = prm2 = prm3 = 0; start_time = 0; 
  prm0s = prm1s = 0.1; prm0d = prm1d = 0.01; sgnt = -3;
  const int ELM_RES = 64; const int OUT_RES = 32;
  v_el.reserve( ELM_RES ); v_ord.reserve( ELM_RES ); v_flg.reserve( ELM_RES );
  v_out.reserve( OUT_RES ); v_oute.reserve( OUT_RES ); 
  v_outt.reserve( OUT_RES );
  v_graph.reserve( 16 );
  inps.reserve( ELM_RES ); pinps.reserve( ELM_RES ); 
  pnames.reserve( ELM_RES ); pflags.reserve( ELM_RES );
  fillCommon();
}/*}}}1*/

TModel::~TModel()/*{{{1*/
{
}/*}}}1*/


double TModel::runOneElem( int elnu, const double *u, double t )/*{{{1*/
{
  TMiso *ob;
  double v;
  ob = getMiso( elnu );
  if( ob == 0 ) return 0;
  v = ob->f( u, t );
  outs[elnu] = v;
  return v;
}/*}}}1*/

int TModel::startRun( int type )/*{{{1*/
{
  int rc;
  if( run_type >= 0 ) // in progress now
    return -1;
  if( type < 0 || type > 2 )
    type = 0;
  reset(); 
  run_type = type; 
  n1_eff = n2_eff = 1;
  if( run_type > 0 ) n1_eff = nl1;
  if( run_type > 1 ) n2_eff = nl2;
  n_tot = nn * n1_eff * n2_eff; i_tot = ii = il1 = il2 = 0;
  sgnt = int( time( 0 ) );
  prm2 = prm2s; prm3 = prm3s;
  //if( seed != -1 )
  //  srand( seed );
  allocOutArrs( run_type );
  rc = preRun( run_type, n1_eff, n2_eff );
  if( rc != 0 ) {
    end_loop = 1; 
    postRun();
    return rc;
  };
  t = rtime = 0;

  return 0;
}/*}}}1*/

int TModel::nextSteps( int csteps )/*{{{1*/
{
  int i, k, elnu, cm, rc;
  double pval;
  TMiso *cur_el;
  if( csteps < 1 ) csteps = 1;  
  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; i++, i_tot++ ) {
    prm0 = prm0s + il1 * prm0d; prm1 = prm1s + il2 * prm1d;
    if( ii == 0 ) {    // --------------------- start inner loop --
      if( il1 == 0 ) { // start prm0 loop
	resetOutArrs( 1 );
      };
      // if( useSameSeed && seed != -1 )
      //   srand( seed );
      resetOutArrs( 0 );
      start_time = get_real_time(); rtime = t = 0;
      // set start param
      for( elnu=0; elnu<n_el; elnu++ ) {
       cur_el = getMiso( elnu );
       for( k=0; k<pinps[elnu].maxl; k++ ) {
	 if( pinps[elnu].l[k] == -1 || pnames[elnu].l[k] == -1 )
	    continue;
	 if( ! ( pflags[elnu].l[k] & 1 ) ) 
	   continue;
	 pval = xout( pinps[elnu].l[k] );
	 cm = cur_el->setDataID( pnames[elnu].l[k], pval, 1 );
	 if( cm == 0 )
	   modified |= 2;
       };
      };
      allStartLoop( il1, il2 );
    };

    rc = runOneLoop();
    if( rc == 1 )
      return 0;    // wait for real time

    if( ii >= nn ) {
      allEndLoop();
      ii = 0; il1++; 
    };
    if( il1 >= nl1 ) {
      il1 = 0; il2++;
    };
  };
  if( i_tot >= n_tot )
    stopRun(0);
  return 0;
}/*}}}1*/

int TModel::stopRun( int reason )/*{{{1*/
{
  if( end_loop || reason ) {
    reset();
    state = stateGood;
  } else {
    postRun();
    state = stateDone;
  };
  return 0;
}/*}}}1*/
  
int TModel::runOneLoop(void)/*{{{1*/
{
  int k, out_nu, elnu, cm, out_level;
  double u[4], pval;
  unsigned long wait_ms;
  TMiso *cur_el; TOutArr *arr;
  rtime = get_real_time() - start_time;
  if( use_sync ) {
     if( t > rtime ) {
       wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms );
       // return 1;
     };
  };  
  out_level = 0;
  if( ii == nn-1 ) {
    out_level = run_type;
  };
  for( elnu=0; elnu<n_el && end_loop == 0; elnu++ ) {
     cur_el = static_cast<TMiso*> ( ptrs[ v_el[elnu] ]);
     int curr_flg = v_flg[elnu];
     if( (curr_flg &  4 ) && ( ii != 0 ) ) continue; // only first
     if( (curr_flg &  8 ) && ( ii != nn-1 ) ) continue; // only last 
     u[0] = u[1] = u[2] = u[3] = 0; // fill u[]
     for( k=0; k<inps[elnu].maxl; k++ ) {
       u[k] = xout( inps[elnu].l[k] );
     };
     for( k=0; k<pinps[elnu].maxl; k++ ) { // set parametrs if need
       if( pinps[elnu].l[k] == -1 || pnames[elnu].l[k] == -1 )
	  continue;
       if( pflags[elnu].l[k] & 1 ) continue; // skip 'only first'
       pval = xout( pinps[elnu].l[k] );
       cm = cur_el->setDataID( pnames[elnu].l[k], pval, 1 );
       if( cm == 0 )
	  modified |= 2;
     };
     // skip locked and noauto
     if( curr_flg & 2 ) continue;  // noauto
     if( curr_flg & 1 ) { // locked
       outs[elnu] = u[0];
       continue;
     };
     outs[elnu] = cur_el->f( u, t );  // <==================== main action
  };  // end element loop;
  for( out_nu=0; out_nu<n_out; out_nu++ ) { // fill out arrays(0)
    arr = getOutArr( out_nu );
    arr->push_val( xout( v_oute[out_nu] ), out_level );
  };
  t += tdt; ii++;
  return 0;
}/*}}}1*/

int TModel::preRun( int run_tp, int anx, int any )/*{{{1*/
{
  int elnu, rc;
  TMiso *ob;
  tdt = tt / nn;
  if( outs.size() < (unsigned)n_el ) {
    outs.resize( n_el * 2 , 0 );
  } 
  outs.assign( outs.size(), 0 );
  
  state = stateRun;
  for( elnu=0; elnu<n_el; elnu++ ) {
    ob = getMiso( elnu );
    if( ob != 0 ) {
      rc = ob->preRun( run_tp, nn, anx, any, tdt );
      if( rc != 0 )
	return rc;
    };
  };
  return 0;
}/*}}}1*/

int TModel::postRun(void)/*{{{1*/
{
  int elnu, cm = 0;
  TMiso *ob;
  for( elnu=0; elnu<n_el; elnu++ ) {
    ob = getMiso( elnu );
    if( ob != 0 ) {
      ob->postRun( end_loop );
      cm |= ob->getModified();
    };
  };
  if( modified == 0 || cm != 0 )
    modified |= 2;
  return 0;
}/*}}}1*/

int TModel::reset(void)/*{{{1*/
{
  int i;
  TOutArr *arr;
  for( i=0; i<n_out; i++ ) {
    arr = getOutArr( i );
    if( arr == 0 ) continue; // never be!
    arr->free();
  };
  linkNames();
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}/*}}}1*/

int TModel::allStartLoop( int acnx, int acny )/*{{{1*/
{
  int elnu, rc;
  TMiso *ob;
  for( elnu=0; elnu<n_el; elnu++ ) {
    ob = getMiso( elnu );
    if( ob == 0 ) continue;
    rc = ob->startLoop( acnx, acny );
    if( rc != 0 )
      return rc;
  };
  return 0;
}/*}}}1*/

void TModel::allEndLoop(void)/*{{{1*/
{
  int elnu;
  TMiso *ob;
  for( elnu=0; elnu<n_el; elnu++ ) {
    ob = getMiso( elnu );
    if( ob == 0 ) continue;
    ob->endLoop();
  };
}/*}}}1*/

void TModel::allocOutArrs( int tp )/*{{{1*/
{
  int out_nu, out_tp;
  TOutArr *arr;
  if( tp < 0 || tp > 2 )
    return;
  for( out_nu=0; out_nu < n_out; out_nu++ ) { // alloc output array
    arr = getOutArr( out_nu );
    if( arr == 0 ) continue;
    out_tp = -1;
    arr->getDataSI( "type", &out_tp, 0 );
    if( out_tp < 0 || out_tp > tp )
      continue;
    switch( out_tp ) {
      case 0: arr->alloc( nn, 1 ); break;
      case 1: arr->alloc( nl1, 1 ); break;
      case 2: arr->alloc( nl1 * nl2, nl1 ); break;
      default: ; // don't allocate special arrays
    };
  };
}/*}}}1*/

void TModel::resetOutArrs( int level )/*{{{1*/
{
  int out_nu;
  TOutArr *arr;
  for( out_nu=0; out_nu < n_out; out_nu++ ) { 
    arr = getOutArr( out_nu );
    if( arr == 0 ) continue;
    arr->reset( level );
  };
}/*}}}1*/


double TModel::getVar( int n )/*{{{1*/
{
  if( n < 0 || n >= MODEL_NVAR )
    return 0;
  return vars[n];
}/*}}}1*/

void TModel::setVar( int n, double v )/*{{{1*/
{
  if( n < 0 || n >= MODEL_NVAR )
    return;
  vars[n] = v;
}/*}}}1*/

double* TModel::getVars(void)/*{{{1*/
{
  return &(vars[0]); // TODO: const vector<double>&
}/*}}}1*/

double TModel::xout( int inu )/*{{{1*/
{
  int k;
  if( inu >= n_el )
    return 0;
  if( inu >= 0 )
    return outs[inu];
  if( inu > -10 ) return 0; // bad or no access by number
  switch( inu ) {
   case -10: return t;
   case -11: return tdt;
   case -12: return prm0;
   case -13: return prm1;	    
   case -14: return prm2;
   case -15: return prm3;
   case -16: return nn;	    
   case -17: return nl1;	    
   case -18: return nl2;	    
   case -19: return ii;	    
   case -20: return il1;	    
   case -21: return il2;
   case -22: return tt;
   case -23: return M_SQRT2;
   case -24: return M_SQRT1_2;
   case -25: return 1;
   case -26: return M_PI;
   case -27: return M_E;
   case -28: return rtime;
  };
  k = -100-inu;
  if( k >=0 && k < MODEL_NVAR ) 
    return vars[ k ];
  return 0;
}/*}}}1*/

double TModel::xout( const char *iname )/*{{{1*/
{
  int i;
  i = oname2elnu( iname );
  if( i != -3  &&  i != -4 )
    return xout( i );
  return 0;
}/*}}}1*/

int TModel::oname2elnu( const char *iname ) const/*{{{1*/
{
  int i, j, l, idx;
  static const char *spc_names[19] =  {
   "t", "tdt", "prm0", "prm1", "prm2", "prm3", 
   "nn", "nl1", "nl2", "ii", "il1", "il2", "T" , "sqrt2", "sqrt1_2",
   "one", "PI", "E", "rtime"
  };	  
  l = strlen( iname );
  if( l < 1 || l >= MAX_INPUTLEN ) return -2; // bad name
  if( iname[0] == ':' ) {  // TModel special nums
    for( i=0; i<19; i++ ) {
      if( strcmp( spc_names[i], iname+1 ) == 0 )  // TODO: hash names ?
        return -10-i;
    };
    return -2;
  };  
  if( iname[0] == '.' ) {  // name relative to TModel
    return -4;
  };	  
  if( iname[0] == '/' ) {  // from root -- not handled as number
     return -3;
  };
  if( iname[0] == '#' ) {  // access to vars[]
    j = atoi( iname+1 );
    if( j < 0 || j >= MODEL_NVAR )
      return -2;
    return (-100-j);
  };
  idx = getDataIdx( iname );
  if( idx < 0 )
    return -2;
  for( i=0; i<n_el; i++ ) {
    if( v_el[i] == idx )
      return i;
  };
  return -1;
}/*}}}1*/

int TModel::outname2out_nu( const char *iname ) const/*{{{1*/
{
  int i, l, idx;
  l = strlen0( iname );
  if( l < 1 || l >= MAX_INPUTLEN ) return -1; // bad name
  idx = getDataIdx( iname );
  if( idx < 0 )
    return -1;
  for( i=0; i<n_out; i++ ) {
    if( v_out[i] == idx )
      return i;
  };
  return -1;
}/*}}}1*/

int TModel::elnu2idx( int elnu ) const/*{{{1*/
{
  if( elnu < 0 || elnu >= n_el )
    return -1;
  return v_el[elnu];
}/*}}}1*/

int TModel::ord2elnu( int aord ) const/*{{{1*/
{
  int i;
  for( i=0; i<n_el; i++ ) {
    if( v_ord[i] == aord )
      return i;
  };	  
  return -1;
}/*}}}1*/

int TModel::fback( int code, int /* aord */, const char* /* tdescr */ )/*{{{1*/
{
  if( code ) {
    end_loop = code;
  };    
  return 0;
}/*}}}1*/

int TModel::checkData( int n )/*{{{1*/
{
  if( n < 0 )
    linkNames();
  return TDataContainer::checkData( n );
}/*}}}1*/

int TModel::xy2elnu( int avis_x, int avis_y )/*{{{1*/
{
  int i, ox, oy;
  TMiso *ob;
  for( i=0; i<n_el; i++ ) {
    ob = static_cast<TMiso*>(ptrs[v_el[i]]);
    if( ob == 0 ) continue;
    ox = oy = -1;
    ob->getDataSI( "vis_x", &ox, 0 );
    ob->getDataSI( "vis_y", &oy, 0 );
    if( ox == avis_x && oy == avis_y ) 
      return i;
  };	  
  return -1;
}/*}}}1*/

int TModel::getNMiso(void) const/*{{{1*/
{
  return n_el;
}/*}}}1*/

TMiso* TModel::getMiso( int elnu )/*{{{1*/
{
  if( elnu < 0 || elnu >= n_el )
    return 0;
  return static_cast<TMiso*> ( ptrs[ v_el[elnu] ]);
}/*}}}1*/

int TModel::getNOutArr(void) const/*{{{1*/
{
  return n_out;
}/*}}}1*/

TOutArr* TModel::getOutArr( int out_nu )/*{{{1*/
{
  int k;
  if( out_nu < 0 || out_nu >= n_out )
    return 0;
  k = v_out[out_nu];
  if( k < 0 || k >= nelm )
    return 0;
  if( d_i[k].tp != dtpObj || d_i[k].subtp != CLASS_ID_TOutArr )
    return 0;
  return static_cast<TOutArr*>(ptrs[ k ]);
}/*}}}1*/

int TModel::getNGraph(void) const/*{{{1*/
{
  return n_graph;
}/*}}}1*/

TGraph* TModel::getGraph( int gra_nu )/*{{{1*/
{
  if( gra_nu < 0 || gra_nu >= n_graph )
    return 0;
  return static_cast<TGraph*>(ptrs[v_graph[gra_nu]]);
}/*}}}1*/

int TModel::insElem( const TDataInfo *cdi, int aord, int avis_x, int avis_y )/*{{{1*/
{
  TMiso *ob;
  int k;
  reset();
  k = add_obj( cdi );
  ob = static_cast<TMiso*>( ptrs[nelm-1] );
  if( k != 0 || ob == 0 || ob->isChildOf( "TMiso" ) == 0 ) 
    return -1;
  ob->setDataSI( "ord", aord, 0 );
  ob->setDataSI( "vis_x", avis_x, 0 );  ob->setDataSI( "vis_y", avis_y, 0 );
  linkNames();
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::insOut( const char *outname, const char *objname )/*{{{1*/
{
  TOutArr *ob;
  int k;
  static TDataInfo odi = 
   { dtpObj, CLASS_ID_TOutArr, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0, 0, 
	   "", "", "" };
  reset();
  odi.name[0] = 0; strncat( odi.name, outname, MAX_NAMELEN-1 );
  k = add_obj( &odi );
  if( k != 0 ) return k;
  ob = static_cast<TOutArr*>( ptrs[nelm-1] );
  if( ob == 0 || ob->isChildOf( "TOutArr" ) == 0 ) 
    return -1;
  ob->setDataSS( "name", objname, 0 );
  
  if( strncmp( outname, "out_", 4 ) == 0 
      && outname[4] != 0 &&
      isGoodName(outname+4) ) 
  {
    ob->setDataSS( "label", outname+4, 0 );
  } else {
    ob->setDataSS( "label", objname, 0 );
  }
  linkNames(); 
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::insGraph( const char *gname )/*{{{1*/
{
  TGraph *ob;
  int k;
  static TDataInfo gdi = 
   { dtpObj, CLASS_ID_TGraph, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0, 0, 
	   "", "", "" };
  reset();
  gdi.name[0] = 0; strncat( gdi.name, gname, MAX_NAMELEN-1 );
  k = add_obj( &gdi );
  if( k != 0 ) return -1;
  ob = static_cast<TGraph*>( ptrs[nelm-1] );
  if( ob == 0 || ob->isChildOf( "TGraph" ) == 0 ) 
    return -1;
  linkNames(); 
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::delElem( const char *ename )/*{{{1*/
{
  int ne, k;
  reset();
  ne = oname2elnu( ename );
  if( ne < 0 )
    return -1;
  k = del_obj( v_el[ne] );
  linkNames();
  modified |= 1;
  return k;
}/*}}}1*/

int TModel::delElem( int elnu )/*{{{1*/
{
  int k;
  reset();
  if( elnu < 0 || elnu >= n_el )
    return -1;
  k = del_obj( v_el[elnu] );
  linkNames();
  modified |= 1;
  return k;
}/*}}}1*/


int TModel::delOut( int out_nu )/*{{{1*/
{
  int k;
  reset();
  if( out_nu < 0 || out_nu >= n_out )
    return -1;
  k = del_obj( v_out[out_nu] );
  linkNames();
  modified |= 1;
  return k;
}/*}}}1*/

int TModel::delGraph( int gr_nu )/*{{{1*/
{
  int k;
  if( gr_nu < 0 || gr_nu >= n_graph )
    return -1;
  reset();
  k = del_obj( v_graph[gr_nu] );
  linkNames();
  modified |= 1;
  return k;
}/*}}}1*/

int TModel::newOrder( const char *name, int new_ord )/*{{{1*/
{
  int elnu;
  elnu = oname2elnu( name );
  if( elnu < 0 || elnu >= n_el )
    return -1;
  return newOrder( elnu, new_ord );
}/*}}}1*/

int TModel::newOrder( int elnu, int new_ord )/*{{{1*/
{
  int k;
  TMiso *ob;
  reset();
  if( elnu < 0 || elnu >= n_el )
    return -1;
  k = ord2elnu( new_ord );
  if( k >= 0 )
    return -1;
  ob = static_cast<TMiso*>(ptrs[v_el[elnu]]);
  ob->setDataSI( "ord", new_ord, 0 );
  ob->getDataSI( "ord", &k, 0 );
  v_ord[elnu] = k;
  linkNames();
  modified |= 1;
  return ( k == new_ord ) ? 0 : -1;
}/*}}}1*/

int TModel::moveElem( int elnu, int newx, int newy )/*{{{1*/
{
  int i, cx, cy;
  TMiso *ob, *ob1;
  reset();
  if( newx < 0 || newy < 0 )
    return -1;
  ob = getMiso( elnu );
  if( ob == 0 ) return -1;
  for( i=0; i<n_el; i++ ) {
    if( i == elnu ) continue;
    cx = cy = -2;
    ob1 = getMiso( i );
    if( ob1 == 0 ) continue;
    ob1->getDataSI( "vis_x", &cx, 0 ); ob1->getDataSI( "vis_y", &cy, 0 );
    if( newx == cx && newy == cy ) return -1;
  };  
  ob->setDataSI( "vis_x", newx, 0 );
  ob->setDataSI( "vis_y", newy, 0 );
  return 0;
}/*}}}1*/

int TModel::linkNames(void)/*{{{1*/
{
  int i, j, k, maxli, maxlp, oord, iin, ipa, ipn, flg,
      elnu, ob_lock, ob_noauto, ob_first, ob_last, out_tp, out_nu;
  static const char* i_names[] = {"inps0","inps1","inps2","inps3"};
  static const char* p_names[] = {"pinps0","pinps1","pinps2","pinps3"};
  static const char* n_names[] = {"pnames0","pnames1","pnames2","pnames3"};
  static const char* f_names[] = {"pflags0","pflags1","pflags2","pflags3"};
  char lname[MAX_NAMELEN], pname[MAX_NAMELEN], nname[MAX_NAMELEN];
  char oname[MAX_NAMELEN];
  TDataSet* ob; TElmLink* lob; TOutArr *arr;
  n_el = n_out = n_graph = 0;    
  v_el.clear(); v_ord.clear(); v_flg.clear();
  v_out.clear(); v_oute.clear(); v_outt.clear();
  v_graph.clear();
  inps.clear(); pinps.clear(); pnames.clear(); pflags.clear();
  for( i=0; i<nelm; i++ ) { // find elements of given types
    if( d_i[i].tp != dtpObj ) continue;
    ob = static_cast<TDataSet*> (ptrs[i]);
    if( ob == 0 ) continue;
    if( ob->isChildOf( "TMiso" )) {
      v_el.push_back( i ); oord = -1;
      ob->getDataSI( "ord", &oord, 0 );
      v_ord.push_back( oord );
      n_el++; continue;
    };
    if( ob->isChildOf( "TOutArr" )) {
      v_out.push_back( i );
      n_out++; continue;
    };
    if( ob->isChildOf( "TGraph" )) {
      v_graph.push_back( i );
      n_graph++; continue;
    };
  }; // end of elems counter
  sortOrd();
  for( i=0; i<n_el; i++ ) { // fill link indexes
    inps.push_back( li_el()  );    pinps.push_back( li_el()  );
    pnames.push_back( li_el()  );  pflags.push_back( li_el()  );
    ob = static_cast<TMiso*>(ptrs[v_el[i]]);
    k = ob->getDataIdx( "links" );
    if( k < 0 ) continue;
    lob = static_cast<TElmLink*>(ob->getObj( k )); maxli = maxlp = 0;
    ob_lock = ob_noauto = 0; k = 0;
    lob->getDataSI( "locked", &ob_lock, 0 );
    lob->getDataSI( "noauto", &ob_noauto, 0 );
    lob->getDataSI( "onlyFirst", &ob_first, 0 );
    lob->getDataSI( "onlyLast", &ob_last, 0 );
    if( ob_lock )  k |= 1; if( ob_noauto ) k |= 2;
    if( ob_first ) k |= 4; if( ob_last ) k |= 8;
    v_flg.push_back( k );
    for( j=0; j<4; j++ ) {
      lname[0] = 0;  pname[0] = 0; nname[0] = 0;
      lob->getDataSS( i_names[j], lname, MAX_NAMELEN, 0 );
      lob->getDataSS( p_names[j], pname, MAX_NAMELEN, 0 );
      lob->getDataSS( n_names[j], nname, MAX_NAMELEN, 0  );
      lob->getDataSI( f_names[j], &flg, 0 );
      if( lname[0] == 0 || ( iin = oname2elnu( lname ) ) == -1 ) {
	inps[i].l[j] = -1;
      } else {
        inps[i].l[j] = iin;
        maxli = j+1;
      };
      if( pname[0] == 0 || ( ipa = oname2elnu( pname ) ) == -1 ||
			   ( ipn = ob->getDataIdx( nname )) < 0 ) {
	pinps[i].l[j] = -1; pnames[i].l[j] = -1;
      } else {
        pinps[i].l[j] = ipa;
        pnames[i].l[j] = ipn;
	pflags[i].l[j] = flg;
	maxlp = j+1;
      };
    };
    inps[i].maxl = maxli;   pinps[i].maxl = maxlp;
  };
  // fill outs elnus and types
  for( out_nu=0; out_nu<n_out; out_nu++ ) {
    arr = getOutArr( out_nu );
    if( arr == 0 ) { v_oute.push_back(-1); v_outt.push_back(-1); };
    oname[0] = 0; out_tp = -1;
    arr->getDataSS( "name", oname, MAX_NAMELEN, 0 );
    arr->getDataSI( "type", &out_tp, 0 );
    elnu = oname2elnu( oname );
    v_oute.push_back( elnu ); v_outt.push_back( out_tp );
  };
  return 0;
}/*}}}1*/

void TModel::sortOrd(void)/*{{{1*/
{
  int i, n, en, t;
  if( n_el < 2 )
    return;
  en = 0; n = n_el;
  while( en == 0 ) { // simple bubble sort TODO: use STL sort
     en = 1;
     for( i=0; i<n-1; i++ ) {
        if( v_ord[i] > v_ord[i+1] ) {
          t = v_ord[i]; v_ord[i] = v_ord[i+1]; v_ord[i+1] = t;
          t = v_el[i];  v_el[i] = v_el[i+1];   v_el[i+1] = t;
          en = 0;
        };
     };
     n--;
  };
}/*}}}1*/

int TModel::hintOrd(void) const/*{{{1*/
{
  int i, mt, m = 0, m1;
  for( i=0; i<n_el; i++ ) {
    mt = v_ord[i];	  
    if( m < mt ) 
      m = mt;
  };
  m1 = ( (m+10) / 10 ) * 10;
  return m1;
}/*}}}1*/


int TModel::getLinkInfos( int elnu, LinkInfo *li )/*{{{1*/
{
  int i, l_elnu, vx, vy, flip;
  TMiso *ob;
  if( li == 0 )
    return -1;
  for( i=0; i<8; i++ ) {  // reset all fields to default
    li[i].ltype = LinkNone; li[i].elnu = li[i].x = li[i].y = -1;
    li[i].eflip = 0; li[i].pflags = 0;
  };
  if( elnu < 0 || elnu >= n_el )
    return -1;
  for( i=0; i<inps[elnu].maxl && i<4; i++ ) {  // inputs
    l_elnu = inps[elnu].l[i];
    if( l_elnu == -1 || l_elnu >= n_el ) 
      continue;
    if( l_elnu >= 0 ) { // real input
      ob = getMiso( l_elnu );
      if( ob == 0 ) continue;
      vx = vy = -1; flip = 0;
      ob->getDataSI( "vis_x", &vx, 0 ); ob->getDataSI( "vis_y", &vy, 0 );
      ob->getDataSI( "links.flip", &flip, 0 ); 
      if( vx < 0 || vy < 0 ) continue;
      li[i].ltype = LinkElm; li[i].elnu = l_elnu;
      li[i].x = vx; li[i].y = vy; li[i].eflip = flip;
    } else {  // special input or error;
      li[i].ltype = (l_elnu<-9) ? LinkSpec : LinkBad; li[i].elnu = l_elnu;
    };    
  };

  for( i=0; i<pinps[elnu].maxl && i<4; i++ ) {  // parm inputs
    l_elnu = pinps[elnu].l[i];
    if( l_elnu == -1 || l_elnu >= n_el ) 
      continue;
    if( l_elnu >= 0 ) { // real parm input
      ob = getMiso( l_elnu );
      if( ob == 0 ) continue;
      vx = vy = -1;
      ob->getDataSI( "vis_x", &vx, 0 ); ob->getDataSI( "vis_y", &vy, 0 );
      ob->getDataSI( "links.flip", &flip, 0 ); 
      if( vx < 0 || vy < 0 ) continue;
      li[i+4].ltype = LinkElm; li[i+4].elnu = l_elnu;
      li[i+4].x = vx; li[i+4].y = vy; li[i+4].eflip = flip;
      li[i+4].pflags = pflags[elnu].l[i];
    } else {  // special input or error;
      li[i+4].ltype = (l_elnu<-9) ? LinkSpec : LinkBad; li[i+4].elnu = l_elnu;
    };    
  };
  return 0;

}/*}}}1*/

void TModel::fillCommon(void)/*{{{1*/
{
  int i = 0;
  if( d_i ==0 || nelm != 0 ) return;
  ptrs.clear();
  memset( d_i, 0, sizeof(TDataInfo) * MAX_DATAELEM );
  // - dialog
  d_i[i].tp = dtpDial; 
  d_i[i].dlg_w = 620; d_i[i].dlg_h=470;
  strcpy( d_i[i].name, "modeldlg" ); d_i[i].listdata = "Model parameters";
  i++; ptrs.push_back( 0 );
  // - label tt
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 30; d_i[i].dlg_y = 10;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_tt" ); d_i[i].listdata = "T";
  i++; ptrs.push_back( 0 );
  // - enter tt 
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 20; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "tt" ); d_i[i].descr = "Time";
  i++; ptrs.push_back( &tt );
  // - label nn
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 130; d_i[i].dlg_y = 10;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_nn" ); d_i[i].listdata = "N";
  i++; ptrs.push_back( 0 );
  // - enter nn
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 120; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  d_i[i].v_max = 1000000;
  strcpy( d_i[i].name, "nn" ); d_i[i].descr = "number of steps";
  i++; ptrs.push_back( &nn );
  // - label nl1
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 230; d_i[i].dlg_y = 10;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_nl1" ); d_i[i].listdata = "N1 (prm0)";
  i++; ptrs.push_back( 0 );
  // - enter nl1
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 220; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  d_i[i].v_max = 5000;
  strcpy( d_i[i].name, "nl1" ); d_i[i].descr = "Loops number on parm 0";
  i++; ptrs.push_back( &nl1 );
  // - label nl2
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 330; d_i[i].dlg_y = 10;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_nl1" ); d_i[i].listdata = "N2 (prm1)";
  i++; ptrs.push_back( 0 );
  // - enter nl2
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 320; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  d_i[i].v_max = 5000;
  strcpy( d_i[i].name, "nl2" ); d_i[i].descr = "Loops number on parm 1";
  i++; ptrs.push_back( &nl2 );
  // - label n_steps 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 430; d_i[i].dlg_y = 10;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_n_steps" ); d_i[i].listdata = "N steps";
  i++; ptrs.push_back( 0 );
  // - enter n_steps
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 420; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  d_i[i].v_max = 10000;
  strcpy( d_i[i].name, "n_steps" ); 
  d_i[i].listdata = "Loops of steps per i/o";
  i++; ptrs.push_back( &n_steps );
  // - switch use_sync
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsSwitch; 
  d_i[i].dlg_x = 510; d_i[i].dlg_y = 30;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "use_sync" ); 
  d_i[i].listdata = "Sync time";
  i++; ptrs.push_back( &use_sync );
  // ------------------------- line ------------------------------
  // - label prm0s
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 30; d_i[i].dlg_y = 60;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm0s" ); d_i[i].listdata = "prm 0";
  i++; ptrs.push_back( 0 );
  // - enter prm0s 
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 20; d_i[i].dlg_y = 80;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm0s" ); d_i[i].descr = "Parametr 0 start";
  i++; ptrs.push_back( &prm0s );
  // - label prm1s
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 130; d_i[i].dlg_y = 60;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm1s" ); d_i[i].listdata = "prm 1";
  i++; ptrs.push_back( 0 );
  // - enter prm1s 
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 120; d_i[i].dlg_y = 80;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm1s" ); d_i[i].descr = "Parametr 1 start";
  i++; ptrs.push_back( &prm1s );
  // - label prm2s
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 230; d_i[i].dlg_y = 60;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm2s" ); d_i[i].listdata = "prm 2";
  i++; ptrs.push_back( 0 );
  // - enter prm2s 
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 220; d_i[i].dlg_y = 80;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm2s" ); d_i[i].descr = "Parametr 2";
  i++; ptrs.push_back( &prm2s );
  // - label prm3s
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 330; d_i[i].dlg_y = 60;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm3s" ); d_i[i].listdata = "prm 3";
  i++; ptrs.push_back( 0 );
  // - enter prm3s
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 320; d_i[i].dlg_y = 80;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm3s" ); d_i[i].descr = "Parametr 3";
  i++; ptrs.push_back( &prm3s );
  // ------------------------------ line --------------------------
  // - label prm0d
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 30; d_i[i].dlg_y = 110;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm0d" ); d_i[i].listdata = "d_prm0";
  i++; ptrs.push_back( 0 );
  // - enter prm0d
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 20; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm0d" ); d_i[i].descr = "Parametr 0 delta";
  i++; ptrs.push_back( &prm0d );
  // - label prm1d
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 130; d_i[i].dlg_y = 110;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_prm1d" ); d_i[i].listdata = "d_prm1";
  i++; ptrs.push_back( 0 );
  // 23- enter prm1d
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 120; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "prm1d" ); d_i[i].descr = "Parametr 1 delta";
  i++; ptrs.push_back( &prm1d );
  // - label xval1 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 230; d_i[i].dlg_y = 110;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_xval1" ); d_i[i].listdata = "xval1";
  i++; ptrs.push_back( 0 );
  // - enter xval1
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 220; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "xval" ); d_i[i].descr = "xval1";
  i++; ptrs.push_back( &xval1 );
  // - label xval2 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 330; d_i[i].dlg_y = 110;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_xval2" ); d_i[i].listdata = "xval2";
  i++; ptrs.push_back( 0 );
  // 27- enter xval2
  d_i[i].tp = dtpDbl; d_i[i].dlg_x = 320; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "xval2" ); d_i[i].descr = "xval2";
  i++; ptrs.push_back( &xval2 );
  // - label seed 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 430; d_i[i].dlg_y = 110;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_seed" ); d_i[i].listdata = "Seed";
  i++; ptrs.push_back( 0 );
  // - enter seed 
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 420; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  d_i[i].v_min = -1;
  d_i[i].v_max = 0x7FFFFFFF;
  strcpy( d_i[i].name, "seed" ); d_i[i].descr = "Random seed";
  i++; ptrs.push_back( &seed );
  // - switch: useSameSeed -- obsoleted
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsSw; d_i[i].flags = efNoDial | efRO;
  // d_i[i].dlg_x = 510; d_i[i].dlg_y = 130;
  // d_i[i].dlg_w = 100; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "useSameSeed" ); d_i[i].descr = "";
  d_i[i].listdata = "Same seed";
  i++; ptrs.push_back( &useSameSeed );
  // list: seed type
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList;
  d_i[i].dlg_x = 510; d_i[i].dlg_y = 130;
  d_i[i].dlg_w = 100; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "seedType" ); d_i[i].descr = "Seeding type";
  d_i[i].max_len = 3;
  d_i[i].listdata = "Every Run\nStart 1d loop\nStart 2d loop\n";
  i++; ptrs.push_back( &seedType );
  
  // ------------------------- line ------------------------------
  // - group inputs 
  d_i[i].tp = dtpGroup; d_i[i].dlg_x = 16; d_i[i].dlg_y = 160;
  d_i[i].dlg_w = 588; d_i[i].dlg_h = 70;
  strcpy( d_i[i].name, "grp_ic" ); d_i[i].listdata = "Input indexes";
  i++; ptrs.push_back( 0 );
  
  // - label ic_mouse 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 30; d_i[i].dlg_y = 180;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_ic_mouse" ); d_i[i].listdata = "Mouse";
  i++; ptrs.push_back( 0 );
  // - enter ic_mouse
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 20; d_i[i].dlg_y = 200;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "ic_mouse" ); d_i[i].descr = "Mouse channel";
  i++; ptrs.push_back( &ic_mouse );
  // - label ic_joy 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 130; d_i[i].dlg_y = 180;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_ic_joy" ); d_i[i].listdata = "Joystick";
  i++; ptrs.push_back( 0 );
  // - enter ic_joy
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 120; d_i[i].dlg_y = 200;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "ic_joy" ); d_i[i].descr = "Joystick channel";
  i++; ptrs.push_back( &ic_joy );
  // - label ic_sound 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 230; d_i[i].dlg_y = 180;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_ic_sound" ); d_i[i].listdata = "sound";
  i++; ptrs.push_back( 0 );
  // - enter ic_sound
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 220; d_i[i].dlg_y = 200;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "ic_sound" ); d_i[i].descr = "sound channel";
  i++; ptrs.push_back( &ic_sound );
  // - label ic_key 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 330; d_i[i].dlg_y = 180;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_ic_key" ); d_i[i].listdata = "keyboard";
  i++; ptrs.push_back( 0 );
  // - enter ic_key
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 320; d_i[i].dlg_y = 200;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "ic_key" ); d_i[i].descr = "key channel";
  i++; ptrs.push_back( &ic_key );
  // - label ic_aux 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 430; d_i[i].dlg_y = 180;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_ic_aux" ); d_i[i].listdata = "aux";
  i++; ptrs.push_back( 0 );
  // - enter ic_aux
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 420; d_i[i].dlg_y = 200;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "ic_aux" ); d_i[i].descr = "aux channel";
  i++; ptrs.push_back( &ic_aux );
  // ------------------------- line ------------------------------
  // - group outputs 
  d_i[i].tp = dtpGroup; d_i[i].dlg_x = 16; d_i[i].dlg_y = 236;
  d_i[i].dlg_w = 588; d_i[i].dlg_h = 90;
  strcpy( d_i[i].name, "grp_ic" ); d_i[i].listdata = "Outputs";
  i++; ptrs.push_back( 0 );
  
  // - label oc_0 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 30; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_0" ); d_i[i].listdata = "0";
  i++; ptrs.push_back( 0 );
  // - enter oc_0
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 20; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_0" ); d_i[i].descr = "Channel 0";
  i++; ptrs.push_back( &oc_0 );
  // - label oc_1 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 130; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_1" ); d_i[i].listdata = "1";
  i++; ptrs.push_back( 0 );
  // - enter oc_1
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 120; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_1" ); d_i[i].descr = "Channel 1";
  i++; ptrs.push_back( &oc_1 );
  // - label oc_2 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 250; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_2" ); d_i[i].listdata = "2";
  i++; ptrs.push_back( 0 );
  // - enter oc_2
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 220; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_2" ); d_i[i].descr = "Channel 2";
  i++; ptrs.push_back( &oc_2 );
  // - label oc_3 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 330; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_3" ); d_i[i].listdata = "3";
  i++; ptrs.push_back( 0 );
  // - enter oc_3
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 320; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_3" ); d_i[i].descr = "Channel 3";
  i++; ptrs.push_back( &oc_3 );
  // - label oc_4 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 430; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_4" ); d_i[i].listdata = "4";
  i++; ptrs.push_back( 0 );
  // - enter oc_4
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 420; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_4" ); d_i[i].descr = "Channel 4";
  i++; ptrs.push_back( &oc_4 );
  // - label oc_5 
  d_i[i].tp = dtpLabel; d_i[i].dlg_x = 530; d_i[i].dlg_y = 250;
  d_i[i].dlg_w = 70; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "l_oc_5" ); d_i[i].listdata = "5";
  i++; ptrs.push_back( 0 );
  // - enter oc_5
  d_i[i].tp = dtpInt; d_i[i].dlg_x = 520; d_i[i].dlg_y = 270;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oc_5" ); d_i[i].descr = "Channel 5";
  i++; ptrs.push_back( &oc_5 );
  // ------------------------- line ------------------------------
  static const char *och_type = 
    "None\nCross1\nCross2\nCross3\n"
    "Vbar0\nVbar1\nVbar2\nVbar3\n"
    "Gbar0\nGbar1\nGbar2\nGbar3\n"
    "LED0\nLED1\nLED2\nLED3\n"
    "Sound0\nSound1\n"
    "Aux0\nAux1";
  // - list oct_0
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 20; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_0" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 0";
  i++; ptrs.push_back( &oct_0 );
  // - list oct_1
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 120; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_1" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 1";
  i++; ptrs.push_back( &oct_1 );
  // - list oct_2
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 220; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_2" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 2";
  i++; ptrs.push_back( &oct_2 );
  // - list oct_3
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 320; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_3" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 3";
  i++; ptrs.push_back( &oct_3 );
  // - list oct_4
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 420; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_4" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 4";
  i++; ptrs.push_back( &oct_4 );
  // - list oct_5
  d_i[i].tp = dtpInt; d_i[i].subtp = dtpsList; d_i[i].max_len = 20; 
  d_i[i].dlg_x = 520; d_i[i].dlg_y = 296;
  d_i[i].dlg_w = 80; d_i[i].dlg_h = 20;
  strcpy( d_i[i].name, "oct_5" ); 
  d_i[i].listdata = och_type;  d_i[i].descr = "Channel 5";
  i++; ptrs.push_back( &oct_5 );
  // ------------------------- line ------------------------------
  // - long_descr
  d_i[i].tp = dtpStr; d_i[i].subtp = dtpsMline; d_i[i].max_len = 1024;
  strcpy( d_i[i].name, "long_descr" );
  d_i[i].dlg_x = 20;  d_i[i].dlg_y = 330;
  d_i[i].dlg_w = 580; d_i[i].dlg_h = 90;
  i++; ptrs.push_back( long_descr );
  
  // ------------------------- line ------------------------------
  // -- button Ok
  d_i[i].tp = dtpButton; d_i[i].subtp = 0;
  d_i[i].dlg_x = 20; d_i[i].dlg_y = 430;
  d_i[i].dlg_w = 100; d_i[i].dlg_h = 30;
  strcpy( d_i[i].name, "btn_ok" ); d_i[i].listdata = "Ok";
  i++; ptrs.push_back( 0 );
  // -- button Cancel
  d_i[i].tp = dtpButton; d_i[i].subtp = 1; 
  d_i[i].dlg_x = 250; d_i[i].dlg_y = 430;
  d_i[i].dlg_w = 100; d_i[i].dlg_h = 30;
  strcpy( d_i[i].name, "btn_can" ); d_i[i].listdata = "Cancel";
  i++; ptrs.push_back( 0 );
  // -- button Help 
  d_i[i].tp = dtpButton; d_i[i].subtp = 2; 
  d_i[i].dlg_x = 500; d_i[i].dlg_y = 430;
  d_i[i].dlg_w = 100; d_i[i].dlg_h = 30;
  strcpy( d_i[i].name, "btn_help" ); d_i[i].listdata = "Help";
  i++; ptrs.push_back( 0 );
  // ============================== end dialog ========================
  // - hide ii 
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "ii" ); 
  i++; ptrs.push_back( &ii );
  // - hide il1 
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "il1" ); 
  i++; ptrs.push_back( &il1 );
  // - hide il2 
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "il2" ); 
  i++; ptrs.push_back( &il2 );
  // - hide n_tot; 
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "n_tot" ); 
  i++; ptrs.push_back( &n_tot );
  // - hide i_tot; 
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "i_tot" ); 
  i++; ptrs.push_back( &i_tot );
  // - hide prm0 
  d_i[i].tp = dtpDbl; d_i[i].flags = efNoDial | efNoSave;
  strcpy( d_i[i].name, "prm0" ); 
  i++; ptrs.push_back( &prm0 );
  // - hide prm1 
  d_i[i].tp = dtpDbl; d_i[i].flags = efNoDial | efNoSave;
  strcpy( d_i[i].name, "prm1" ); 
  i++; ptrs.push_back( &prm1 );
  // - hide t 
  d_i[i].tp = dtpDbl; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "t" ); 
  i++; ptrs.push_back( &t );
  // - hide tdt 
  d_i[i].tp = dtpDbl; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "dt" ); 
  i++; ptrs.push_back( &tdt );
  // - hide rtime 
  d_i[i].tp = dtpDbl; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "rtime" ); 
  i++; ptrs.push_back( &rtime );
  // - hide sgnt
  d_i[i].tp = dtpInt; d_i[i].flags = efRO | efNoDial | efNoSave;
  strcpy( d_i[i].name, "sgnt" );
  i++; ptrs.push_back( &sgnt );
  // - end
  d_i[i].tp = dtpEnd;
  initHash();
}/*}}}1*/

TDataSet* TModel::create( TDataSet* apar ) // static/*{{{1*/
{
   return new TModel( apar );
}/*}}}1*/

int TModel::getClassId(void) const/*{{{1*/
{
   return CLASS_ID_TModel;
}/*}}}1*/

const char* TModel::getClassName(void) const/*{{{1*/
{
   return "TModel";
}/*}}}1*/

const TClassInfo* TModel::getClassInfo(void) const/*{{{1*/
{
  return &class_info;
}/*}}}1*/

const char *TModel::getHelp(void) const/*{{{1*/
{
  return helpstr;
}/*}}}1*/

int TModel::saveDatas( ostream *os )/*{{{1*/
{
  int i;
  // for debug: list elements and links TODO: must depend on debug level
  *os << "%!m model: n_el= " << n_el << " n_out= " << n_out 
      << " n_graph = " << n_graph << "\n%!m  elems:\n";
  for( i=0; i<n_el; i++ ) {
    *os << "%!m e " << i << '[' << v_el[i] << "] " << v_ord[i] << ' '
        << v_flg[i] << '\n';
  };
  for( i=0; i<n_el; i++ ) {
    *os << "%!m   i" << i << ' ' << inps[i].maxl << ": "
        << inps[i].l[0] << ' ' << inps[i].l[1] << ' '
        << inps[i].l[2] << ' ' << inps[i].l[3] << ' '
        << '\n';
    *os << "%!m   p" << i << ' '  << pinps[i].maxl << ' '
        << pinps[i].l[0] << ' ' << pinps[i].l[1] << ' '
        << pinps[i].l[2] << ' ' << pinps[i].l[3] << " | "
        << pnames[i].l[0] << ' ' << pnames[i].l[1] << ' '
        << pnames[i].l[2] << ' ' << pnames[i].l[3] << ' '
        << '\n';
  };
  *os << "%!m -- outs:\n";
  for( i=0; i<n_out; i++ ) {
    *os << "%!m o " << i << '[' << v_out[i] << ']' << ' '
        << v_oute[i] << ' ' << v_outt[i] << '\n';
  };
  i = TDataContainer::saveDatas( os );
  // *os << "\n# end of TModel\n";
  return i;
}/*}}}1*/

// end of tmodel.cpp

