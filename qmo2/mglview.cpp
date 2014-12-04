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
  Reload();
}

MglDrawer::~MglDrawer()
{
  DBGx( "dbg: dtor gra=%p", gra );
  resetData();
  gra = nullptr; // not delete, we are not owner, just for debug
}

void MglDrawer::resetData()
{
  delete dx; dx = nullptr;
  qDeleteAll( d.begin(), d.end() );
  d.clear();

  label_x = label_y = QString();
  labels.clear();
  extras.clear();
  w0 = 100; h0 = 50;
}

int MglDrawer::Draw( mglGraph *gr )
{
  if( !gr  || !gra ) {
    return 0;
  }

  int fsz = 1;
  gra->getData( "fontSise", &fsz );
  gr->SetFontSize( fsz );

  double plotFactor = 1.5;
  gra->getData( "plotFactor", &plotFactor );
  gr->SetPlotFactor( plotFactor );

  double phi = 0, theta = 0;
  gra->getData( "phi", &phi );
  gra->getData( "theta", &theta );
  gr->Rotate( phi, theta );

  int useLight = 0;
  gra->getData( "useLight", &useLight );
  gr->Light( useLight );

  int useAlpha = 0;
  gra->getData( "useAlpha", &useAlpha );
  if( useAlpha ) {
    double alpha = 0.5;
    gra->getData( "alpha", &alpha );
    gr->Alpha( true );
    gr->SetAlphaDef( alpha );
  } else{
    gr->Alpha( false );
  }

  int i_cb;
  gra->getData( "bgcolor", &i_cb );
  QColor cv = QRgb( i_cb );
  // QString s_cb; // TODO: remove after debug: need not here
  // s_cb.sprintf( "{x%02X%02X%02X}", cv.red(),  cv.green(), cv.blue() );
  //DBGx( "dbg: bgcolor ok=%d %lf %lf %lf \"%s\" %6X.",
  //    (int)(ok), cv.redF(), cv.greenF(), cv.blueF(), qP(s_cb), i_cb )
  gr->Clf( cv.redF(), cv.greenF(), cv.blueF() );

  int start_idx = 0; // most of 1D plots
  int end_idx = d.size();

  if( type >= TGraph::PlotType::PlotSurf ) { // 2D plot
    // gr->SetRanges( x_min, x_max, y_min, y_max, y_min, y_max ); // TODO: fix for Z~
    if( d.size() < 2 ) {
      return 0;
    }
    start_idx = 1;
    gr->Axis( "xyzU3AKDTVISO" );
    gr->Grid( "xyz", "{h7}", "" );
  } else {
    if( d.size() < 1 ) {
      return 0;
    }
    gr->SetRanges( x_min, x_max, y_min, y_max );
    gr->Axis( "xyU3AKDTVISO" );
    gr->Grid( "xy", "{h7}", "" );
  }

  for( int ng = start_idx; ng < end_idx; ++ng ) {
    auto di = d[ng];
    auto lbl = labels[ng];
    auto extra = qP( extras[ng] );
    switch( type ) {
      case TGraph::PlotType::PlotPlot:
        gr->Plot( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotRadar:
        gr->Radar( *di, extra );
        break;
      case TGraph::PlotType::PlotStep:
        gr->Step( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotArea:
        gr->Area( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotStem:
        gr->Stem( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotBars:
        gr->Bars( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotBarh:
        gr->Barh( *dx, *di, extra );
        break;
      case TGraph::PlotType::PlotChart:
        gr->Chart( *di, extra );
        break;
      case TGraph::PlotType::PlotSurf:
        // gr->Surf( *dx, *dy, *di );
        break;
      default:
        break;
    }
  }


  gr->Box();
  return 1;
}

void MglDrawer::Reload( )
{
  DBG1( "dbg: Reload()!" );
  resetData();

  if( !gra ) {
    DBG1( "err: no TGraph passed to MglDrawer" );
    return;
  }

  gra->getData( "w0", &w0 );
  gra->getData( "h0", &h0 );

  gra->getData( "type", &type );

  model = gra->getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(gra->getFullName()) );
    return;
  }

  // get X axis element
  GraphElem *ge = gra->getElemT<GraphElem*>( "x" );
  if( !ge ) {
    DBGx( "warn: not found 'x' element in \"%s\"", qP(gra->getFullName()) );
    return;
  }
  QString src;
  ge->getData( "src", src );
  if( src.isEmpty() ) {
    DBGx( "warn: empty source for 'x' element in \"%s\"", qP(gra->getFullName()) );
    return;
  }

  TOutArr *arr = model->getOutArr( src );
  if( !arr ) {
    DBGx( "warn: bad source \"%s\" for 'x' element in \"%s\"", qP(src), qP(gra->getFullName()) );
    return;
  }


  // get all other elements
  int nx = 0;
  arr->getData( "n", &nx );
  int ny = 1;
  arr->getData( "ny", &ny );
  arr->getData( "dmin", &x_min );
  arr->getData( "dmax", &x_max );
  const dvector *ve = arr->getArray();
  const double *ver = ve->data();
  arr->getData( "label", label_x );

  mglData *md = new mglData( nx, ny );
  dx = md;
  md->Link( const_cast<double*>(ver), nx, ny );

  QString label_c, extra_c;
  int ng = 0;

  for( auto c : gra->children() ) {
    ge = qobject_cast<GraphElem*>( c );
    if( ! ge ) {
      continue;
    }
    if( ge->objectName() == "x" ) {
      continue;
    }

    src = QString();
    ge->getData( "src", src );
    if( src.isEmpty() ) {
      continue;
    }
    arr = model->getOutArr( src );
    if( !arr ) {
      continue;
    }
    arr->getData( "n", &nx );
    arr->getData( "ny", &ny );
    double tmin = 0, tmax = 0;
    arr->getData( "dmin", &tmin );
    arr->getData( "dmax", &tmax );
    if( tmin < y_min ) {
      y_min = tmin;
    }
    if( tmax > y_max ) {
      y_max = tmax;
    }
    ve = arr->getArray();
    ver = ve->data();

    label_c = QString( "y_%1" ).arg( ng );
    arr->getData( "label", label_c );
    if( labels.isEmpty() ) {
      label_y = label_c;
    }
    labels.push_back( label_c );

    int lw = 1;
    ge->getData( "lw", &lw );

    int i_cc = 0;
    ge->getData( "color", &i_cc );
    QColor cc = QRgb( i_cc );
    QString s_cc;
    s_cc.sprintf( "%d{x%02X%02X%02X}", lw, cc.red(),  cc.green(), cc.blue() );

    QString extra_add = QString( "" );
    ge->getData( "extra", extra_add );
    extra_c = s_cc + extra_add;
    extras.push_back( extra_c );


    mglData *md = new mglData( nx, ny );
    md->Link( const_cast<double*>(ver), nx, ny );
    d.push_back( md );
    DBGx( "dbg: added array \"%s\" nx= %d, ny=%d ng = %d label: \"%s\" extra: \"%s\"",
        qP(arr->getFullName()), nx, ny, ng, qP(label_c), qP(extra_c) );
    ++ng;

  }
}

// ------------------------- MglView ------------------------

MglView::MglView( TGraph *agra, QWidget *parent )
          : QWidget( parent ),
          lay( new QVBoxLayout( this ) ),
          drawer( new MglDrawer( agra ) ),
          mgl( new QMathGL( this ) ),
          lbl( new QLabel( "lbl", this) )
{
  lbl->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  lay->addWidget( mgl );
  lay->addWidget( lbl );

  QSize sz0 = drawer->getSize0();
  mgl->resize( sz0 );
  resize( sz0 + lbl->sizeHint() );

  mgl->setDraw( drawer );
  mgl->update();
}

MglView::~MglView()
{
  delete mgl; mgl = nullptr;
  // delete drawer; drawer = nullptr; // deleted by mgl dtor !!!
}

QSize MglView::sizeHint() const
{
  return drawer->getSize0() + QSize( 10, 10 );
}


