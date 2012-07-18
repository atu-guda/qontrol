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
#include <algorithm>
#include <limits>
#include "miscfun.h"
#include "tmodel.h"

using namespace std;

const char* TModel::helpstr = "<H1>TModel</H1>\n"
 "Hold all active elements, output arrays and graph descriptions";

static const int RESERVED_OUTS = 1024;

TClassInfo TModel::class_info = {
 CLASS_ID_TModel, "TModel", TModel::create,
 &TDataContainer::class_info, helpstr, clpSpecial | clpContainer };

TModel::TModel( TDataSet* aparent )/*{{{1*/
       :TDataContainer( aparent ), 
         vars( MODEL_NVAR, 0 )
{
  end_loop = 0;
  tt = 100; nn = n_tot= 100000; nl1 = 1; nl2 = 1; n_steps=1000; use_sync = 0;
  prm0s = prm1s = prm2s = prm3s = 0.1; 
  prm0d = prm1d = 0.01; xval1 = xval2 = 0;
  seed = 117; useSameSeed = 1; seedType = 0;
  ic_mouse = ic_joy = ic_sound = ic_key = ic_aux = -1;
  oc_0 = oc_1 = oc_2 = oc_3 = oc_4 = oc_5 = -1;
  oct_0 = oct_1 = oct_2 = oct_3 = oct_4 = oct_5 = 0;
  ii = il1 = il2 = i_tot = 0;
  rtime = t = 0; tdt = tt / nn; 
  prm0 = prm1 = prm2 = prm3 = 0; start_time = 0; 
  prm0s = prm1s = 0.1; prm0d = prm1d = 0.01; sgnt = -3;
  m_sqrt2 = sqrt(2.0);
  m_sqrt1_2 = sqrt(0.5);
  one = 1.0;
  m_PI = M_PI;
  m_E = M_E;
  const int ELM_RES = 64; const int OUT_RES = 32;
  v_el.reserve( ELM_RES ); v_ord.reserve( ELM_RES );
  v_out.reserve( OUT_RES ); 
  v_outt.reserve( OUT_RES );
  v_graph.reserve( 16 );
  fillCommon();
  
}/*}}}1*/

TModel::~TModel()/*{{{1*/
{
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
  int i, rc;
  if( csteps < 1 ) 
    csteps = 1;  

  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; i++, i_tot++ ) {
    prm0 = prm0s + il1 * prm0d; prm1 = prm1s + il2 * prm1d;
    if( ii == 0 ) {    // --------------------- start inner loop --
      if( il1 == 0 ) { // start prm0 loop
	resetOutArrs( 1 );
      };

      resetOutArrs( 0 );
      start_time = get_real_time(); rtime = t = 0;
      // set start param
      allStartLoop( il1, il2 );
    };// end start inner loop

    rc = runOneLoop();
    if( rc == 1 )
      return 0;    

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
  int out_level;
  unsigned long wait_ms;
  rtime = get_real_time() - start_time;
  if( use_sync ) {
     if( t > rtime ) {
       wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms );
       // return 1;
     };
  };  
  IterType itype = IterMid;
  out_level = 0;
  if( ii == 0 ) {
    itype = IterFirst;
  } else if( ii == nn-1 ) {
    itype = IterLast;
    out_level = run_type;
  };

  int elnu = 0;
  for( TMiso* cur_el : v_el ) {
    if( end_loop )
      break;
     
     cur_el->fun( t, itype );  // <============ main action TODO: from ^ to fun
     ++elnu;
  };  // end element loop;
  for( TOutArr* arr : v_out ) { 
    arr->take_val( out_level );
  };
  t += tdt; ii++;
  return 0;
}/*}}}1*/

int TModel::preRun( int run_tp, int anx, int any )/*{{{1*/
{
  int rc;
  tdt = tt / nn;
  
  state = stateRun;
  for( TMiso *ob : v_el ) {
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
  int cm = 0;
  for( TMiso *ob : v_el ) {
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
  linkNames();
  for( TOutArr *arr : v_out ) {
    if( arr == 0 ) 
      continue; // never be!
    arr->free();
  };
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}/*}}}1*/

int TModel::allStartLoop( int acnx, int acny )/*{{{1*/
{
  int rc;
  for( TMiso *ob : v_el ) {
    if( ob == 0 ) 
      continue;
    rc = ob->startLoop( acnx, acny );
    if( rc != 0 )
      return rc;
  };
  return 0;
}/*}}}1*/

void TModel::allEndLoop(void)/*{{{1*/
{
  for( TMiso *ob : v_el ) {
    if( ob == nullptr ) 
      continue;
    ob->endLoop();
  };
}/*}}}1*/

void TModel::allocOutArrs( int tp )/*{{{1*/
{
  int out_tp;
  if( tp < 0 || tp > 2 )
    return;
  for( TOutArr *arr: v_out ) { // alloc output array
    if( !arr ) 
      continue;
    out_tp = -1;
    arr->getData( "type", &out_tp );
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
  for( TOutArr *arr : v_out ) { 
    if( !arr ) 
      continue;
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

int TModel::oname2elnu( const QString &iname ) const/*{{{1*/
{
  ltype_t lt;
  const double *dp;
  if( iname.isEmpty() )
    return -1;
  dp = getDoublePtr( iname, &lt );
  if( !dp || lt == LinkBad )
    return -2;
  if( lt == LinkSpec ) {  
    return -12; 
  };  

  int i = 0;
  for( TMiso *ob : v_el ) {
    if( ob->objectName() == iname )
      return i;
    ++i;
  };
  return -2;
}/*}}}1*/

int TModel::outname2out_nu( const QString &iname ) const/*{{{1*/
{
  int i = 0;
  for( TOutArr *arr : v_out ) {
    if( arr && arr->objectName() == iname )
      return i;
    ++i;
  };
  return -1;
}/*}}}1*/


int TModel::ord2elnu( int aord ) const/*{{{1*/
{
  for( unsigned i=0; i<v_ord.size(); i++ ) {
    if( v_ord[i] == aord )
      return i;
  };	  
  return -1;
}/*}}}1*/

int TModel::fback( int code, int /* aord */, const QString & /* tdescr */ )/*{{{1*/
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

int TModel::xy2elnu( int avis_x, int avis_y )/*{{{1*/ // TODO: todel
{
  int i = 0, ox, oy;
  for( TMiso *ob : v_el ) {
    if( ob == 0 ) 
      continue;
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) 
      return i;
    ++i;
  };	  
  return -1;
}/*}}}1*/

TMiso* TModel::xy2Miso( int avis_x, int avis_y ) const/*{{{1*/
{
  int  ox, oy;
  for( TMiso *ob : v_el ) {
    if( !ob )
      continue;
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) 
      return ob;
  }
  return nullptr;
}/*}}}1*/

int TModel::getNMiso(void) const/*{{{1*/
{
  return v_el.size();
}/*}}}1*/

TMiso* TModel::getMiso( int elnu )/*{{{1*/
{
  if( elnu < 0 || elnu >= (int)v_el.size() )
    return nullptr;
  return v_el[elnu];
}/*}}}1*/

int TModel::getNOutArr(void) const/*{{{1*/
{
  return v_out.size();
}/*}}}1*/

TOutArr* TModel::getOutArr( int out_nu )/*{{{1*/
{
  if( out_nu < 0 || out_nu >= (int)v_out.size() )
    return nullptr;
  return v_out[out_nu];
}/*}}}1*/

int TModel::getNGraph(void) const/*{{{1*/
{
  return v_graph.size();
}/*}}}1*/

TGraph* TModel::getGraph( int gra_nu )/*{{{1*/
{
  if( gra_nu < 0 || gra_nu >= (int)v_graph.size() )
    return nullptr;
  return v_graph[gra_nu];
}/*}}}1*/

int TModel::insElem( const QString &cl_name, const QString &ob_name,
                     int aord, int avis_x, int avis_y )/*{{{1*/
{
  TMiso *ob;
  ob = static_cast<TMiso*>( add_obj( cl_name, ob_name ) );
  if( ob == 0 || ob->isChildOf( "TMiso" ) == 0 ) 
    return -1;
  ob->setData( "ord", aord );
  ob->setData( "vis_x", avis_x );  
  ob->setData( "vis_y", avis_y );
  reset();
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::insOut( const QString &outname, const QString &objname )/*{{{1*/
{
  TOutArr *arr = static_cast<TOutArr*>( add_obj( "TOutArr", outname ) );
  if( !arr || arr->isChildOf( "TOutArr" ) == 0 ) 
    return -1;
  arr->setData( "name", objname );
  
  QString lbl = objname;
  if( lbl.left(4) == "out_" )
    lbl.remove(0,4);
  if( lbl.left(1) == ":" )
    lbl.remove(0,1);

  arr->setData( "label", lbl );
  
  reset();
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::insGraph( const QString &gname )/*{{{1*/
{
  TGraph *gra = static_cast<TGraph*>( add_obj( "TGraph", gname ) );
  if( !gra || gra->isChildOf( "TGraph" ) == 0 ) 
    return -1;
  reset();
  modified |= 1;
  return 0;
}/*}}}1*/

int TModel::delElem( const QString &ename )/*{{{1*/
{
  TMiso *ob = qobject_cast<TMiso*>(getObj( ename, "TMiso" ));
  if( !ob ) {
    qDebug( "TModel::delElem: fail to find TMiso \"%s\"", qPrintable(ename) );
    return 0;
  }
  int rc = del_obj( ename );
  if( rc ) {
    reset();
    modified |= 1;
  }
  return rc;
}/*}}}1*/


int TModel::delOut( int out_nu )/*{{{1*/
{
  int k;
  if( out_nu < 0 || out_nu >= (int)v_out.size() )
    return -1;
  k = del_obj( v_out[out_nu]->objectName() );
  reset();
  modified |= 1;
  return k;
}/*}}}1*/

int TModel::delGraph( int gr_nu )/*{{{1*/
{
  int k;
  if( gr_nu < 0 || gr_nu >= (int)v_graph.size() )
    return -1;
  k = del_obj( v_graph[gr_nu]->objectName() );
  reset();
  modified |= 1;
  return k;
}/*}}}1*/

int TModel::newOrder( const QString &name, int new_ord )/*{{{1*/
{
  TMiso *ob = qobject_cast<TMiso*>( getObj( name, "TMiso" ) );
  if( !ob )
    return -1;
  int k = ord2elnu( new_ord );
  if( k >= 0 )
    return -1;
  ob->setData( "ord", new_ord );
  ob->getData( "ord", &k );
  reset();
  modified |= 1;
  return ( k == new_ord ) ? 0 : -1;
}/*}}}1*/

int TModel::moveElem( int elnu, int newx, int newy )/*{{{1*/
{
  int cx, cy;
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 )
    return -1;
  ob = getMiso( elnu );
  if( ob == 0 ) return -1;
  for( unsigned i=0; i<v_el.size(); i++ ) {
    if( (int)i == elnu ) continue;
    cx = cy = -2;
    ob1 = getMiso( i );
    if( ob1 == 0 ) continue;
    ob1->getData( "vis_x", &cx ); ob1->getData( "vis_y", &cy );
    if( newx == cx && newy == cy ) return -1;
  };  
  ob->setData( "vis_x", newx );
  ob->setData( "vis_y", newy );
  reset();
  return 0;
}/*}}}1*/

int TModel::linkNames(void)/*{{{1*/
{
  int oord, out_tp;
  QString lname, pname, nname, oname;
  v_el.clear(); v_ord.clear();
  v_out.clear(); v_outt.clear();
  v_graph.clear();
  
  HolderData* ho; HolderObj *hob;      
  for( QObject* child : children() ) { // find elements of given types: TODO:
    ho = qobject_cast<HolderData*>( child );
    if( !ho || !ho->isObject()  )
      continue;
    hob = qobject_cast<HolderObj*>(ho);
    if( !hob )
      continue;
    TDataSet* ds = hob->getObj();
    
    if( ds->isChildOf( "TMiso" )) {
      v_el.push_back( qobject_cast<TMiso*>(ds) );
      oord = -1;
      ds->getData( "ord", &oord );
      v_ord.push_back( oord );
      continue;
    };
    if( ds->isChildOf( "TOutArr" )) {
      v_out.push_back( qobject_cast<TOutArr*>(ds) );
      continue;
    };
    if( ds->isChildOf( "TGraph" )) {
      v_graph.push_back( qobject_cast<TGraph*>(ds) );
      continue;
    };

  }

  sortOrd();

  // fill outs elnus and types
  for( TOutArr *arr : v_out ) {
    if( arr == 0 ) { // FIXME
      v_outt.push_back(-1); 
      continue;
    }; 
    oname = ""; out_tp = -1;
    arr->getData( "type", &out_tp );
    v_outt.push_back( out_tp );
  };
  return 0;
}/*}}}1*/

void TModel::sortOrd(void)/*{{{1*/
{
  int i, n, en, t;
  TMiso *tob;
  if( v_el.size() < 2 )
    return;
  en = 0; n = v_el.size();
  while( en == 0 ) { // simple bubble sort TODO: use STL sort
     en = 1;
     for( i=0; i<n-1; i++ ) {
        if( v_ord[i] > v_ord[i+1] ) {
          t = v_ord[i];   v_ord[i] = v_ord[i+1]; v_ord[i+1] = t;
          tob = v_el[i];  v_el[i] = v_el[i+1];   v_el[i+1] = tob;
          en = 0;
        };
     };
     n--;
  };
}/*}}}1*/

int TModel::hintOrd(void) const/*{{{1*/
{
  vector<int>::const_iterator pm = max_element( v_ord.begin(), v_ord.end() );
  int m = 0;
  if( pm != v_ord.end() )
    m = *pm;
  int m1 = ( (m+10) / 10 ) * 10;
  return m1;
}/*}}}1*/


int TModel::getLinkInfos( int elnu, LinkInfo *li )/*{{{1*/
{
  int i, l_elnu, vx, vy, flip;
  QString lnk;
  if( !li )
    return -1;
  for( i=0; i<8; i++ ) {  // reset all fields to default
    li[i].reset();
  };
  TMiso *to_ob = getMiso( elnu );
  if( !to_ob )
    return -1;
  
  for( i=0; i<4; ++i ) {  // inputs
    if( ! to_ob->getData( "links.inps" + QString::number(i), lnk ) )
      continue;
    l_elnu = oname2elnu( lnk );
    li[i].elnu = l_elnu;
    switch( l_elnu ) {
      case -1: continue;
      case -2:  li[i].ltype = LinkBad; continue;
      case -12: li[i].ltype = LinkSpec; continue;
    }
    if( l_elnu < 0 ) // should not happen
      continue;
    
    TMiso *ob = getMiso( l_elnu );
    if( !ob ) 
      continue;
    vx = vy = -1; flip = 0;
    ob->getData( "vis_x", &vx ); ob->getData( "vis_y", &vy );
    ob->getData( "links.flip", &flip ); 
    if( vx < 0 || vy < 0 ) 
      continue;
    li[i].ltype = LinkElm;
    li[i].x = vx; li[i].y = vy; li[i].eflip = flip;
  };

  for( i=0; i<4; i++ ) {  // parm inputs
    int pi = i+4;
    if( ! to_ob->getData( "links.pinps" + QString::number(i), lnk ) )
      continue;
    l_elnu = oname2elnu( lnk );
    li[pi].elnu = l_elnu;
    switch( l_elnu ) {
      case -1: continue;
      case -2:  li[pi].ltype = LinkBad; continue;
      case -12: li[pi].ltype = LinkSpec; continue;
    }
    if( l_elnu < 0 ) // should not happen
      continue;
    
    TMiso *ob = getMiso( l_elnu );
    if( !ob ) 
      continue;
    vx = vy = -1; flip = 0;
    ob->getData( "vis_x", &vx ); ob->getData( "vis_y", &vy );
    ob->getData( "links.flip", &flip ); 
    if( vx < 0 || vy < 0 ) 
      continue;
    li[pi].ltype = LinkElm;
    li[pi].x = vx; li[pi].y = vy; li[pi].eflip = flip;
  };
  return 0;

}/*}}}1*/

void TModel::fillCommon(void)/*{{{1*/ // TODO: del at all
{
}/*}}}1*/

TDataSet* TModel::create( TDataSet* apar ) // static/*{{{1*/
{
   return new TModel( apar );
}/*}}}1*/

int TModel::getClassId(void) const/*{{{1*/
{
   return CLASS_ID_TModel;
}/*}}}1*/


const TClassInfo* TModel::getClassInfo(void) const/*{{{1*/
{
  return &class_info;
}/*}}}1*/

const char *TModel::getHelp(void) const/*{{{1*/
{
  return helpstr;
}/*}}}1*/

const double* TModel::getDoublePtr( const QString &nm, ltype_t *lt, int lev ) const
{
  static const double fake_so = 0;
  if( nm[0] != '#' ) {
    return TDataSet::getDoublePtr( nm, lt, lev );
  }
  QString nn = nm;
  nn.remove( 0, 1 );
  int n = nn.toInt();
  if( n < 0 || n >= MODEL_NVAR ) {
    if( lt )
      *lt = LinkBad;
    return &fake_so;
  }
  if( lt )
    *lt = LinkSpec;
  return &( vars[n] );
}

int TModel::registered = reg();

int TModel::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of tmodel.cpp

