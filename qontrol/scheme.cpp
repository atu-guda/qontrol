/***************************************************************************
                          scheme.cpp
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2000-2019 by atu
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

#include <ctime>
#include <cmath>
// unistd for usleep: TODO: replace with threads
#include <unistd.h>
#include <algorithm>

#include <QStandardItem>

#include "miscfun.h"
#include "scheme.h"
#include "tmodel.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial);

CTOR(Scheme,LinkedObj)
{
  allowed_types = QSL("TMiso");
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES );
  needReadInputsRecurse = true;
}


QVariant Scheme::dataObj( int col, int role ) const
{
  if( role == Qt::BackgroundRole ) {
    if( col != 0 ) {
      return TDataSet::dataObj( col, role );
    }
    if( hasAllFlags( efTmp )  ) { // transient schemes
      return QBrush( QColor(128,128,128) ) ;
    }
    return TDataSet::dataObj( col, role );

  }
  // else if ( role == Qt::ToolTipRole ) {
  // }
  // else if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
  // }

  return TDataSet::dataObj( col, role );
}


// must be similar to fillComplModelForInputs
const double* Scheme::getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev) const
{
  static int clt;
  static const double fake_src = 0.123456; // ptr here for non- main_s schemes uplinks
  int *plt = lt ? lt : &clt;

  if( nm.startsWith( '^' ) ) { // direct to parent
    QString nnm = nm.mid( 1 );
    LinkedObj *pds = qobject_cast<LinkedObj*>( par );
    if( !pds ) {
      *plt = LinkBad; return nullptr;
    }
    if( pds->isChildOf( "ContScheme" ) ) {
      *plt = LinkSpec;
      return &fake_src;
    }
    // qWarning() << "^ detected in " << nm << " nnm= " << nnm << NWHE;
    const double* rv =  pds->getDoublePtr( nnm, lt, src_ob, lev );
    // qWarning() << "lt= " << *lt << "rv=" << ( (rv==nullptr) ? "nullptr" : "V") << " pds: " << pds->getFullName() << NWHE;
    return rv;
    // return pds->getDoublePtr( nnm, lt, src_ob, lev );
  }

  // own names: elements (no local vars?)
  const double *p =  getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }

  // get model - for simulation and model itself
  TModel *mod = getAncestorT<TModel>();
  if( !mod ) {
    qWarning() << "No model detected" << NWHE;
    return nullptr;
  }

  // try active simulation
  Simulation *csim = mod->getActiveSimulation();
  if( csim ) {
    p = csim->getDoublePtr( nm, lt, src_ob, lev );
    if( p ) {
      return p;
    }
  }

  // then try model
  p =  mod->getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }

  qWarning() << "fail to find target " << nm << NWHE;

  return nullptr;
}

// must be in correspondence with getSchemeDoublePtr
void Scheme::fillComplModelForInputs( QStandardItemModel *mdl ) const
{
  if( ! mdl ) { return; }
  auto root_item = mdl->invisibleRootItem();

  fillComplForInputs( root_item );

  // if( !par ) { return; }
  // if( par->isObject( "TSubScheme" ) ) {  // ^ - TSubScheme -- no way to get anchor element
  //    par->fillComplForInputs( root_item, "^" );
  // }


  // get model - for simulation and model itself
  TModel *mod = getAncestorT<TModel>();
  if( !mod ) { return;  }

  // try active simulation
  Simulation *csim = mod->getActiveSimulation();
  if( csim ) {
    csim->fillComplForInputs( root_item );
  }

  // then model
  mod->fillComplForInputs( root_item );
}



int Scheme::runOneLoop( IterType itype )
{
  // readInputs(); // <-- too slow here
  for( auto cur_el : v_el ) {
     cur_el->readInputs(); // optimization
  }

  for( auto cur_el : v_el ) {
    if( end_loop ) {
      break;
    }

    cur_el->fun( itype );  // <============ main action
  };

  return ( end_loop == 0 );
}



void Scheme::do_reset()
{
  // linkNames();
  state = stateGood; run_type = -1; obj_brk = nullptr;
}




int Scheme::do_preRun()
{
  end_loop = 0;
  obj_brk = nullptr;
  breakObjNm = QES;
  return 1;
}

int Scheme::fback( int code, TMiso *obj )
{
  obj_brk = obj;
  breakObjNm = QSL("unknown?");
  if( obj ) {
    breakObjNm = obj->getFullName();
  }

  if( code ) {
    qWarning() << "Break with code" << code << "from obj" << breakObjNm << NWHE;
    end_loop = code;
  };
  return 0;
}



TMiso* Scheme::xy2Miso( int avis_x, int avis_y ) const
{
  for( auto ob : TCHILD(TMiso*) ) {
    int ox = ob->getDataD( QSL("eprm.vis_x"), -1 );
    int oy = ob->getDataD( QSL("eprm.vis_y"), -1 );
    if( ox == avis_x && oy == avis_y ) {
      return ob;
    }
  }
  return nullptr;
}

QSize Scheme::getMaxXY() const
{
  int mx = 0, my = 0;
  for( auto ob : TCHILD(TMiso*)  ) {
    int ox = ob->getDataD( QSL("eprm.vis_x"), 0 );
    int oy = ob->getDataD( QSL("eprm.vis_y"), 0 );
    if( ox > mx ) {
      mx = ox;
    }
    if( oy > my ) {
      my = oy;
    }
  }
  return QSize( mx, my );
}


bool Scheme::addElem( const QString &cl_name, const QString &ob_name, int avis_x, int avis_y )
{
  // not addObjT, downcast
  TMiso *ob = qobject_cast<TMiso*>( addObjP( cl_name, ob_name ) );
  if( !ob ) {
    return false;
  }
  ob->setData( QSL("eprm.vis_x"), avis_x );
  ob->setData( QSL("eprm.vis_y"), avis_y );
  reset();
  return true;
}

int Scheme::delElem( const QString &ename )
{
  int rc = delObj( ename );
  if( rc ) {
    reset();
  }
  return rc;
}


int Scheme::moveElem( const QString &nm, int newx, int newy )
{
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 ) {
    return -1;
  }
  ob = getObjT<TMiso*>( nm );
  if( !ob ) {
    return -1;
  }

  ob1 = xy2Miso( newx, newy );
  if( ob1 ) {
    return -1; // busy
  }

  ob->setData( QSL("eprm.vis_x"), newx );
  ob->setData( QSL("eprm.vis_y"), newy );
  reset();
  return 0;
}


void Scheme::do_structChanged()
{
  LinkedObj::do_structChanged();
  v_el.clear();

  for( auto ob : TCHILD(TMiso*) ) {
    int ign = 0;
    ob->getData( QSL("eprm.ignored"), &ign, false );
    if( ign ) {
      continue;
    }
    v_el.push_back( ob );
  };

}


int Scheme::post_startLoop()
{
  // LinkedObj::post_startLoop(); // = return 1
  if( !rinf || !rinf->sim ) {
    return 0;
  }

  int n_pre = rinf->sim->getDataD( QSL("n_pre"), 1 );
  for( int i=0; i<n_pre; ++i ) {
    for( auto el : v_el ) {
      el->preCalc();
    }
  }
  return 1;
}



DEFAULT_FUNCS_REG(Scheme)


// ------------------ container of Schemes --------------


const char* ContScheme::helpstr = "<H1>ContScheme</H1>\n"
 "Container of simulations";

STD_CLASSINFO(ContScheme,clpSpecial);

CTOR(ContScheme,LinkedObj)
{
  allowed_types = "Scheme,+SPECIAL";
  needReadInputsRecurse = true;
}

DEFAULT_FUNCS_REG(ContScheme)



// end of scheme.cpp

