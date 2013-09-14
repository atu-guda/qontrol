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
 "<p><b>Obsoleted.</b>Defines signal and parametric inputs for given element. \n"
 "Each element may have up to 4 both signal and parametric inputs</p>\n";

STD_CLASSINFO( TElmLink, clpSpecial );

CTOR(TElmLink,TDataSet)
{
  flags |= efNoSave | efNoDial;
}

DEFAULT_FUNCS_REG(TElmLink);


// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

STD_CLASSINFO(TMiso,clpSpecial|clpPure);


CTOR(TMiso,TDataSet) ,
       links( new TElmLink( "links", this, 0, "links", "object links", "" ) ),
       pis( new InputParams( "pis", this, 0, "param links", "object paramitric links", "sep=blockend") )
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

  // param links convert
  if( ! pis || pis->getNumObj() != 0 )
    return;

  QString prm_name, flg_name;
  int flg;
  for( int i=0; i<4; ++i ) {
    iname = "pinps" + QSN(i);
    if( ! links->getData( iname, source )  )
      continue;
    if( source.isEmpty() )
      continue;
    iname = "pnames" + QSN(i);
    links->getData( iname, prm_name ); // ignore error
    iname = "pflags" + QSN(i);
    links->getData( iname, &flg ); // ignore error
    iname = "p" + QSN(i);
    pis->add_obj( "InputParam", iname );
    InputParam *pa = pis->getElemT<InputParam*>(iname);
    if( !pa ) {
      DBGx( "warn: fail to convert param input in \"%s\" N %d src \"%s\"",
          qP(objectName()), i, qP(source) );
      continue;
    }
    pa->setData( "source", source );
    pa->setData( "tparam", prm_name );
    pa->setData( "onlyFirst", flg );
    pa->setData( "line_w", 2 );
    pa->setData( "line_color", "red" );
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
  model = qobject_cast<TModel*>(par);
  pis->prepare();
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
  pis->apply_pre();
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

