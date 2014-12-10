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
  d_x = d_y = d_z = d_c0 = d_c1 = d_c2 = d_c3 = d_c4 = d_c5 = nullptr;
  x_min = x_max = y_min = y_max = z_min = z_max = 0.0;

  for( auto cdl : dl ) {
    delete cdl.md;
  }
  dl.clear();

  label_x = label_y = label_z = "";
}

QSize MglDrawer::getSize0() const
{
  return scd ? QSize( scd->w0, scd->h0 ) : QSize( 50, 50);
}

int MglDrawer::Draw( mglGraph *gr )
{
  if( !gr  || !gra ) {
    return 0;
  }

  gr->SetFontSize( scd->fontSise );
  gr->SetPlotFactor( scd->plotFactor );
  gr->Rotate( scd->phi, scd->theta );
  gr->Light( scd->useLight );
  gr->SetAlphaDef( scd->alpha );
  gr->Alpha( (bool)(scd->useAlpha) );

  gr->SetRanges( x_min, x_max, y_min, y_max, z_min, z_max );

  if( ! scd->autoScX ) {
    gr->SetRange( 'x', scd->plotMinX, scd->plotMaxX );
  }
  if( ! scd->autoScY ) {
    gr->SetRange( 'y', scd->plotMinY, scd->plotMaxY );
  }
  gr->SetTicks( 'x', -(scd->gridX), scd->tickX );
  gr->SetTicks( 'y', -(scd->gridY), scd->tickY );

  string axis_style = color2style( QColor(scd->axis_color).rgb(), 1 ).toStdString();
  string grid_style = color2style( QColor(scd->grid_color).rgb(), 1, "=" ).toStdString();

  gr->Clf( QColor(scd->bgcolor).redF(), QColor(scd->bgcolor).greenF(), QColor(scd->bgcolor).blueF() );


  for( auto cdl : dl ) {
    if( cdl.type < GraphElem::DataType::DataPlot // only real plots
        || cdl.type >= GraphElem::DataType::DataC0 )
    {
      continue;
    }

    switch( cdl.type ) {
      case GraphElem::DataType::DataPlot :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Plot( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Plot( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataStep :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Step( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Step( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataTape :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Tape( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Tape( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataStem :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Stem( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Stem( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataBars :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Bars( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Bars( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataBarh :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            // gr->Barh( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          }
          break;
        }
        if( d_x ) {
          gr->Barh( *d_x, *(cdl.md), cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataTens : // C0: color
        if( cdl.is2D ) {
          if( d_x && d_y && d_c0 ) {
            gr->Tens( *d_x, *d_y, *(cdl.md), *d_c0, cdl.extra.c_str() );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Tens( *d_x, *(cdl.md), *d_c0, "" /*cdl.extra.c_str()*/ ); // TODO: drop only color!
        }
        break;

      case GraphElem::DataType::DataArea :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Area( *d_x, *d_y, *(cdl.md), "B" /*cdl.extra.c_str()*/ ); // TODO: only color?
          }
          break;
        }
        if( d_x ) {
          gr->Area( *d_x, *(cdl.md), "B" /*cdl.extra.c_str()*/ );
        }
        break;
      case GraphElem::DataType::DataRegion :
        break; // unknown for now
      case GraphElem::DataType::DataOHLC :
        break; // unknown for now
      case GraphElem::DataType::DataBoxPlot :
        break; // unknown for now
      case GraphElem::DataType::DataCandle :
        break; // unknown for now
      case GraphElem::DataType::DataCones :
        break; // unknown for now
      case GraphElem::DataType::DataError :
        break; // unknown for now
      case GraphElem::DataType::DataMark : // C0: sz
        if( cdl.is2D ) {
          if( d_x && d_y && d_c0 ) {
            gr->Mark( *d_x, *d_y, *(cdl.md), *d_c0, cdl.extra.c_str() );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Mark( *d_x, *(cdl.md), *d_c0, cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataTube : // C): r
        if( cdl.is2D ) {
          if( d_x && d_y && d_c0 ) {
            gr->Tube( *d_x, *d_y, *(cdl.md), *d_c0, cdl.extra.c_str() );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Tube( *d_x, *(cdl.md), *d_c0, cdl.extra.c_str() );
        }
        break;

      case GraphElem::DataType::DataSurf :
        if( d_x && d_y ) {
          gr->Surf( *d_x, *d_y, *(cdl.md), cdl.extra.c_str() );
          // gr->Surf( *d_x, *d_y, *(cdl.md) );
        }
        break;
      case GraphElem::DataType::DataMesh :
        if( d_x && d_y ) {
          // gr->Mesh( *d_x, *d_y, *(cdl.md), "" /*cdl.extra.c_str()*/ );
          gr->Mesh( *d_x, *d_y, *(cdl.md) );
        }
        break;
      case GraphElem::DataType::DataFall :
      case GraphElem::DataType::DataBelt :
      case GraphElem::DataType::DataDens :
      case GraphElem::DataType::DataCont :
      case GraphElem::DataType::DataContF :
      case GraphElem::DataType::DataContD :
      default: break;
    }
    gr->AddLegend( cdl.label.c_str(), cdl.extra.c_str() );
  }



  gr->Box( axis_style.c_str() );
  gr->Grid( "xyz", grid_style.c_str() );
  gr->Axis( "xyzU3AKDTVISO",  axis_style.c_str() );
  gr->Label( 'x', label_x.c_str() );
  gr->Label( 'y', label_y.c_str() );
  // gr->Label( 'z', label_z.c_str() );
  if( scd->legend_pos < 4 ) {
    gr->Legend( scd->legend_pos, "#" );
  }
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
    DBG1( "ScaleData not found, recreating" );
    scd = new ScaleData( "scd", gra, 0, "scale", "default scale data" );
    scd_del = scd; // to delete on exit
  }

  TModel *model = gra->getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(gra->getFullName()) );
    return;
  }

  int nn = 0, ny = 1, nx = 0, ng = 0;
  double vmin = DMAX, vmax = DMIN; // cross!
  const dvector* ve_x = nullptr;

  QString label_c, extra_c;

  for( auto c : gra->children() ) {
    GraphElem *ge = qobject_cast<GraphElem*>( c );
    if( ! ge ) {
      continue;
    }

    int dtype = GraphElem::DataType::DataNone;
    ge->getData( "type", &dtype );
    int is2D = 0;
    ge->getData( "is2D", &is2D );

    QString src = QString();
    ge->getData( "src", src );
    if( src.isEmpty() ) {
      continue;
    }
    TOutArr *arr = model->getOutArr( src );
    if( !arr ) {
      continue;
    }

    int nn_c = 0;
    arr->getData( "n", &nn_c );
    int ny_c = 1;
    arr->getData( "ny", &ny_c );
    int nx_c = nn_c / ny_c;
    // first array defines dimensions
    if( dl.size() == 0 ) {
      nn = nn_c; nx = nx_c; ny = ny_c;
    }
    else if( nn_c != nn  ||  ny_c != ny  ||  nx_c != nx ) { // only same dimensions
       continue;
    }

    double tmin = 0, tmax = 0;
    arr->getData( "dmin", &tmin );
    arr->getData( "dmax", &tmax );

    if( dtype >= GraphElem::DataType::DataPlot ) { // TODO: combine with bottom part
      if( tmin < vmin ) {
        vmin = tmin;
      }
      if( tmax > vmax ) {
        vmax = tmax;
      }
    }

    label_c = QString( "y_%1" ).arg( ng );
    ge->getData( "label", label_c );
    label_c.prepend( QSN( ng ) + ": " );

    int lw = 1;
    ge->getData( "lw", &lw );
    int i_cc = 0;
    ge->getData( "color", &i_cc );
    QString extra_add = QString( "" );
    ge->getData( "extra", extra_add );
    extra_c = color2style( i_cc, lw, extra_add );

    DBGx( "dbg: adding array \"%s\" type: %d nx= %d, ny=%d nn= %d ng= %d label: \"%s\" extra: \"%s\"",
        qP(arr->getFullName()), dtype, nx, ny, nn, ng, qP(label_c), qP(extra_c) );

    dl.push_back( {
        dtype, is2D, label_c.toStdString(), extra_c.toStdString(), tmin, tmax,
        nullptr, arr->getArray()
        } );
    ++ng;
    // special case: need for squeeze
    if( dtype == GraphElem::DataType::DataAxisX  &&  ! ve_x ) {
      ve_x = arr->getArray();
    }



  }

  // -------------------- squeeze data if needed -----------

  const int max_nn_nosqz = 2000;
  int np = nn; // number of selected points
  vector<uint8_t> plp( nn, 0 ); // flags: point worth to draw

  if( nn > max_nn_nosqz && ve_x ) {  // TODO: no on Surf ....

    const int nd0 = 4;
    double mdlt_y = scd->maxErr * ( vmax - vmin ) / scd->h0; // TODO: current height?
    int stp0 = nn * nd0 / scd->h0;
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
      for( auto cdl : dl ) {
        const dvector* yyc = cdl.ve;
        if( ! yyc ) {
          continue;
        }
        for( int i0 = 0, i1 = 0; i0 < nn-1; i0 = i1 ) {
          for( i1 = i0+1; !plp[i1] && i1 < nn; ++i1 ) /* NOP: find next set*/;
          double x0 = (*ve_x)[i0], x1 = (*ve_x)[i1];
          double y0 = (*yyc)[i0], y1 = (*yyc)[i1];
          if( y0 == y1 ) {
            continue;
          }
          double kxy = ( y1 - y0 ) / ( x1 - x0 );

          // find point of maximum delta
          int i_cm = 0; // index of current maximum
          double dly_cm = 0; // value of this max
          for( int i2=i0+1; i2<i1-1; ++i2 ) {
            double yc = y0 + kxy * ( (*ve_x)[i2] - x0 );
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
  DBGx( "dbg: after squeeze: nn: %d np: %d nx: %d ny: %d ng: %d", nn, np, nx, ny, ng );


  for( auto &cdl : dl ) {
    mglData *md = new mglData( nx, ny );
    // copy squeezed data
    for( int i=0, j=0; i<nn && j<np ; ++i ) {
      if( plp[i] ) {
        md->a[j] = (*cdl.ve)[i];
      ++j;
      }
    }
    cdl.md = md;

    int minmax_idx = -1; // -1: none, 0: x, 1: y, 2: z 3: ?c?

    switch( cdl.type ) {
      case GraphElem::DataType::DataNone :
        break;
      case GraphElem::DataType::DataAxisX:
        d_x = md; label_x = cdl.label;
        minmax_idx = 0;
        break;
      case GraphElem::DataType::DataAxisY :
        d_y = md; label_y = cdl.label;
        minmax_idx = 1;
        break;
      case GraphElem::DataType::DataAxisZ :
        d_z = md; label_z = cdl.label;
        minmax_idx = 2;
        break;
      case GraphElem::DataType::DataPlot :
      case GraphElem::DataType::DataStep :
      case GraphElem::DataType::DataTape :
      case GraphElem::DataType::DataStem :
      case GraphElem::DataType::DataBars :
      case GraphElem::DataType::DataBarh :
      case GraphElem::DataType::DataTens :
      case GraphElem::DataType::DataArea :
      case GraphElem::DataType::DataRegion :
      case GraphElem::DataType::DataOHLC :
      case GraphElem::DataType::DataBoxPlot :
      case GraphElem::DataType::DataCandle :
      case GraphElem::DataType::DataCones :
      case GraphElem::DataType::DataError :
      case GraphElem::DataType::DataMark :
      case GraphElem::DataType::DataTube :
        if( cdl.is2D ) {
          minmax_idx = 2;
        } else {
          minmax_idx = 1;
        }
        break;
      case GraphElem::DataType::DataSurf :
      case GraphElem::DataType::DataMesh :
      case GraphElem::DataType::DataFall :
      case GraphElem::DataType::DataBelt :
      case GraphElem::DataType::DataDens :
      case GraphElem::DataType::DataCont :
      case GraphElem::DataType::DataContF :
      case GraphElem::DataType::DataContD :
        minmax_idx = 2;
        break;
      case GraphElem::DataType::DataC0 :
        d_c0 = md; break;
      case GraphElem::DataType::DataC1 :
        d_c0 = md; break;
      case GraphElem::DataType::DataC2 :
        d_c0 = md; break;
      case GraphElem::DataType::DataC3 :
        d_c0 = md; break;
      case GraphElem::DataType::DataC4 :
        d_c0 = md; break;
      case GraphElem::DataType::DataC5 :
        d_c0 = md; break;
      default:
        DBGx( "warn: unknown type %d label \"%s\"", cdl.type, qP(label_c) );
    }

    // DBGx( "dbg: minmax_idx %d lbl: \"%s\" min: %lf max: %lf",  minmax_idx, cdl.label.c_str(), cdl.v_min, cdl.v_max );
    switch( minmax_idx ) {
      case 0:
        if( cdl.v_min < x_min ) {
          x_min = cdl.v_min;
        }
        if( cdl.v_max > x_max ) {
          x_max = cdl.v_max;
        }
        break;
      case 1:
        if( cdl.v_min < y_min ) {
          y_min = cdl.v_min;
        }
        if( cdl.v_max > y_max ) {
          y_max = cdl.v_max;
        }
        break;
      case 2:
        if( cdl.v_min < z_min ) {
          z_min = cdl.v_min;
        }
        if( cdl.v_max > z_max ) {
          z_max = cdl.v_max;
        }
        break;
      default:
        break;
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


