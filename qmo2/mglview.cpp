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

QString color2style( int color, int lw, const QString &extra )
{
  QColor cc = QRgb( color );
  QString s_cc;
  s_cc.sprintf( "%d{x%02X%02X%02X}", lw, cc.red(),  cc.green(), cc.blue() );
  s_cc += extra;
  return s_cc;
}

// ---------------------------------------------------------------------

using namespace std;
MglDrawer::MglDrawer( TGraph *agra )
  : gra( agra )
{
  Reload();
}

MglDrawer::~MglDrawer()
{
  // DBGx( "dbg: dtor gra=%p", gra );
  resetData();
  gra = nullptr; // not delete, we are not owner, just for debug
  delete scd_del; scd_del = nullptr;
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

  gr->SetFontSize( fsz );
  gr->SetPlotFactor( plotFactor );
  gr->Rotate( phi, theta );
  gr->Light( useLight );
  gr->SetAlphaDef( alpha );
  gr->Alpha( (bool)(useAlpha) );

  gr->SetRanges( x_min, x_max, y_min, y_max );

  if( ! scd->autoScX ) {
    gr->SetRange( 'x', scd->plotMinX, scd->plotMaxX );
  }
  if( ! scd->autoScY ) {
    gr->SetRange( 'y', scd->plotMinY, scd->plotMaxY );
  }
  gr->SetTicks( 'x', -(scd->gridX), scd->tickX );
  gr->SetTicks( 'y', -(scd->gridY), scd->tickY );

  QByteArray axis_style = color2style( axis_color.rgb(), 1 ).toLocal8Bit();
  QByteArray grid_style = color2style( grid_color.rgb(), 1, "=" ).toLocal8Bit();

  gr->Clf( bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF() );

  int start_idx = 0; // most of 1D plots
  int end_idx = d.size();

  if( type >= TGraph::PlotType::PlotSurf ) { // 2D plot
    // gr->SetRanges( x_min, x_max, y_min, y_max, y_min, y_max ); // TODO: fix for Z~
    if( d.size() < 2 ) {
      return 0;
    }
    start_idx = 1;
    gr->Axis( "xyzU3AKDTVISO",  axis_style );
    gr->Grid( "xyz", grid_style );
  } else {
    if( d.size() < 1 ) {
      return 0;
    }
    gr->Axis( "xyzU3AKDTVISO", axis_style );
    gr->Grid( "xy", grid_style );
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


  gr->Box( axis_style );
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
  scd =  gra->getElemT<ScaleData*>( "scd" );
  if( !scd ) {
    DBG1( "ScaleData not found, recreatind" );
    scd = new ScaleData( "scd", gra, 0, "scale", "default scale data" );
    scd_del = scd; // to delete on exit
  }


  gra->getData( "w0", &w0 );
  gra->getData( "h0", &h0 );

  gra->getData( "fontSise", &fsz );
  gra->getData( "plotFactor", &plotFactor );
  gra->getData( "phi", &phi );
  gra->getData( "theta", &theta );
  gra->getData( "useLight", &useLight );
  gra->getData( "useAlpha", &useAlpha );
  gra->getData( "alpha", &alpha );

  int col;
  gra->getData( "bgcolor", &col );
  bgcolor = QColor( col );
  gra->getData( "axis_color", &col );
  axis_color = QColor( col );
  gra->getData( "grid_color", &col );
  grid_color = QColor( col );

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


  int nn = 0;
  arr->getData( "n", &nn );
  int ny = 1;
  arr->getData( "ny", &ny );
  int nx = nn / ny;
  arr->getData( "dmin", &x_min );
  arr->getData( "dmax", &x_max );
  const dvector *ve = arr->getArray();
  arr->getData( "label", label_x );


  QString label_c, extra_c;
  int ng = 0;

  // get all other elements
  vector< const dvector* > ves;
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

    int nn_c = 0;
    arr->getData( "n", &nn_c );
    int ny_c = 1;
    arr->getData( "ny", &ny_c );
    int nx_c = nn_c / ny_c;
    if( nn_c != nn  ||  ny_c != ny  ||  nx_c != nx ) { // only same dimensions
      continue;
    }
    double tmin = 0, tmax = 0;
    arr->getData( "dmin", &tmin );
    arr->getData( "dmax", &tmax );
    if( tmin < y_min ) {
      y_min = tmin;
    }
    if( tmax > y_max ) {
      y_max = tmax;
    }
    ves.push_back( arr->getArray() );

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

    QString extra_add = QString( "" );
    ge->getData( "extra", extra_add );
    extra_c = color2style( i_cc, lw, extra_add );
    extras.push_back( extra_c );


    DBGx( "dbg: added array \"%s\" nx= %d, ny=%d ng = %d label: \"%s\" extra: \"%s\"",
        qP(arr->getFullName()), nx, ny, ng, qP(label_c), qP(extra_c) );
    ++ng;

  }

  const int max_nn_nosqz = 2000;
  int np = nn; // number of selected points
  vector<uint8_t> plp( nn, 0 ); // flags: point worth to draw

  if( nn > max_nn_nosqz ) {

    const double qual = 1.3; // TODO: param
    const int nd0 = 4;
    double mdlt_y = qual * ( y_max - y_min ) / h0; // TODO: current height?
    int stp0 = nn * nd0 / h0;
    np = 0;


    // base grid
    for( int i=0; i<nn; i+=stp0 ) {
      plp[i] = 1;
      ++np;
    }
    if( ! plp[nn-1] ) {
      plp[nn-1] = 1;
      ++np;
    }
    // DBGx( " dbg: np start = %d  mdlt_y= %f" , np, mdlt_y );

    int was_add = 1;
    for( int n_add = 0; was_add && n_add < 10; ++n_add ) { // 10 : max iterations to split
      was_add = 0;
      for( auto yyc : ves ) {
        for( int i0 = 0, i1 = 0; i0 < nn-1; i0 = i1 ) {
          for( i1 = i0+1; !plp[i1] && i1 < nn; ++i1 ) /* NOP: find next set*/;
          double x0 = (*ve)[i0], x1 = (*ve)[i1];
          double y0 = (*yyc)[i0], y1 = (*yyc)[i1];
          if( y0 == y1 ) {
            continue;
          }
          double kxy = ( y1 - y0 ) / ( x1 - x0 );

          // find point of maximum delta
          int i_cm = 0; // index of current maximum
          double dly_cm = 0; // value of this max
          for( int i2=i0+1; i2<i1-1; ++i2 ) {
            double yc = y0 + kxy * ( (*ve)[i2] - x0 );
            double dly = fabs( yc - (*yyc)[i2] );
            if( dly > dly_cm ) {
              dly_cm = dly; i_cm = i2;
            }
          }
          if( dly_cm > mdlt_y  && ! plp[i_cm]  ) { // may be set by other
            plp[i_cm] = 1; ++np; ++was_add;
          }
        }
      }
      // DBGx( " dbg: np mid = %d  n_add= %d was_add= %d" , np, n_add, was_add );
    }
  } else {
    plp.assign( nn, 1 ); // all point are good
  }
  // DBGx( "dbg: nn: %d np: %d nx: %d ny: %d ng: %d", nn, np, nx, ny, ng );

  // create mglData for X and copy data
  mglData *md = new mglData( np, ny );
  dx = md;
  for( int ig=0; ig<ng; ++ig ) {
    d.push_back( new mglData( np, ny ) );
  }

  for( int i=0, j=0; i<nn && j<np ; ++i ) {
    if( plp[i] ) {
      dx->a[j] = (*ve)[i];
      for( int ig=0; ig<ng; ++ig ) {
        d[ig]->a[j] = (*ves[ig])[i];
      }
      ++j;
    }
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


