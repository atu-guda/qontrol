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

#include <mgl2/mgl.h>

#include "labowin.h"
#include "toutarr.h"
#include "tgraph.h"
#include "tmodel.h"
#include "datawidget.h"

#include "mglview.h"

using namespace std;

QString color2style( int color, int lw, const QString &extra )
{
  QColor cc = QRgb( color );
  QString s_cc;
  s_cc.sprintf( "%d{x%02X%02X%02X}", lw, cc.red(),  cc.green(), cc.blue() );
  s_cc += extra;
  return s_cc;
}

QString toQString( const mglPoint &p )
{
  QString s = QString( "[ %1; %2; %3 ]" ).arg( p.x ).arg( p.y ).arg( p.z );
  return s;
}

// ------------------------- MglView ------------------------

MglView::MglView( TGraph *agra, QWidget *parent )
          : QWidget( parent ),
          gra( agra ),
          scd ( new ScaleData( "scd", gra, 0, "scale", "default scale data" ) ),
          pa_fnt ( LaboWin::labowin->getPlotFont() )
{
  QFontMetrics fm( pa_fnt );
  em = fm.width( 'W' );
  ex = fm.height();
  bottom_h = 4 * ex;

  Reload();
  QSize sz0 = getSize0();
  resize( sz0 + QSize( 0, bottom_h ) );

  alc_x = sz0.height(); alc_y = sz0.width();
  pb.resize( alc_x * alc_y * 4 );

  setMinimumSize( 480, 420 );
  setCursor( Qt::CrossCursor );
}

MglView::~MglView()
{
  resetData();
  gra = nullptr; // not delete, we are not owner, just for debug
  delete scd; scd = nullptr;
}

void MglView::resetData()
{
  d_x = d_y = d_z = d_c0 = d_c1 = d_c2 = d_c3 = d_c4 = d_c5 = nullptr;
  pr_min  = { 0, 0, 0 }; pr_max  = { 1, 1, 1 };  pr_dlt = { 1, 1, 1 };

  for( auto cdl : dl ) {
    delete cdl.md;
  }
  dl.clear();

  label_x = label_y = label_z = "";
}

QSize MglView::getSize0() const
{
  return QSize( scd->w0, scd->h0 );
}

int MglView::Draw( mglGraph *gr )
{
  if( !gr  || !gra ) {
    return 0;
  }

  gr->DefaultPlotParam();

  gr->SetFontSize( scd->fontSise );
  // gr->SetMarkSize( 3.0 );
  gr->SetPlotFactor( scd->plotFactor );
  gr->Rotate( scd->phi, scd->theta );
  gr->Light( scd->useLight );
  gr->SetAlphaDef( scd->alpha );
  gr->Alpha( (bool)(scd->useAlpha) );

  pv_dlt = pr_dlt / mag; // realy * by coords
  pv_max = pv_min + pv_dlt;

  gr->SetRanges( pv_min, pv_max );

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
    const char *ext = cdl.extra.c_str();
    const char *opt = cdl.opt.c_str();

    switch( cdl.type ) {
      case GraphElem::DataType::DataPlot :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Plot( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Plot( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataStep :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Step( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Step( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataTape :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Tape( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Tape( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataStem :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Stem( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Stem( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataBars :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Bars( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Bars( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataBarh :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            // gr->Barh( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Barh( *d_x, *(cdl.md), ext, opt );
        }
        break;

      case GraphElem::DataType::DataTens : // C0: color
        if( cdl.is2D ) {
          if( d_x && d_y && d_c0 ) {
            gr->Tens( *d_x, *d_y, *(cdl.md), *d_c0, ext, opt );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Tens( *d_x, *(cdl.md), *d_c0, ext, opt );
        }
        break;

      case GraphElem::DataType::DataArea :
        if( cdl.is2D ) {
          if( d_x && d_y ) {
            gr->Area( *d_x, *d_y, *(cdl.md), ext, opt );
          }
          break;
        }
        if( d_x ) {
          gr->Area( *d_x, *(cdl.md), ext, opt );
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
            gr->Mark( *d_x, *d_y, *(cdl.md), *d_c0, ext, opt );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Mark( *d_x, *(cdl.md), *d_c0, ext, opt  );
        }
        break;

      case GraphElem::DataType::DataTube : // C0: r
        if( cdl.is2D ) {
          if( d_x && d_y && d_c0 ) {
            gr->Tube( *d_x, *d_y, *(cdl.md), *d_c0, ext, opt );
          }
          break;
        }
        if( d_x && d_c0 ) {
          gr->Tube( *d_x, *(cdl.md), *d_c0, ext, opt );
        }
        break;

      case GraphElem::DataType::DataSurf :
        if( d_x && d_y ) {
          gr->Surf( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataSurfC :
        if( d_x && d_y && d_c0 ) {
          gr->SurfC( *d_x, *d_y, *(cdl.md), *d_c0, ext, opt );
        }
        break;
      case GraphElem::DataType::DataSurfA :
        if( d_x && d_y && d_c0 ) {
          gr->SurfA( *d_x, *d_y, *(cdl.md), *d_c0, ext, opt );
        }
        break;
      case GraphElem::DataType::DataMesh :
        if( d_x && d_y ) {
          gr->Mesh( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataFall :
        if( d_x && d_y ) {
          gr->Fall( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataBelt :
        if( d_x && d_y ) {
          gr->Belt( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataDens :
        if( d_x && d_y ) {
          gr->Dens( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataCont :
        if( d_x && d_y ) {
          gr->Cont( *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      case GraphElem::DataType::DataContF : // C0: v
        if( d_x && d_y && d_c0) {
          gr->ContF( *d_c0, *d_x, *d_y, *(cdl.md),  ext, opt );
        }
        break;
      case GraphElem::DataType::DataContD : // C0: d
        if( d_x && d_y && d_c0) {
          gr->ContD( *d_c0, *d_x, *d_y, *(cdl.md), ext, opt );
        }
        break;
      default: break;
    }
    gr->AddLegend( cdl.label.c_str(), ext );
  }



  gr->Box( axis_style.c_str() );
  gr->Grid( "xyz", grid_style.c_str() );
  gr->Axis( "xyzU3AKDTVISO",  axis_style.c_str() );
  gr->Label( 'x', label_x.c_str() );
  gr->Label( 'y', label_y.c_str() );
  gr->Mark( mark_point, "3r+" );
  gr->Mark( base_point, "3B*" );
  // gr->Error( base_point, mglPoint( x_max-x_min, y_max-y_min, z_max-z_min ), "B" );
  gr->Error( base_point, pr_dlt, "B" );
  // gr->Label( 'z', label_z.c_str() );

  if( scd->legend_pos < 4 ) {
    gr->Legend( scd->legend_pos, "#" );
  }

  return 1;
}

void MglView::Reload( )
{
  resetData();

  if( !gra ) {
    DBG1( "err: no TGraph passed to MglDrawer" );
    return;
  }

  scd_o =  gra->getElemT<ScaleData*>( "scd" );
  if( scd_o ) {
    // terrble copy
    QString s = scd_o->toString();
    scd->fromString( s );
  }
  int ch = 30;
  scd->getData( "h0", &ch );
  DBGx( "dbg: Reload()! ch=%d", ch );

  TModel *model = gra->getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(gra->getFullName()) );
    return;
  }

  int nn = 0, ny = 1, nx = 0;
  double vmin = DMAX, vmax = DMIN; // cross!
  const dvector* ve_x = nullptr;

  QString label_c, extra_c, opt_c;

  for( auto c : gra->children() ) {
    GraphElem *ge = qobject_cast<GraphElem*>( c );
    if( ! ge ) {
      continue;
    }

    int dtype = GraphElem::DataType::DataNone;
    ge->getData( "type", &dtype );
    int is2D = 0;
    ge->getData( "is2D", &is2D );
    int noColor = 0;
    ge->getData( "noColor", &noColor );

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

    label_c = QString( "y_%1" ).arg( dl.size() );
    ge->getData( "label", label_c );
    // label_c.prepend( QSN( ng ) + ": " );

    int lw = 1;
    ge->getData( "lw", &lw );
    int i_cc = 0;
    ge->getData( "color", &i_cc );
    QString extra_add = QString( "" );
    ge->getData( "extra", extra_add );
    if( noColor ) {
      extra_c = extra_add;
    } else {
      extra_c = color2style( i_cc, lw, extra_add );
    }

    opt_c = QString();
    ge->getData( "opt", opt_c );

    DBGx( "dbg: adding array \"%s\" type: %d nx= %d, ny=%d nn= %d label: \"%s\" extra: \"%s\"",
        qP(arr->getFullName()), dtype, nx, ny, nn, qP(label_c), qP(extra_c) );

    dl.push_back( {
        dtype, is2D, -1, label_c.toStdString(), extra_c.toStdString(), opt_c.toStdString(),
        tmin, tmax,  nullptr, arr->getArray()
        } );
    // special case: need for squeeze
    if( dtype == GraphElem::DataType::DataAxisX  &&  ! ve_x ) {
      ve_x = arr->getArray();
    }



  }

  // -------------------- squeeze data if needed -----------

  const int max_nn_nosqz = 2000;
  int np = nn; // number of selected points
  vector<uint8_t> plp( nn, 0 ); // flags: point worth to draw

  if( nn > max_nn_nosqz && ve_x && ny == 1 ) {  // TODO: no on Surf (ny?)

    const int nd0 = 4;
    double mdlt_y = scd->maxErr * ( vmax - vmin ) / ch;
    int stp0 = nn * nd0 / ch;
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
    nx = np;
  } else {
    plp.assign( nn, 1 ); // all point are good
  }
  DBGx( "dbg: after squeeze: nn: %d np: %d nx: %d ny: %d ", nn, np, nx, ny );


  int ng = 0;
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
    int add_ng = 0;

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
        add_ng = 1;
        if( cdl.is2D ) {
          minmax_idx = 2;
        } else {
          minmax_idx = 1;
        }
        break;
      case GraphElem::DataType::DataSurf :
      case GraphElem::DataType::DataSurfC :
      case GraphElem::DataType::DataSurfA :
      case GraphElem::DataType::DataMesh :
      case GraphElem::DataType::DataFall :
      case GraphElem::DataType::DataBelt :
      case GraphElem::DataType::DataDens :
      case GraphElem::DataType::DataCont :
      case GraphElem::DataType::DataContF :
      case GraphElem::DataType::DataContD :
        add_ng = 1; minmax_idx = 2;
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

    if( add_ng ) {
      cdl.ig = ng++;
      cdl.label = to_string( cdl.ig ) + ": " + cdl.label;
    }

    switch( minmax_idx ) {
      case 0:
        if( cdl.v_min < pr_min.x ) {
          pr_min.x = cdl.v_min;
        }
        if( cdl.v_max > pr_max.x ) {
          pr_max.x = cdl.v_max;
        }
        break;
      case 1:
        if( cdl.v_min < pr_min.y ) {
          pr_min.y = cdl.v_min;
        }
        if( cdl.v_max > pr_max.y ) {
          pr_max.y = cdl.v_max;
        }
        break;
      case 2:
        if( cdl.v_min < pr_min.z ) {
          pr_min.z = cdl.v_min;
        }
        if( cdl.v_max > pr_max.z ) {
          pr_max.z = cdl.v_max;
        }
        break;
      default:
        break;
    }
  }

  if( ! scd->autoScX ) { // manual scale from config
    pr_min.x =  scd->plotMinX; pr_max.x =  scd->plotMaxX;
  }
  if( ! scd->autoScY ) {
    pr_min.y =  scd->plotMinY; pr_max.y =  scd->plotMaxY;
  }

  mag = { 1, 1, 1 };
  pr_dlt = pr_max - pr_min;
  pv_min = pr_min; //   pv_max = pr_max; pv_dlt = pr_dlt; recalced on Draw

}


void MglView::zoom()
{
  mglPoint p0 = base_point, p1 = mark_point;
  if( base_point == mark_point ) {
    p0 -= 0.25 * pv_dlt; p1 += 0.25 * pv_dlt;
  }
  pv_min = p0; pv_max = p1; pv_dlt = pv_max - pv_min;
  mag = mglPoint( pv_dlt.x / pr_dlt.x, pv_dlt.y / pr_dlt.y, pv_dlt.z / pr_dlt.z );
  update();
}

QSize MglView::sizeHint() const
{
  return getSize0() + QSize( 0, bottom_h );
}

void MglView::drawAll( QPainter &p )
{
  int w = width(), h = height(), hg = h - bottom_h;
  gr.SetSize( w, hg );
  Draw( &gr );

  gr.GetBGRN( pb.data(), 4*w*hg );

  QPixmap pic = QPixmap::fromImage( QImage( pb.data(), w, hg, QImage::Format_RGB32 ));

  p.setFont( pa_fnt );
  // p.fillRect( 0, 0, w, h, QBrush( QColor(128,255,255) ) );
  p.drawPixmap( 0, 0, pic );
  drawFooter( p, hg );
}

void MglView::paintEvent( QPaintEvent * /*pe*/ )
{
  QPainter p( this );
  drawAll( p );
  p.end();

}

void MglView::drawFooter( QPainter &p, int hg )
{
  int w = width();
  QColor bg_c = QColor( scd->bgcolor );
  QColor fg_c = QColor( 0,0,0 );
  if( bg_c.value() < 128 ) {
    fg_c = QColor( 255, 255, 255 );
  }

  p.fillRect( 0, hg, w, bottom_h, QBrush( bg_c ) );
  p.setPen( fg_c );
  p.drawLine( 0, hg, w, hg );

  mglPoint rel_p = mark_point - base_point;
  double kyx = 0.0, kzx = 0.0;
  if( rel_p.x != 0 ) {
    kyx = rel_p.y / rel_p.x;
    kzx = rel_p.z / rel_p.x;
  }

  QString s = QString( "mark: ") % toQString( mark_point )
     % "  base: " % toQString( base_point )
     % "  rel: " % toQString( rel_p ) % " kyx: " % QSN( kyx ) % " kzx: " % QSN( kzx )
     % "\n  vis: " % toQString( pv_min ) % " - " % toQString( pv_max )
     % " D: " % toQString( pv_dlt )
     % "  mag: " % toQString( mag )
     % "  real: " % toQString( pr_min ) % " - " % toQString( pr_max );
  p.drawText( ex, hg, w-2*ex, bottom_h, Qt::AlignLeft, s );
}

void MglView::mousePressEvent( QMouseEvent *me )
{
  int mx = me->x(), my = me->y(), btn = me->button();
  mglPoint po = gr.CalcXYZ( mx, my );
  // DBGx( "dbg: mouse: x: %d y: %d ; graph: x: %lf y: %lf z: %lf",
  //        mx, my, mark_point.x, mark_point.y, mark_point.z );
  switch( btn ) {
    case Qt::LeftButton:
      mark_point = po;
         break;
    case Qt::RightButton:
         break;
    case Qt::MidButton:
         break;
    default:
         break;
  };
  update();

}

void MglView::keyPressEvent( QKeyEvent *ke )
{
  constexpr int Sh = 0x40000000;
  constexpr int Ct = 0x80000000;
  int k = ke->key(), st = ke->modifiers();
  if( st & Qt::ShiftModifier ) {
      k |= Sh;
  }
  if( st & Qt::ControlModifier ) {
      k |= Ct;
  }

  switch( k ) {
    case Qt::Key_F1:
      showHelp();
      break;
    case Qt::Key_M:
      markToBase();
      break;
    case Qt::Key_Q:
      parentWidget()->close();
      break;
    case Qt::Key_R | Sh:
      resetScale();
      break;
    case Qt::Key_P | Ct:
      printPlot();
      break;
    case Qt::Key_E:
      exportPlot();
      break;
    case Qt::Key_G:
      setMark();
      break;
    case Qt::Key_S:
      setScale();
      break;
    case Qt::Key_S | Sh:
      saveScale();
      break;
    case Qt::Key_A:
      setAlpha( 0.1, true );
      break;
    case Qt::Key_A | Sh :
      setAlpha( -0.1, true );
      break;

    // Shift
    case Qt::Key_Up:
      setYbase( -pv_dlt.y * scale_step, true );
      break;
    case Qt::Key_Down:
      setYbase( pv_dlt.y * scale_step, true );
      break;
    case Qt::Key_Left:
      setXbase( pv_dlt.x * scale_step, true );
      break;
    case Qt::Key_Right:
      setXbase( -pv_dlt.x * scale_step, true );
      break;
    case Qt::Key_PageUp:
      setZbase( -pv_dlt.z * scale_step, true );
      break;
    case Qt::Key_PageDown:
      setZbase( pv_dlt.z * scale_step, true );
      break;

    // Rotate
    case Qt::Key_Up | Sh:
      setPhi( angle_step, true );
      break;
    case Qt::Key_Down | Sh:
      setPhi( -angle_step, true );
      break;
    case Qt::Key_Left | Sh:
      setTheta( angle_step, true );
      break;
    case Qt::Key_Right | Sh:
      setTheta( -angle_step, true );
      break;

    case Qt::Key_Plus:
      setXmag( mag_step, true );
      break;
    case Qt::Key_Minus:
      setXmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Plus | Sh:
      setYmag( mag_step, true );
      break;
    case Qt::Key_Minus | Sh:
      setYmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Plus | Ct:
      setZmag( mag_step, true );
      break;
    case Qt::Key_Minus | Ct:
      setZmag( 1.0/mag_step, true );
      break;
    case Qt::Key_Z:
      zoom();
      break;
  }

  update();
}

void MglView::resizeEvent( QResizeEvent *e )
{
  QSize cs = size();
  alc_x = cs.width(); alc_y = cs.height();
  pb.resize( alc_x * alc_y * 4 ); // sub bottom_h ?

  QWidget::resizeEvent( e );
}

void MglView::resetScale()
{
  Reload();
  // TODO: more
  update();
}

void MglView::markToBase()
{
  base_point = mark_point;
  update();
}

void MglView::setPhi( double a_phi, bool add )
{
  if( add ) {
    scd->phi += a_phi;
  } else {
    scd->phi  = a_phi;
  }
  update();
}

void MglView::setTheta( double a_theta, bool add )
{
  if( add ) {
    scd->theta += a_theta;
  } else {
    scd->theta  = a_theta;
  }
  update();
}

void MglView::setXmag( double amag, bool mul )
{
  if( mul ) {
    mag.x *= amag;
  } else {
    mag.x = amag;
  }
  update();
}

void MglView::setYmag( double amag, bool mul )
{
  if( mul ) {
    mag.y *= amag;
  } else {
    mag.y = amag;
  }
  update();
}

void MglView::setZmag( double amag, bool mul )
{
  if( mul ) {
    mag.z *= amag;
  } else {
    mag.z = amag;
  }
  update();
}

void MglView::setXbase( double base, bool rel )
{
  if( rel ) {
    pv_min.x += base;
  } else {
    pv_min.x  = base;
  }
  update();
}

void MglView::setYbase( double base, bool rel )
{
  if( rel ) {
    pv_min.y += base;
  } else {
    pv_min.y  = base;
  }
  update();
}

void MglView::setZbase( double base, bool rel )
{
  if( rel ) {
    pv_min.z += base;
  } else {
    pv_min.z  = base;
  }
  update();
}

void MglView::setAlpha( double al, bool rel )
{
  if( rel ) {
    scd->alpha += al;
  } else {
    scd->alpha  = al;
  }
  scd->alpha = qBound( 0.0, (double)(scd->alpha), 1.0 );
  update();
}


void MglView::setMark()
{
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( "Set mark to point: " );
  QGridLayout *lay = new QGridLayout( dia );

  QLineEdit *markX_ed = new QLineEdit( dia );
  markX_ed->setValidator( new QDoubleValidator() );
  markX_ed->setText( QSN( mark_point.x ) );
  lay->addWidget( markX_ed, 0, 1 );

  QLineEdit *markY_ed = new QLineEdit( dia );
  markY_ed->setValidator( new QDoubleValidator() );
  markY_ed->setText( QSN( mark_point.y ) );
  lay->addWidget( markY_ed, 1, 1 );

  QLineEdit *markZ_ed = new QLineEdit( dia );
  markZ_ed->setValidator( new QDoubleValidator() );
  markZ_ed->setText( QSN( mark_point.z ) );
  lay->addWidget( markZ_ed, 2, 1 );

  QLabel *la_x = new QLabel( "&X value", dia );
  la_x->setBuddy( markX_ed );
  lay->addWidget( la_x, 0, 0 );
  QLabel *la_y = new QLabel( "&Y value", dia );
  la_y->setBuddy( markY_ed );
  lay->addWidget( la_y, 1, 0 );
  QLabel *la_z = new QLabel( "&Z value", dia );
  la_z->setBuddy( markZ_ed );
  lay->addWidget( la_z, 2, 0 );

  QDialogButtonBox *bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 3, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    mark_point.x = markX_ed->text().toDouble();
    mark_point.y = markY_ed->text().toDouble();
    mark_point.z = markZ_ed->text().toDouble();
  };
  delete dia;
  update();

}

void MglView::setScale()
{
  DataDialog *dia = new DataDialog( *scd, this );

  int rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    update();
  };
}

void MglView::saveScale()
{
  if( !scd_o ) {
    return;
  }
  if( QMessageBox::question( this, "Save scale settings",
        "Are you sure to save current scale settings?" )
      == QMessageBox::Yes )
  {
    // terrble copy
    QString s = scd->toString();
    scd_o->fromString( s );
  }
}

void MglView::printPlot()
{
  QPrinter *pr;
  if( ! LaboWin::labowin ) {
    return;
  }
  pr = LaboWin::labowin->getPrinter();
  if( !pr ) {
    return;
  }

  QPrintDialog pr_dialog( pr, this );
  if( pr_dialog.exec() ) {
    pr->setFullPage( false );
    pr->newPage();
    QPainter p( pr );
    drawAll( p );
    p.end();
  };

}

void MglView::exportPlot()
{
  QString fn = QFileDialog::getSaveFileName( this, "Save Picture", "out.png",
               "PNG files (*.png);;All files (*)" );
  if( fn.isEmpty() ) {
    return;
  };

  QImage timg( width(), height(), QImage::Format_RGB32 );
  timg.fill( 0xFF );
  QPainter painter( &timg );
  render( &painter );

  if( ! timg.save( fn, "PNG", 50 ) ) {
    QString err = strerror(errno);
    QMessageBox::warning(this, "Fail to save picture",
        QString("Fail fo save picture file: \"%1\": %2").arg( fn ).arg( err ),
        QMessageBox::Ok );
  }


}

static const char plot_helpstr[] = "<b>Hot keys:</b><br/>\n"
 "<b>q</b> - close <br/>\n"
 "<b>F1</b> - this help <br/>\n"
 "<b>a/A</b> - change alpha <br/>\n"
 "<b>o/O</b> - on/off non-selected plots <br/>\n"
 "<b>s/S</b> - scale dialog / save current scale <br/>\n"
 "<b>Ctrl-P</b> - print<br/>\n"
 "<b>R</b> - reset and redraw<br/>\n"
 "<b>e</b> - export image to png <br/>\n"
 "<b>m</b> - set base to mark<br/>\n"
 "<b>g</b> - set mark to given point <br/>\n"
 "<b>l/L</b> - link/Unlink to data <br/>\n"
 "<b>d</b> - data info <br/>\n"
 "<b>z</b> - zoom base - mark, or near mark <br/>\n"
 "<b>PageUp, PageDown</b> - move Up/Down <br>\n"
 "<b>+/- Shift, Ctrl</b> - Zoom on X (Y,Z) <br>\n"
 "<b> - not in link state: - </b> <br/>\n"
 "<b>Left, Top, Right, Bottom</b> - Move <br/>\n"
 "<b>[Shift]{Left, Top, Right, Bottom}</b> - Rotate <br>\n"
 "<b>Ctrl {Left, Top, Right, Bottom}</b> - shift plot <br>\n"
 "<b>- in link state: -</b>  <br>\n"
 "<b>[Shift]{Left, Top}</b> - prev/next point [more] <br>\n"
 "<b>[Shift]{Top, Bottom}}</b> - find [global] max/min <br>\n";

void MglView::showHelp()
{
  QMessageBox::information( this,"Hot keys in plot window", plot_helpstr );
}

