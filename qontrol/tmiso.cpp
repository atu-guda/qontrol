/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2015 by atu
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

#include <QStandardItem>

#include "tmiso.h"

using namespace  std;



// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

STD_CLASSINFO(TMiso,clpSpecial|clpPure);


CTOR(TMiso,LinkedObj) ,
       pis( new InputParams( "pis", this, 0, "param links",
                             "object paramitric links", "sep=blockend") )
{
}

TMiso::~TMiso()
{
}


DEFAULT_FUNCS_REG(TMiso);

double TMiso::fun( double t, IterType itype )
{
  int v;
  iter_c = itype;
  if( locked ) {
    return out0 = (double)out0_init;
  }

  if( onlyFirst && itype != IterFirst ) {
    return out0;
  }

  if( onlyLast && itype != IterLast ) {
    return out0;
  }

  prm_mod |= pis->apply();

  v = out0 = f( t );
  return v;
}


double TMiso::f( double /* t */ )
{
  return 0;
}

int TMiso::preRun( int run_tp, int an, int anx, int any, double adt )
{
  tdt = adt; model_nn = an;
  iter_c = IterNo;
  pis->prepare();
  prm_mod = 0;
  int rc =  do_preRun( run_tp, an, anx, any, adt );
  if( !rc ) {
    return 0;
  }
  return (state > 0 ) ? 1 : 0;
}

int TMiso::do_preRun( int /*run_tp*/, int /*an*/, int /*anx*/,
                      int /*any*/, double /*adt*/ )
{
  return 1;
}



int TMiso::postRun( int good )
{
  int rc = do_postRun( good );
  state = good ? stateDone : stateGood;
  iter_c = IterNo;
  if( prm_mod ) {
    modified |= modifAuto;
  }
  return rc;
}

int TMiso::do_postRun( int /*good*/ )
{
  return 1;
}

int TMiso::startLoop( int acnx, int acny )
{
  state = stateRun;
  out0 = (double)out0_init;
  prm_mod |= pis->apply_pre();
  return do_startLoop( acnx, acny );
}

int TMiso::do_startLoop( int /* acnx */, int /* acny */ )
{
  return 1;
}

int TMiso::endLoop()
{
  state = stateGood;
  return do_endLoop();
}

int TMiso::do_endLoop()
{
  return 1;
}

void TMiso::fillComplModelForParams( QStandardItemModel *mdl ) const
{
  // TODO: scheme
  for( auto hd: TCHILD(HolderDouble*) ) {
    auto it = new QStandardItem;
    it->setText( hd->objectName() );
    mdl->appendRow( it );
  }
}

