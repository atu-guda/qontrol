/***************************************************************************
                          tmiso.h  - base for scheme elements
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2017 by atu
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
 "Base class for all active elements. Never should be used directly.";

STD_CLASSINFO(TMiso,clpSpecial|clpAbstract);


CTOR(TMiso,LinkedObj)
{
  allowed_types = "ElemParams";
  eprm = addObjT<ElemParams>( "eprm" );
  eprm->setParm( "sep", "blockend" );
}


DEFAULT_FUNCS_REG(TMiso);

QString TMiso::textVisual() const
{
  return QSL("=") % QSND( out0 );
}

double TMiso::fun( IterType itype ) noexcept
{
  iter_c = itype;
  if( ca_locked || ca_ignored ) {
    return out0 = (double)out0_init;
  }

  if( ca_onlyFirst && itype != IterFirst ) {
    return out0;
  }

  if( ca_onlyLast && itype != IterLast ) {
    return out0;
  }

  // readInputs(); // moved to scheme

  ct = *p_t_model;
  out0 = f();
  return out0;
}


double TMiso::f() noexcept
{
  return 0;
}

double TMiso::f_d( double /*arg0*/, double /*arg1*/, double /*arg2*/, double /*arg3*/ )
{
  return 0;
}


int TMiso::do_startLoop( long acnx, long acny )
{
  ca_noCalcStart = getDataD( QSL("eprm.noCalcStart"), 0 );
  ca_locked      = getDataD( QSL("eprm.locked"),      0 );
  ca_ignored     = getDataD( QSL("eprm.ignored"),     0 );
  ca_onlyFirst   = getDataD( QSL("eprm.onlyFirst"),   0 );
  ca_onlyLast    = getDataD( QSL("eprm.onlyLast"),    0 );
  if( ca_ignored ) {
    return 1;
  }
  state = stateRun;
  out0 = (double)out0_init;
  // readAllInputs(); // moved to LinkedObj
  return miso_startLoop( acnx, acny );
}

void TMiso::preCalc()
{
  if( !eprm ) {
    return;
  }
  if( hasClassProps( clpCalcAtStart ) && ! ca_noCalcStart  ) {
    readAllInputs();
    fun( IterFirst );
  }
}

int TMiso::miso_startLoop( long /* acnx */, long /* acny */ )
{
  return 1;
}


void TMiso::do_structChanged()
{
  LinkedObj::do_structChanged();
  prm_will_mod = 0;
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

