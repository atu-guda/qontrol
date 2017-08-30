/***************************************************************************
                          tmiso.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2016 by atu
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

// ------------------------ ElemParams --------------------------

const char* ElemParams::helpstr = "<h1>ElemParams</h1>\n"
 "<p>Structure to hold misc auxilary element parameters.</p>\n";

STD_CLASSINFO(ElemParams,clpSpecial);

CTOR(ElemParams,TDataSet)
{
}

DEFAULT_FUNCS_REG(ElemParams);

int ElemParams::do_postFileRead()
{
  if( !par || cvtCount > 0 ) { // only on conversion
    return 0;
  }

  vis_x       = par->getDataD( QSL("vis_x"), 1 );
  vis_y       = par->getDataD( QSL("vis_y"), 1 );
  noCalcStart = par->getDataD( QSL("noCalcStart"), 0 );
  locked      = par->getDataD( QSL("locked"), 0 );
  ignored     = par->getDataD( QSL("ignored"), 0 );
  onlyFirst   = par->getDataD( QSL("onlyFirst"), 0 );
  onlyLast    = par->getDataD( QSL("onlyLast"), 0 );
  flip        = par->getDataD( QSL("flip"), 0 );
  noIcon      = par->getDataD( QSL("noIcon"), 0 );
  showBaloon  = par->getDataD( QSL("showBaloon"), 0 );
  return 1;
}

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
  if( locked || ignored ) {
    return out0 = (double)out0_init;
  }

  if( onlyFirst && itype != IterFirst ) {
    return out0;
  }

  if( onlyLast && itype != IterLast ) {
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
  if( ignored ) {
    return 1;
  }
  state = stateRun;
  out0 = (double)out0_init;
  // readAllInputs(); // moved to LinkedObj
  return miso_startLoop( acnx, acny );
}

void TMiso::preCalc()
{
  if( hasClassProps( clpCalcAtStart ) && ! noCalcStart  ) {
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

