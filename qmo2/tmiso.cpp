/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2014 by atu
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

using namespace  std;



// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

STD_CLASSINFO(TMiso,clpSpecial|clpPure);


CTOR(TMiso,TDataSet) ,
       pis( new InputParams( "pis", this, 0, "param links", "object paramitric links", "sep=blockend") )
{
}

TMiso::~TMiso()
{
}


DEFAULT_FUNCS_REG(TMiso);

double TMiso::fun( double t, IterType itype )
{
  int v;
  if( locked ) {
    return out0 = (double)out0_init;
  }

  if( onlyFirst && itype != IterFirst ) {
    return out0;
  }

  if( onlyLast && itype != IterLast ) {
    return out0;
  }

  prm_mod = pis->apply();

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
  pis->prepare();
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
  pis->apply_pre();
  prm_mod = 0;
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

