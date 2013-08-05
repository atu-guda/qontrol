/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
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

#include <limits>
#include "tmiso.h"
#include "tmodel.h"

using namespace  std;

const char* TElmLink::helpstr = "<H1>TElmLink</H1> \n"
 "Defines signal and parametric inputs for given element. \n"
 "Each element may have up to 4 both signal and parametric inputs\n";


TClassInfo TElmLink::class_info = {
 0, "TElmLink", TElmLink::create, // TODO: ??? remove id
 &TDataSet::class_info, helpstr, clpSpecial };


TElmLink::TElmLink( TDataSet* apar )
         :TDataSet( apar )
{
  noauto = locked = onlyFirst = onlyLast = flip = noIcon = 0;
  pflags0 = pflags1 = pflags2 = pflags3 = 0;
}

TDataSet* TElmLink::create( TDataSet* apar )
{
  return new TElmLink( apar );
}

const TClassInfo* TElmLink::getClassInfo(void) const
{
  return &class_info;
}

const char *TElmLink::getHelp(void) const
{
  return helpstr;
}

// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

TClassInfo TMiso::class_info = {
 0, "TMiso", TMiso::create, // TODO: remove id
 &TDataSet::class_info, helpstr, clpSpecial };


TMiso::TMiso( TDataSet* aparent )
      :TDataSet( aparent ),
       out0_init(0), out0(0),
       links( new TElmLink( this ) )
{
  ord = -1; 
  vis_x = vis_y = 0; tdt = 0; model_nn = 0; 
  model = 0;
}

TMiso::~TMiso()
{
  // delete links; --- it's part of TMiso, deleted by ~TDataSet
}

TDataSet* TMiso::create( TDataSet* /* apar */ )
{
  return 0; // cannot create abstract object
}

double TMiso::fun( double t, IterType itype )
{
  int v;
  if( links->locked ) 
    return out0 = *in_so[0];
  if( links->onlyFirst && itype != IterFirst )
    return out0;
  if( links->onlyLast && itype != IterLast )
    return out0;
  
  modifyPrms();

  v = out0 = f( t );
  prm_mod = 0;
  return v;
}

const TClassInfo* TMiso::getClassInfo(void) const
{
  return &class_info;
}

const char *TMiso::getHelp(void) const
{
  return helpstr;
}

double TMiso::f( double /* t */ )
{
  return 0;
}

int TMiso::preRun( int run_tp, int an, int anx, int any, double adt )
{
  tdt = adt; model_nn = an; 
  model = static_cast<TModel*>(par); 
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
  
  // ordinary input links
  for( int i=0; i<4; ++i ) {
    in_so[i] = &fake_so;
    iname = "inps" + QString::number(i);
    links->getData( iname, soname );
    p = par->getDoublePtr( soname );
    if( p ) {
      in_so[i] = p;
    }
  }

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
  out0 = out0_init;
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

