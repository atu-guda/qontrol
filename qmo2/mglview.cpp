/***************************************************************************
                          mglview.cpp - window to show plots via MathGL
                             -------------------
    begin                : 2014.11.29
    copyright            : (C) 2014-2014 by atu
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

#include <cmath>
#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>

#include <mgl2/qt.h>
#include <mgl2/qmathgl.h>

#include "toutarr.h"
#include "tgraph.h"
#include "tmodel.h"

#include "mglview.h"


using namespace std;
MglDrawer::MglDrawer( TGraph *agra )
  : gra( agra )
{
}

MglDrawer::~MglDrawer()
{
  DBGx( "dbg: dtor gra=%p", gra );
  gra = nullptr; // not delete, we are not owner, juts for debug
}

int MglDrawer::Draw( mglGraph *gr )
{
  gr->Rotate( 60, 40 );
  gr->Box();
  gr->Axis( "xyz" );
  return 1;
}

void MglDrawer::Reload( )
{
  DBG1( "dbg: Reload()!" );
}

// ------------------------- MglView ------------------------

MglView::MglView( TGraph *agra, QWidget *parent )
          : QWidget( parent ),
          drawer( new MglDrawer( agra ) ),
          // drawer( agra ),
          mgl( new QMathGL( this ) )
{
  // getData();
  mgl->setDraw( drawer );
  mgl->update();
}

MglView::~MglView()
{
  DBGx( "dbg: dtor xxx=%d", xxx );
  xxx = 0;
  delete mgl; mgl = nullptr;
  // delete drawer; drawer = nullptr; // deleted by mgl dtor !!!
  // delete dx; dx = nullptr;
  // while( ! d.isEmpty() ) {
  //   delete d.takeFirst();
  // }
}

QSize MglView::sizeHint() const
{
  // return QSize( w0+10, h0+10 );
  return QSize( 500, 400 );
}

// void MglView::getData()
// {
//   delete dx; dx = nullptr;
//   while( ! d.isEmpty() ) {
//     delete d.takeFirst();
//   }
//   model = nullptr;
//
//   if( !gra ) {
//     DBG1( "err: no TGraph passed to MglView" );
//     return;
//   }
//
//   gra->getData( "w0", &w0 );
//   gra->getData( "h0", &h0 );
//   mgl->resize( w0, h0 );
//   mgl->setSize( w0, h0 );
//   resize( w0+10, h0+10 );
//
//   gra->getData( "type", &type );
//
//   model = gra->getAncestorT<TModel>();
//   if( !model ) {
//     DBGx( "warn: not found model in \"%s\"", qP(gra->getFullName()) );
//     return;
//   }
//
//   GraphElem *ge = gra->getElemT<GraphElem*>( "x" );
//   if( !ge ) {
//     DBGx( "warn: not found 'x' element in \"%s\"", qP(gra->getFullName()) );
//     return;
//   }
//   QString src;
//   ge->getData( "src", src );
//   if( src.isEmpty() ) {
//     DBGx( "warn: empty source for 'x' element in \"%s\"", qP(gra->getFullName()) );
//     return;
//   }
//
//   TOutArr *arr = model->getOutArr( src );
//   if( !arr ) {
//     DBGx( "warn: bad source \"%s\" for 'x' element in \"%s\"", qP(src), qP(gra->getFullName()) );
//     return;
//   }
//
//
//   int nx = 0;
//   arr->getData( "n", &nx );
//   int ny = 1;
//   arr->getData( "ny", &ny );
//   arr->getData( "dmin", &x_min );
//   arr->getData( "dmax", &x_max );
//   const dvector *ve = arr->getArray();
//   const double *ver = ve->data();
//
//   mglData *md = new mglData( nx, ny );
//   dx = md;
//   md->Link( const_cast<double*>(ver), nx, ny );
//
//   for( auto c : gra->children() ) {
//     ge = qobject_cast<GraphElem*>( c );
//     if( ! ge ) {
//       continue;
//     }
//     src = QString();
//     ge->getData( "src", src );
//     if( src.isEmpty() ) {
//       continue;
//     }
//     arr = model->getOutArr( src );
//     if( !arr ) {
//       continue;
//     }
//     arr->getData( "n", &nx );
//     arr->getData( "ny", &ny );
//     double tmin = 0, tmax = 0;
//     arr->getData( "dmin", &tmin );
//     arr->getData( "dmax", &tmax );
//     if( tmin < y_min ) {
//       y_min = tmin;
//     }
//     if( tmax > y_max ) {
//       y_max = tmax;
//     }
//     ve = arr->getArray();
//     ver = ve->data();
//
//     mglData *md = new mglData( nx, ny );
//     md->Link( const_cast<double*>(ver), nx, ny );
//     d.push_back( md );
//     DBGx( "dbg: added array \"%s\" nx= %d, ny=%d", qP(arr->getFullName()), nx, ny );
//
//   }
//
//
// }

