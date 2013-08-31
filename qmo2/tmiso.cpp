/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
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

#include "tmiso.h"
#include "tmodel.h"

using namespace  std;

const char* TElmLink::helpstr = "<H1>TElmLink</H1> \n"
 "Defines signal and parametric inputs for given element. \n"
 "Each element may have up to 4 both signal and parametric inputs\n";

STD_CLASSINFO( TElmLink, clpSpecial );

CTOR(TElmLink,TDataSet)
{
}

DEFAULT_FUNCS_REG(TElmLink);


// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

STD_CLASSINFO(TMiso,clpSpecial|clpPure);


CTOR(TMiso,TDataSet) ,
       links( new TElmLink( "links", this, 0, "links", "object links", "sep=blockend" ) )
{
}

TMiso::~TMiso()
{
}

void TMiso::post_set()
{
  TDataSet::post_set();
  // no links -- means only new data
  if( !links )
    return;
  int t = 0;
  if( ! ( locked || flip || onlyFirst || onlyLast || noIcon ) ) {
    // try to migrate from old data
    t = 0; links->getData( "locked", &t ); locked = t;
    t = 0; links->getData( "flip", &t ); flip = t;
    t = 0; links->getData( "onlyFirst", &t ); onlyFirst = t;
    t = 0; links->getData( "onlyLast", &t ); onlyLast = t;
    t = 0; links->getData( "noIcon", &t ); noIcon = t;
  }

  // ordirary links convert
  int nl = inputs.size();
  if( nl > 4 ) 
    nl = 4;
  QString source, iname;
  for( int i=0; i<nl; ++i ) {
    InputSimple *in = inputs[i];
    if( !in )
      continue;
    if( ! in->getData( "source", source )  ) 
      continue;
    if( ! source.isEmpty() )
      continue;
    iname = "inps" + QSN(i);
    if( ! links->getData( iname, source )  ) 
      continue;
    in->setData( "source", source );
  }
}

DEFAULT_FUNCS_REG(TMiso);

double TMiso::fun( double t, IterType itype )
{
  int v;
  if( locked ) 
    return out0 = (double)out0_init;
  if( onlyFirst && itype != IterFirst )
    return out0;
  if( onlyLast && itype != IterLast )
    return out0;
  
  modifyPrms();

  v = out0 = f( t );
  prm_mod = 0;
  return v;
}


double TMiso::f( double /* t */ )
{
  return 0;
}

int TMiso::preRun( int run_tp, int an, int anx, int any, double adt )
{
  tdt = adt; model_nn = an; 
  model = qobject_cast<TModel*>(par); 
  fillLinks();
  int rc =  do_preRun( run_tp, an, anx, any, adt );
  if( rc != 0 )
    return rc;
  return (state > 0 ) ? 0 : 1;
}

int TMiso::do_preRun( int /*run_tp*/, int /*an*/, int /*anx*/,
                      int /*any*/, double /*adt*/ )
{
  return 0;
}

int TMiso::fillLinks()
{
  const double *p;
  double *pp;
  QString iname, pname, soname, sopname;
  
  // ordinary input links: obsoleted by InputSimple

  // parametric input links
  max_prm = 0;
  int flg, onlyFirstPrm;
  for( int i=0; i<4; ++i ) {
    inp_so[i] = &fake_so;
    inp_prm[i] = &fake_prm;
    prm_flg[i] = -1;
    onlyFirstPrm = 0;
    iname = "pinps" + QString::number(i);
    pname = "pnames" + QString::number(i);
    links->getData( iname, soname );
    links->getData( pname, sopname );
    p = par->getDoublePtr( soname );
    pp = getDoublePrmPtr( sopname, &flg );
    links->getData( "pflags" + QString::number(i), &onlyFirstPrm );
    if( flg & efNoRunChange ) // force only-first if param cannot be changed in RT
      onlyFirstPrm = 1;
    if( onlyFirstPrm ) 
      flg |= efOnlyFirst; 

    if( p && pp && ! ( flg & efRO )) {
      inp_so[i] = p;
      inp_prm[i] = pp;
      prm_flg[i] = flg;
      max_prm = i+1;
    }
  }
  return 0;
}

int TMiso::modifyPrmsPre()
{
  for( int i=0; i<max_prm; ++i ) {
    if( prm_flg[i] == -1  || ! ( prm_flg[i] & efOnlyFirst )  ) { 
      continue; 
    }
    *inp_prm[i] = *inp_so[i];
    ++prm_mod;
  }
  return prm_mod;
}

int TMiso::modifyPrms()
{
  for( int i=0; i<max_prm; ++i ) {
    if( prm_flg[i] == -1 || ( prm_flg[i] & efOnlyFirst ) ) {
      continue; 
    }
    *inp_prm[i] = *inp_so[i];
    ++prm_mod;
  }
  return prm_mod;
}

int TMiso::postRun( int good )
{
  do_postRun( good );
  state = good ? stateDone : stateGood;
  model = 0;
  return 0;
}

int TMiso::do_postRun( int /*good*/ )
{
  return 0;
}

int TMiso::startLoop( int acnx, int acny )
{
  state = stateRun;
  out0 = (double)out0_init;
  modifyPrmsPre();
  prm_mod = 0;
  return do_startLoop( acnx, acny );
}

int TMiso::do_startLoop( int /* acnx */, int /* acny */ )
{
  return 0;
}

int TMiso::endLoop()
{
  state = stateGood;
  return do_endLoop();
}

int TMiso::do_endLoop()
{
  return 0;
}

