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



// ------------------------ TMiso -------------------------------

const char* TMiso::helpstr = "<H1>TMiso</H1>\n"
 "Base class for all active elements. Never should be used.";

STD_CLASSINFO(TMiso,clpSpecial|clpPure);


CTOR(TMiso,LinkedObj)
{
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



int TMiso::do_startLoop( int acnx, int acny )
{
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

int TMiso::miso_startLoop( int /* acnx */, int /* acny */ )
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

