/***************************************************************************
                          qplotview.cpp  -  description
                             -------------------
    begin                : Sun Aug 20 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// for debug:
// #include <stdio.h>

#include <cmath>
#include <QtGui>

#include "resource.h"
#include "miscfun.h"
#include "dataset.h"
#include "qanydial.h"
#include "toutarr.h"
#include "tgraph.h"
#include "qcolorbtn.h"
#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"

#include "qplotview.h"

using namespace std;

QPlotView::QPlotView( QMo2Doc *adoc, TGraph *agra,
                      QWidget *parent, const char *name )
          : QWidget( parent, name )
{
  int i;
  doc = adoc; gra = agra;
  datax = 0; plpbuf = 0;
  for( i=0; i<6; i++ ) { datay[i] = 0; plotOn[i] = 1; plp[i] = 0; };
  nxGrid = nyGrid = 10; nxTick = nyTick = 1;
  asX = asY = gsX = gsY = 1; modelSign = -1; devTp = 0;
  logX = logScaleX = logY = logScaleY = zeroX = centerX = zeroY = centerY = 0;
  realMinX = realMinY = plotMinX = plotMinY = 0;
  realMaxX = realMaxY = plotMaxX = plotMaxY = 1;
  plotMinLX = plotMinLY = 0;
  leftMar = 0.1; topMar = 0.02; rightMar = 0.1; bottomMar = 0.1; maxErr = 0.5;
  pix_h = 450; pix_w = 550; 
  leg_sz = 30; 
  sel_g = -1; min_sel_g = -1; max_sel_g = -1;
  sel_idx = -1; qual = 0; linew = 0; need_rescale = 1;
  tool_x = tool_y = ref_x = ref_y = 0;
  ng = 0; nn = 0; ny = 0;
  errstr = 0;
  fontfam = "helvetica";
  setStartColors();
  setBackgroundMode( Qt::NoBackground );
  setFocusPolicy( Qt::StrongFocus );
  setMinimumSize( 350, 280 );
  setCursor( Qt::crossCursor );
}

QPlotView::~QPlotView()
{
  delete plpbuf; plpbuf = 0;
}

QSize QPlotView::sizeHint(void) const
{
  return QSize( 550, 450 );
}

void QPlotView::resizeEvent( QResizeEvent *e )
{
  need_rescale = 1;
  QWidget::resizeEvent( e );
}
void QPlotView::paintEvent( QPaintEvent * /*pe*/ )
{
  devTp = 0;
  QPixmap pix( width(), height() );
  pix.fill( bgColor );
  QPainter p( &pix );
  drawAll( p );
  p.end();
  bitBlt( this, QPoint(0,0), &pix );
}

void QPlotView::printPlot( void )
{
  int k;
  QPrinter *pr;
  if( QMo2Win::qmo2win == 0 ) return;
  pr = QMo2Win::qmo2win->getPrinter();
  if( pr == 0 ) return;
  k = pr->setup();
  if( k ) {
    devTp = 1;
    pr->setFullPage( FALSE );
    pr->newPage();
    QPainter p( pr ); 
    drawAll( p );
    p.end();
  };
}

void QPlotView::drawAll( QPainter &p )
{
  int currSgnt;
  pix_h = height(); pix_w = width(); 
  root = doc->getRoot(); model = doc->getModel();
  currSgnt = -2;
  if( model != 0 )
    model->getDataSI( "sgnt", &currSgnt, 0 );
  if( currSgnt != modelSign ) {
    ng = 0; 
    if( currSgnt != -2 ) modelSign = currSgnt;
  };
  if( ng == 0 )
    initArrs();
  if( need_rescale ) {
    calcScale(); 
  };
  if( devTp == 1 ) {
    linew = 1;
  };
  if( ! errstr ) {
    QFont smlf;
    smlf.fromString( QMo2Win::qmo2win->getSettings()->plotFont );
    p.setFont( smlf );
    drawGrid( p );
    drawPlots( p );
    drawAxes( p );
  } else {
    p.setPen( Qt::yellow );
    p.drawText( 20, 20, errstr );
  };
  qual = 0; linew = 0;
}

void QPlotView::drawGrid( QPainter &p )
{
  int i, j, ix, iy;
  double vx, vy, lvx, lvy;
  if( nxGrid > 0 ) {
    for( i=0; i<=nxGrid; i++ ) {  // vertical
      if( logX && logScaleX ) {
	vx = pow( 10, plotMinLX + i*(log10(plotMaxX)-plotMinLX) / nxGrid );
	lvx = log10( vx );
      } else {
	vx = plotMinX + ( plotMaxX - plotMinX ) * i / nxGrid; lvx = vx;
      };
      phys2vis( vx, 0, &ix, 0 );
      p.setPen( QPen( gridColor, 0 /*linew*/) );
      p.drawLine( ix, pix_y0,  ix, pix_y0 - pix_y );
      p.setPen( QPen( labelColor, linew ) );
      p.drawText( ix - 20, pix_y0 + 2, 40, 12, Qt::AlignHCenter,
                QString::number( lvx ) );
    };
  };
  if( nyGrid > 0 ) {
  for( j=0; j<=nyGrid; j++ ) {  // horizontal
      if( logY && logScaleY ) {
	vy = pow( 10, plotMinLY + j*(log10(plotMaxY)-plotMinLY) / nyGrid );
	lvy = log10( vy );
      } else {
	vy = plotMinY + ( plotMaxY - plotMinY ) * j / nyGrid; lvy = vy;
      };
      phys2vis( 0, vy, 0, &iy );
      p.setPen( QPen( gridColor, 0 /*linew*/ ) );
      p.drawLine( pix_l, iy, pix_x0 + pix_x, iy );
      p.setPen( QPen( labelColor, linew ) );
      p.drawText( 1, iy - 6, pix_l-2, 12, Qt::AlignRight,
                  QString::number( lvy ) );
    };
  };
  // -------- draw ticks
  if( nxGrid > 0  &&  nxTick > 0  &&  nxTick < 10 ) {
    j = nxGrid * (nxTick + 1);
    for( i=0; i<j; i++ ) {
      if( logX && logScaleX ) {
	vx = pow( 10, plotMinLX + i*(log10(plotMaxX)-plotMinLX) / j );
      } else {
	vx = plotMinX + ( plotMaxX - plotMinX ) * i / j;
      };
      phys2vis( vx, 0, &ix, 0 );
      p.drawLine( ix, pix_y0, ix, pix_y0-3 );
    };
  };
  if( nyGrid > 0  &&  nyTick > 0  &&  nyTick < 10 ) {
    j = nyGrid * (nyTick + 1);
    for( i=0; i<j; i++ ) {
      if( logY && logScaleY ) {
	vy = pow( 10, plotMinLY + i*(log10(plotMaxY)-plotMinLY) / j );
      } else {
	vy = plotMinY + ( plotMaxY - plotMinY ) * i / j;
      };
      phys2vis( 0, vy, 0, &iy );
      p.drawLine( pix_x0, iy, pix_x0 + 3, iy );
    };
  };
}

void QPlotView::drawAxes( QPainter &p )
{
  int i, ix, iy; 
  double toolk, tooldx, tooldy;
  QString qs;
  p.setPen( QPen( scaleColor, linew+1 ) ); // ------ draw axis
  p.drawLine( pix_x0, pix_y0, pix_x0 + pix_x, pix_y0 );
  p.drawLine( pix_x0, pix_y0, pix_x0, pix_y0 - pix_y );
  if( ref_x >= plotMinX && ref_y >= plotMinY && // draw reference point
      ref_x <= plotMaxX && ref_y <= plotMaxY ) {
    phys2vis( ref_x, ref_y, &ix, &iy );
    p.setPen( QPen( scaleColor, linew ) );
    p.drawLine( pix_x0 - 5, iy, pix_x0 + pix_x , iy );
    p.drawLine( ix, pix_t  - 2, ix, pix_y0 + 5 );
  };
  
  // -------- draw labels
  if( pix_r >= leg_sz ) {
    p.setPen( labelColor ); p.setBrush( Qt::NoBrush );
    p.drawText( pix_x0 + pix_x + 5, pix_y0 + 6, xLabel );
    p.drawText( pix_x0 + pix_x + 5, pix_y0 - 14, QString::number(nn) );
    p.drawText( pix_x0 + pix_x + 5, pix_y0 - 24, QString::number(ny) );
    for( i=0; i<ng; i++ ) {
      p.setPen( QPen( plotColor[i], linew ) );
      p.drawLine( pix_x0 + pix_x + 2, pix_t + 10 + i*20,
	  pix_x0 + pix_x + 10, pix_t + 10 + i*20 );
      if( i == sel_g )
        p.drawRect( pix_x0 + pix_x + 2, pix_t + 8 + i*20, 9, 5 );
      p.drawText( pix_x0 + pix_x + 12, pix_t + 16 + i*20, yLabel[i] );
    };
  };
  if( devTp < 1 ) {
    if( qual == 1 || nn <= 1000 ) {  // quality mark at right bottom
      p.setBrush( Qt::yellow );
    } else {
      p.setBrush( Qt::red );
    };
    p.drawRect( pix_w - 8, pix_h - 8, 7, 7 );
    if( ny > 1 ) {  // mark - 3D-like plot
      p.setBrush( Qt::green );
      p.drawRect( pix_w - 16, pix_h - 8, 7, 7 );
    };
  };
  // draw tool cross
  p.setPen( QPen( scaleColor, linew ) ); p.setBrush( Qt::NoBrush );
  phys2vis( tool_x, tool_y, &ix, &iy );
  p.drawLine( ix-10, iy, ix-3, iy );  p.drawLine( ix+10, iy, ix+3, iy );
  p.drawLine( ix, iy-10, ix, iy-3 );  p.drawLine( ix, iy+10, ix, iy+3 );
  p.drawLine( pix_x0-8, iy, pix_x0, iy );
  p.drawLine( ix, pix_y0+8, ix, pix_y0 );
  toolk = 0; tooldx = tool_x - ref_x; tooldy = tool_y - ref_y;
  if( tooldx != 0) 
    toolk = tooldy / tooldx;
  qs = QString("(") 
    + QString::number( tool_x, 'g', 4 ) + QString("; " )
    + QString::number( tool_y, 'g', 4 ) + QString(") [" )
    + QString::number( sel_idx ) + QString("]; ref= (" )
    + QString::number( ref_x, 'g', 4 ) + QString("; " ) 
    + QString::number( ref_y, 'g', 4 ) + QString(") dlt = (" ) 
    + QString::number( tooldx, 'g', 4 ) + QString("; " ) 
    + QString::number( tooldy, 'g', 4 ) + QString(") R = " ) 
    + QString::number( euRange( tooldx, tooldy  ), 'g', 4 ) 
    + QString(" ; f = " ) 
    + QString::number( atan2( tooldy, tooldx ), 'g', 4 ) 
    + QString(" ; k = " ) + QString::number( toolk, 'g', 4 );
  if( ny > 1 && sel_idx >= 0 ) {
    qs += QString( "; y= " ) + QString::number( datay[0][sel_idx], 'g', 4 );
  };
  p.drawText( 2, pix_h-12, qs  );
  if( devTp == 0 ) {
    p.setCompositionMode( QPainter::CompositionMode_Xor ); 
  };
  p.setPen( QPen( QColor(255,255,255), linew ) );
  p.drawPoint( ix, iy ); 
  if( devTp == 0 ) {
    p.setCompositionMode( QPainter::CompositionMode_SourceOver ); 
  };
}

void QPlotView::drawPlots( QPainter &p )
{
  int i, j, j_st, ix, iy;
  j_st = 0; 
  
  if( ny > 1 ) { // if 3D-plot, skip 2 first data arrays
    j_st = 1;
  };

  for( j=j_st; j<ng; j++ ) {  // all graphs

    if( j == sel_g ) // seleceted printed last
      continue;

    if( plotOn[j] == 0 )
        continue;
    
    QPainterPath path;

    for( i=0; i<nn; i++ ) {
      phys2vis( datax[i], datay[j][i], &ix, &iy );
      if( i == 0 || plp[j][i] == 2 ) {
	path.moveTo( ix, iy ); 
      } else {
	if( ( plp[j][i] & 1 ) || qual == 1 ) {	  
	  path.lineTo( ix, iy );
	};
      };
    };   // - end of nn loop;
    p.setPen( QPen( plotColor[j], linew ) );
    p.drawPath( path );

  };   // - end of ng loop

  if( sel_g >=0 && sel_g < ng ) {  // draw selected
    
    p.setPen( QPen( plotColor[sel_g], linew ) );
    QPainterPath path;

    for( i=0; i<nn; i++ ) {
      phys2vis( datax[i], datay[sel_g][i], &ix, &iy );
      if( i == 0 || plp[sel_g][i] == 2  ) { // start of line
	path.moveTo( ix, iy ); 
      } else {
	if( ( plp[sel_g][i] & 1 ) || qual == 1 ) {
	  if( maxErr > 0 ) {
	    path.lineTo( ix, iy );
	  } else {
	    p.drawPoint( ix, iy );
	  };
	};
      };
    };   // - end of nn loop;
    p.drawPath( path );

  };
}

void QPlotView::mousePressEvent( QMouseEvent *me )
{
  int x, y, btn, no;
  double vx, vy;
  if( errstr )  // dont need mouse in bad plot
    return;
  x = me->x(); y = me->y(); btn = me->button();
  vis2phys( x, y, &vx, &vy );
  no = -1;
  if( x > pix_x0 + pix_x + 2 )          // legens zone
    no = ( y - pix_t - 10 ) / 20;
  if( no < 0 || no >= ng )
    no = -1;
  switch( btn ) {
    case Qt::LeftButton:
         if( no >= 0 && no >= min_sel_g && no <= max_sel_g ) { // select plot
           sel_g = no; plotOn[sel_g] = 1; sel_idx = -1;
         } else {        // move tool point
           tool_x = vx; tool_y = vy; sel_idx = -1;
         };
         break;
    case Qt::RightButton:
         if( no >= 0 && no != sel_g ) { // on/off plot line
           plotOn[no] = !plotOn[no];
         } else {
           // TODO: menu here
         };
         break;
    case Qt::MidButton:
         //QMessageBox::information( this, "Coordinates",
         //          QString("Middle button pressed at :")
         //          + QString::number( vx ) + QString(", ")
         //          + QString::number( vy ),
         //          0, 0, 0 );
         break;
    default:
         break;
  };
  update();
}

void QPlotView::keyPressEvent( QKeyEvent *ke )
{
  int k, st, btnCtrl, btnShift, i, di;
  double dvx, dvy, adx, ady;
  k = ke->key(); st = ke->state();
  btnShift = (( st & Qt::ShiftModifier ) != 0 );
  btnCtrl = (( st & Qt::ControlModifier ) != 0 );
  if( btnShift ) {
    dvx = ( plotMaxX - plotMinX ) / 20;
    dvy = ( plotMaxY - plotMinY ) / 20;
    di = nn / 50;
  } else {
    dvx = 1 / kx; dvy = 1 / ky; di = 1;
  };
  switch( k ) {
    case Qt::Key_Q: parentWidget()->close( true ); break;
    case Qt::Key_A: qual = 1; linew = 1; break;
    case Qt::Key_O:
         if( sel_g < 0 ) break;
	 for( i=0; i<ng; i++ ) {
	   if( i == sel_g ) continue;
	   plotOn[i] = btnShift;
	 };
         break;
    case Qt::Key_S:
	 if( btnShift ){
	   asX = gsX = asY = gsY = 1;
	   zeroX = centerX = logX = zeroY = centerY = logY = 0;
	   need_rescale = 1;
	 } else {
	   setScale();
	 };  
	 break;
    case Qt::Key_Plus:
	 asX = asY = 0;
	 adx = ( plotMaxX - plotMinX ) / 4;
         ady = ( plotMaxY - plotMinY ) / 4;
	 plotMinX = tool_x - adx; plotMaxX = tool_x + adx;
         plotMinY = tool_y - ady; plotMaxY = tool_y + ady;
	 need_rescale = 1;
	 break;
    case Qt::Key_Minus:
	 asX = asY = 0;
	 adx = plotMaxX - plotMinX;
         ady = plotMaxY - plotMinY;
	 plotMinX = tool_x - adx; plotMaxX = tool_x + adx;
         plotMinY = tool_y - ady; plotMaxY = tool_y + ady;
	 need_rescale = 1;
	 break;
    case Qt::Key_C:
	 if( btnShift )
	   setStartColors();
	 else
	   setColors();
	 break;
    case Qt::Key_M:
	 if( btnCtrl ) {
	   ref_x = logX; ref_y = logY;
	 } else {
	   ref_x = tool_x; ref_y = tool_y;
	 };
	 break;
    case Qt::Key_G: moveTool(); break;
    case Qt::Key_H: case Qt::Key_F1: showHelp(); break;
    case Qt::Key_L:
	 if( btnShift ) { sel_idx = -1; break; };
         if( sel_g < 0 ) break;
         i = findNearIndex( nn, datax, tool_x );
         if( i >= 0 ) {
           sel_idx = i; tool_x = datax[i]; tool_y = datay[sel_g][i];
         };
         break;
    case Qt::Key_Left:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     asX = 0; 
	     adx = ( plotMaxX - plotMinX ) / (( nxGrid > 0 ) ? nxGrid : 10);
	     plotMinX += adx; plotMaxX += adx;
	     need_rescale = 1;
	   } else {
	     tool_x -= dvx;
	     if( tool_x < plotMinX ) tool_x = plotMinX;
	   };  
	 } else {
	   sel_idx -= di; if( sel_idx < 0 ) sel_idx = 0;
	   tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_Right:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     asX = 0; 
	     adx = ( plotMaxX - plotMinX ) / (( nxGrid > 0 ) ? nxGrid : 10);
	     plotMinX -= adx; plotMaxX -= adx;
	     need_rescale = 1;
	   } else {
	     tool_x += dvx;
	     if( tool_x > plotMaxX ) tool_x = plotMaxX;
	   };  
	 } else {
	   sel_idx += di; if( sel_idx >= nn ) sel_idx = nn - 1;
	   tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_Up:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     asY = 0; 
	     ady = ( plotMaxY - plotMinY ) / (( nyGrid > 0 ) ? nyGrid : 10);
	     plotMinY -= ady; plotMaxY -= ady;
	     need_rescale = 1;
	   } else {
	     tool_y += dvy;
	     if( tool_y > plotMaxY ) tool_y = plotMaxY;
	   };
	 } else {
	   if( btnShift )
	     sel_idx = findGlobalMax( nn, datay[sel_g] );
	   else
	     sel_idx = findNearMax( nn, datay[sel_g], sel_idx );
	   tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_Down:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     asY = 0; 
	     ady = ( plotMaxY - plotMinY ) / (( nyGrid > 0 ) ? nyGrid : 10);
	     plotMinY += ady; plotMaxY += ady;
	     need_rescale = 1;
	   } else {
	     tool_y -= dvy;
	     if( tool_y < plotMinY ) tool_y = plotMinY;
	   };
	 } else {
	   if( btnShift )
	     sel_idx = findGlobalMin( nn, datay[sel_g] );
	   else
	     sel_idx = findNearMin( nn, datay[sel_g], sel_idx );
           tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_PageDown:
         if( ng < 1 ) break;
         sel_g++; 
	 if( sel_g > max_sel_g ) 
	   sel_g = max_sel_g;
         break;
    case Qt::Key_PageUp:
         if( ng < 1 ) break;
         sel_g--; 
	 if( sel_g < min_sel_g ) 
	   sel_g = min_sel_g;
         break;
    case Qt::Key_D: dataInfo(); break;
    case Qt::Key_P: 
	 if( btnCtrl ) 
	   setPrintColors();
	 else
	   printPlot(); 
	 break;
    default: ke->ignore();
  };
  update();
}


void QPlotView::initFakeArrs(void)
{
  static const double fakex[] = { 0, 1, 2, 3 };
  static const double fakey[] = { 0.5, 5.5, 1.3, 0.2 };
  ng = 1; nn = 4; datax = fakex; datay[0] = fakey;
  realMinX = 0; realMaxX = 3; realMinY = 0.2; realMaxY = 5.5;
  calcScale();
}

void QPlotView::initArrs(void)
{
  int i, k, tp, out_nu, out_tp, out_nn, out_ny, start_from;
  double vmin, vmax;
  char outname[MAX_NAMELEN], buf[MAX_NAMELEN];
  TOutArr *arr;
  const double *adr;
  ng = 0; nn = 0; ny = 0;
  sel_g = -1; min_sel_g = -1; max_sel_g = -1;
  if( gra == 0 || root == 0 || model == 0 ) {
    initFakeArrs();
    errstr = "Fail to find data source";
    return;
  };
  if( model->getState() != stateDone ) {
    initFakeArrs();
    errstr = "Data yet not computed";
    return;
  };

  outname[0] = 0;
  gra->getDataSS( "xname", outname, MAX_NAMELEN, 0 );
  if( !isGoodName( outname ) ) { 
    initFakeArrs(); 
    errstr = "Not found X array";
    return; 
  };
  out_nu = model->outname2out_nu( outname );
  if( out_nu < 0 ) { 
    initFakeArrs(); 
    errstr = "Fail to convert X array name to array index";
    return; 
  };
  arr = model->getOutArr( out_nu );
  if( arr == 0 ) { 
    initFakeArrs(); 
    errstr = "Fail to get X array object";
    return; 
  };
  adr = arr->getArray();
  out_tp = -1; out_nn = -1; out_ny = 1;
  arr->getDataSI( "type", &out_tp, 0 ); 
  arr->getDataSI( "n", &out_nn, 0 ); arr->getDataSI( "ny", &out_ny, 0 );
  if( out_ny < 1 ) out_ny = 1;
  ny = out_ny;
  if( adr == 0 || out_tp < 0 || out_nn < 2 ) { 
    initFakeArrs(); 
    errstr = "Bad or missed data in X array";
    return; 
  };
  vmin = 0; vmax = 1; datax = adr;
  arr->getDataSD( "dmin", &vmin, 0 ); arr->getDataSD( "dmax", &vmax, 0 );
  if( vmin >= vmax ) vmax = vmin + 1;
  tp = out_tp; ++tp; --tp; // TODO: fake
  nn = out_nn;
  realMinX = plotMinX = vmin; realMaxX = plotMaxX = vmax;
  buf[0] = 0;
  arr->getDataSS( "label", buf, MAX_NAMELEN, 1 );
  if( buf[0] == 0 ) { buf[1] = 0; buf[0] = 'x'; };
  xLabel = buf;
  for( i=0; i<6; i++ ) {
    start_from = 0;
    if( ny > 1 ) start_from = 1; // skip y in autoscale for 3D-like plot
    strcpy( buf, "y0name" ); buf[1] = char( '0' + i );
    outname[0] = 0;
    gra->getDataSS( buf, outname, MAX_NAMELEN, 0 );
    if( !isGoodName( outname ) ) continue;
    out_nu = model->outname2out_nu( outname );
    if( out_nu < 0 ) continue;
    arr = model->getOutArr( out_nu );
    if( arr == 0 ) continue;
    adr = arr->getArray();
    out_tp = -1; out_nn = -1; out_ny = -1;
    arr->getDataSI( "type", &out_tp, 0 );
    arr->getDataSI( "n", &out_nn, 0 );
    arr->getDataSI( "ny", &out_ny, 0 );
    if( adr == 0 || /*out_tp != tp ||*/ out_nn != nn || out_ny != ny ) 
      continue;
    datay[ng] = adr;
    if( i >= start_from ) {
      k = arr->getDataSD( "dmin", &vmin, 0 );
      k += arr->getDataSD( "dmax", &vmax, 0 );
      if( k == 0 ) {
	if( ng <= start_from ) {
	  realMinY = vmin; realMaxY = vmax;
	} else {
	  if( realMinY > vmin )
	    realMinY = vmin;
	  if( realMaxY < vmax )
	    realMaxY = vmax;
	};
      };
    };
    buf[0] = 0;
    arr->getDataSS( "label", buf, MAX_NAMELEN, 1 );
    if( buf[0] == 0 ) {
      buf[2] = 0; buf[1] = char( '0' + i ); buf[0] = 'y';
    };
    yLabel[ng] = buf;
    ng++;
  };
  tool_x = ref_x = realMinX; tool_y = ref_y = realMinY;
  sel_g = -1; sel_idx = -1;
  if( ny > 1 ) { // 3d
    if( ng > 1 ) { // enough data
      min_sel_g = 1; max_sel_g = ng-1;
      errstr = 0;
    } else {
      errstr = "Not enoth data to 3d plot";
    };
  } else { // 2d
    if( ng > 0 ) { // enough data
      min_sel_g = 0; max_sel_g = ng-1; 
      errstr = 0;
    } else {
      errstr = "Not enoth data to 2d plot";
    };
  };
  sel_g = min_sel_g;
  calcScale();
}

void QPlotView::phys2vis( double x, double y, double *idx, double *idy )
{
  if( idx != 0 ) {
    if( logX ) {
      *idx = pix_x0 + ( log10(x) - plotMinLX ) * kx;
    } else { 
      *idx = pix_x0 + ( x - plotMinX ) * kx;
    };
  };
  if( idy != 0 ) {
    if( logY ) {
      *idy =  pix_y0 - ( log10(y) - plotMinLY ) * ky;
    } else {
      *idy = pix_y0 - ( y - plotMinY ) * ky;
    };
  };
}

void QPlotView::phys2vis( double x, double y, int *ix, int *iy )
{
  double tx, ty;
  phys2vis( x, y, &tx, &ty );
  if( ix != 0 ) {
    *ix = (int) rint( tx ); 
  };
  if( iy != 0 ) {
    *iy = (int) rint( ty ); 
  };
}

void QPlotView::vis2phys( int ix, int iy, double *x, double *y )
{
  if( x != 0 ) {
    if( logX ) {
      *x = pow( 10 , log10(plotMinX) + (( ix - pix_x0 ) / kx) );
    } else {
      *x = plotMinX + ( ix - pix_x0 ) / kx;
    };
  };
  if( y != 0 ) {
    if( logY ) {
      *y = pow( 10, log10(plotMinY) + (( pix_y0 - iy) / ky) );
    } else {
      *y = plotMinY + ( pix_y0 - iy ) / ky;
    };
  };
}


void QPlotView::calcPlp(void)
{
  int i, j, r, nxy, cnc, onc, imcr, st_ng;
  double cr, mcr, xs, ys, xe, ye, xp, yp;
  if( plpbuf == 0 ) { // alloc arrays if need
    plpbuf = new char[ ng * nn + 6 ];
    for( j=0; j<ng; j++ ) {
      plp[j] = plpbuf + j * nn;
    };
  };
  for( i=0; i<ng*nn; i++ ) plpbuf[i] = 0;
  if( ny > 1 ) {
    nxy = ny; st_ng = 1;
  } else {
    nxy = nn; st_ng = 0;
  };
  for( i=0; i<nn; i++ ) {   // find x - extremum points and start/stop
    if( (i % nxy) == 0 ) { // start point
      for( j=0; j<ng; j++ )
	plp[j][i] = 2;
      continue;
    };
    if( (i % nxy) == (nxy-1) ) { // stop point
      for( j=0; j<ng; j++ )
	plp[j][i] = 1;
      continue;
    };
    if( ( datax[i-1] <= datax[i] && datax[i] >= datax[i+1] ) ||  // x-extr
	( datax[i-1] >= datax[i] && datax[i] <= datax[i+1] ) ) {
      if( ! ( datax[i-1] == datax[i] && datax[i] == datax[i+1] ) ) {
	for( j=st_ng; j<ng; j++ )
	  plp[j][i] = 1;
	continue;
      };
    };
    for( j=st_ng; j<ng; j++ ) { // y-extr
      if( ( datay[j][i-1] <= datay[j][i] && datay[j][i] >= datay[j][i+1] ) ||
	  ( datay[j][i-1] >= datay[j][i] && datay[j][i] <= datay[j][i+1] ) ) {
	if( !(datay[j][i-1] == datay[j][i] && datay[j][i] == datay[j][i+1] )) {
	  plp[j][i] = 1;
	  continue;
	}; 
      };
    };
  };
  // find points, worth to plot
  for( j=st_ng; j<ng; j++ ) {
    onc = 1; 
    while( 1 ) {
      cnc = -1; // find first unchecked point
      for( i=onc; i<nn; i++ ) {
	if( plp[j][i] == 0 ) {
	  onc = cnc = i; break;
	};
      };
      if( cnc < 0 ) break; 
      r = -1; // find next setted point
      for( i=cnc+1; i<nn; i++ ) {
	if( plp[j][i] & 3 ) {
	  r = i; break;
	};
      };
      if( r < 0 ) break;

      // calc ranges and find max
      mcr = 0; imcr = -1;
      phys2vis( datax[cnc-1], datay[j][cnc-1], &xs, &ys );
      phys2vis( datax[r], datay[j][r], &xe, &ye );
      for( i=cnc; i<r; i++ ) {
	phys2vis( datax[i], datay[j][i], &xp, &yp );
	cr = perpLen( xs, ys, xe, ye, xp, yp );
	if( cr > mcr ) {
	  mcr = cr; imcr = i;
	};
      };
      // check max
      if( mcr < fabs( maxErr ) ) { 
	for( i=cnc; i<r; i++ )
	  plp[j][i] = 0x80;
      } else {
	plp[j][imcr] = 1;
      };
    }; // end while(1)
    for( i=0; i<nn; i++ )
      plp[j][i] &= 0x7F;
  }; // end for on j(st_ng..ng-1)
}

void QPlotView::calcScale(void)
{
  double dlt, k1, k2, imin, imax, t;
  int ki;
  if( leftMar + rightMar > 0.6 ) {
    leftMar = 0.1; rightMar = 0.02;
  };
  if( topMar + bottomMar > 0.6 ) {
    topMar = bottomMar = 0.05;
  };
  pix_w = width(); pix_h = height();
  pix_l = int( pix_w * leftMar );  pix_r = int( pix_w * rightMar );
  pix_t = int( pix_h * topMar );  pix_b = int( pix_h * bottomMar );
  pix_x = pix_w - pix_l - pix_r; 
  pix_y = pix_h - pix_t - pix_b;
  pix_x0 = pix_l; pix_y0 = pix_t + pix_y;
  need_rescale = 0;

  if( ng < 1 ) return;

  if( asX ) { // autoscale x
     imin = realMinX;  imax = realMaxX;
     if( imin > imax ) { t = imin;  imin = imax;  imax = t; } ;
     if( imin == imax ) { imin -= 0.5;  imax += 0.5; };
     if( zeroX  && ( imin > 0 ) ) imin = 0;
     if( gsX ) { // good scale x
           dlt = imax - imin;  k1 = log10( dlt * 0.98 );
           ki = (int) floor( k1 );  k1 = pow( 10.0, ki );
           k2 = 0.1 * dlt / k1;
           k2 = ( k2 > 0.5 ) ? 1 : ( ( k2 > 0.2 ) ? 0.5 : 0.2 ) ;
           k1 *= k2;
           imin = k1 * floor( imin / k1 );
           imax = imin + k1 * 10;
     };
     if( centerX ) { 
         if( imin + imax > 0 )  imin = -imax;
         else imax = -imin;
     };
     plotMinX = imin;  plotMaxX = imax;
  };
  if( plotMinX <= 0 || realMinX <= 0 )
    logX = 0;
  
  if( asY ) { // autoscale y
     imin = realMinY;  imax = realMaxY;
     if( imin > imax ) { t = imin;  imin = imax;  imax = t; } ;
     if( imin == imax ) { imin -= 0.5;  imax += 0.5; };
     if( zeroY  && ( imin > 0 ) ) imin = 0;
     if( gsY ) { // good scale y
           dlt = imax - imin;  k1 = log10( dlt * 0.98 );
           ki = (int) floor( k1 );  k1 = pow( 10.0, ki );
           k2 = 0.1 * dlt / k1;
           k2 = ( k2 > 0.5 ) ? 1 : ( ( k2 > 0.2 ) ? 0.5 : 0.2 ) ;
           k1 *= k2;
           imin = k1 * floor( imin / k1 );
           imax = imin + k1 * 10;
     };
     if( centerY ) { 
         if( imin + imax > 0 )  imin = -imax;
         else imax = -imin;
     };
     plotMinY = imin;  plotMaxY = imax;
  };
  if( fabs( plotMinX - plotMaxX ) < 1e-200 )
    plotMaxX += 0.1;
  if( fabs( plotMinY - plotMaxY ) < 1e-200 )
    plotMaxY += 0.1;
  if( plotMinY <= 0 || realMinY <= 0 )
    logY = 0;
  if( logX ) {
    plotMinLX = log10( plotMinX );
    kx = pix_x / ( log10( plotMaxX ) - plotMinLX );
  } else { 
    kx = pix_x / ( plotMaxX - plotMinX );
  };
  if( logY ) {
    plotMinLY = log10( plotMinY );
    ky = pix_y / ( log10( plotMaxY ) - plotMinLY );
  } else {
    ky = pix_y / ( plotMaxY - plotMinY );
  };
  calcPlp();
}

// here onfy iface function -- no meat

void QPlotView::setPrintColors(void)
{
  bgColor = Qt::white;
  scaleColor = Qt::black; labelColor = scaleColor;
  gridColor = Qt::black;
  plotColor[0] = Qt::black;
  plotColor[1] = ( ny > 1 ) ? Qt::black : Qt::darkRed;
  plotColor[2] = Qt::darkBlue;
  plotColor[3] = Qt::darkGreen;
  plotColor[4] = Qt::darkGray;
  plotColor[5] = Qt::yellow;
}    

void QPlotView::setStartColors(void)
{
  int i, c;
  char yname[MAX_NAMELEN];
  scaleColor = Qt::white; labelColor = scaleColor;
  gridColor = QColor( 128, 128, 128 );
  xLabel = "x";
  yLabel[0] = "y0"; yLabel[1] = "y1"; yLabel[2] = "y2";
  yLabel[3] = "y3"; yLabel[4] = "y4"; yLabel[5] = "y5";
  if( gra != 0 ) {
    gra->getDataSI( "bgcolor", &c, 1 );
    bgColor = QRgb( c );
    strcpy( yname, "y0color" );
    for( i=0; i<6; i++ ) {
      yname[1] = char( '0' + i );
      gra->getDataSI( yname, &c, MAX_NAMELEN );
      plotColor[i] = QRgb( c );
    };
  } else {
    bgColor = QColor( 0, 0, 32 );
    plotColor[0] = QColor( 255, 255, 255 );
    plotColor[1] = QColor( 255, 255, 0 );
    plotColor[2] = QColor( 255, 0, 0 );
    plotColor[3] = QColor( 0, 255, 255 );
    plotColor[4] = QColor( 0, 255, 0 );
    plotColor[5] = QColor( 0, 0, 0 );
  };
}

void QPlotView::setScale(void)
{
  QDialog *dia;
  QGroupBox *grp_x, *grp_y, *grp_m;
  QLabel *lab;
  QLineEdit *ed_rix, *ed_pix, *ed_rax, *ed_pax, *ed_grix, *ed_tix,
            *ed_riy, *ed_piy, *ed_ray, *ed_pay, *ed_griy, *ed_tiy,
	    *ed_ml,  *ed_mt,  *ed_mr, *ed_mb, *ed_font, *ed_maxErr;
  QCheckBox *cb_asx, *cb_agx, *cb_zerox, *cb_centerx, *cb_logx, *cb_logsx,
            *cb_asy, *cb_agy, *cb_zeroy, *cb_centery, *cb_logy, *cb_logsy;    
  QPushButton *bt_ok, *bt_can; 
  QString qs; int rc;
  calcScale();
  dia = new QDialog( this, "scale_dial", true );
  dia->resize( 450, 450 );
  dia->setCaption( PACKAGE ": Plot scales" );
  // x group
  grp_x = new QGroupBox( dia, "grp_x" );
  grp_x->setGeometry( 20, 10, 200, 250 );
  grp_x->setTitle( "X" );
  lab= new QLabel( dia, "lx1" );
  lab->setGeometry( 30, 40, 30, 20 );  lab->setText( "Min" );
  ed_rix= new QLineEdit( dia, "realMinX" );
  ed_rix->setGeometry( 60, 40, 70, 20 );
  ed_rix->setEnabled( false );  ed_rix->setMaxLength( 12 );
  lab= new QLabel( dia, "lx2" );
  lab->setGeometry( 80, 20, 40, 20 );  lab->setText( "Real" );
  lab= new QLabel( dia, "lx3" );
  lab->setGeometry( 160, 20, 30, 20 );  lab->setText( "Plot" );
  ed_pix= new QLineEdit( dia, "plotMinX" );
  ed_pix->setGeometry( 140, 40, 70, 20 );  ed_pix->setMaxLength( 12 );
  lab= new QLabel( dia, "lx4" );
  lab->setGeometry( 30, 70, 30, 20 );  lab->setText( "Max" );
  ed_rax= new QLineEdit( dia, "realMaxX" );
  ed_rax->setGeometry( 60, 70, 70, 20 );
  ed_rax->setEnabled( false );  ed_rax->setMaxLength( 12 );
  ed_pax= new QLineEdit( dia, "PlotMaxX" );
  ed_pax->setGeometry( 140, 70, 70, 20 );
  cb_asx= new QCheckBox( dia, "asX" );
  cb_asx->setGeometry( 30, 100, 100, 20 );  cb_asx->setText( "AutoScale" );
  cb_agx= new QCheckBox( dia, "gsX" );
  cb_agx->setGeometry( 30, 120, 100, 20 );  cb_agx->setText( "Best Fit" );
  cb_zerox = new QCheckBox( dia, "zeroX" );
  cb_zerox->setGeometry( 30, 140, 100, 20 ); cb_zerox->setText( "Zero" );
  cb_centerx= new QCheckBox( dia, "centerX" );
  cb_centerx->setGeometry( 30, 160, 100, 20 ); cb_centerx->setText( "Center" );
  cb_logx= new QCheckBox( dia, "logX" );
  cb_logx->setGeometry( 30, 180, 100, 20 ); cb_logx->setText( "Logariphmic" );
  cb_logsx= new QCheckBox( dia, "logsX" );
  cb_logsx->setGeometry( 30, 200, 100, 20 ); cb_logsx->setText( "Log. scale" );
  lab= new QLabel( dia, "lx5" );
  lab->setGeometry( 140, 100, 70, 20 );  lab->setText( "Grid/Tick" );
  ed_grix= new QLineEdit( dia, "nxGrid" );
  ed_grix->setGeometry( 120, 120, 40, 20 );  ed_grix->setMaxLength( 3 );
  ed_tix= new QLineEdit( dia, "nxTick" );
  ed_tix->setGeometry( 170, 120, 40, 20 );  ed_tix->setMaxLength( 2 );

  // y group
  grp_y= new QGroupBox( dia, "grp_y" );
  grp_y->setGeometry( 230, 10, 200, 250 );  grp_y->setTitle( "Y" );
  lab= new QLabel( dia, "ly1" );
  lab->setGeometry( 240, 40, 30, 20 );  lab->setText( "Min" );
  ed_riy= new QLineEdit( dia, "realMinY" );
  ed_riy->setGeometry( 270, 40, 70, 20 );
  ed_riy->setEnabled( false );  ed_riy->setMaxLength( 12 );
  lab= new QLabel( dia, "ly2" );
  lab->setGeometry( 290, 20, 40, 20 );  lab->setText( "Real" );
  lab= new QLabel( dia, "ly3" );
  lab->setGeometry( 370, 20, 30, 20 );  lab->setText( "Plot" );
  ed_piy= new QLineEdit( dia, "plotMinY" );
  ed_piy->setGeometry( 350, 40, 70, 20 );  ed_piy->setMaxLength( 12 );
  lab= new QLabel( dia, "ly4" );
  lab->setGeometry( 240, 70, 30, 20 );  lab->setText( "Max" );
  ed_ray= new QLineEdit( dia, "realMaxY" );
  ed_ray->setGeometry( 270, 70, 70, 20 );
  ed_ray->setEnabled( false );  ed_ray->setMaxLength( 12 );
  ed_pay= new QLineEdit( dia, "PlotMaxY" );
  ed_pay->setGeometry( 350, 70, 70, 20 );  ed_pay->setMaxLength( 12 );
  cb_asy= new QCheckBox( dia, "asY" );
  cb_asy->setGeometry( 240, 100, 100, 20 );  cb_asy->setText( "AutoScale" );
  cb_agy= new QCheckBox( dia, "gsY" );
  cb_agy->setGeometry( 240, 120, 100, 20 );  cb_agy->setText( "Best Fit" );
  cb_zeroy = new QCheckBox( dia, "zeroY" );
  cb_zeroy->setGeometry( 240, 140, 100, 20 ); cb_zeroy->setText( "Zero" );
  cb_centery= new QCheckBox( dia, "centerY" );
  cb_centery->setGeometry( 240, 160, 100, 20 ); cb_centery->setText( "Center" );
  cb_logy= new QCheckBox( dia, "logY" );
  cb_logy->setGeometry( 240, 180, 100, 20 ); cb_logy->setText( "Logariphmic" );
  cb_logsy= new QCheckBox( dia, "logsY" );
  cb_logsy->setGeometry( 240, 200, 100, 20 ); cb_logsy->setText( "Log. scale" );
  lab= new QLabel( dia, "ly5" );
  lab->setGeometry( 350, 100, 70, 20 );  lab->setText( "Grid/Tick" );
  ed_griy= new QLineEdit( dia, "nyGrid" );
  ed_griy->setGeometry( 330, 120, 40, 20 ); ed_griy->setMaxLength( 3 );
  ed_tiy= new QLineEdit( dia, "nyTick" );
  ed_tiy->setGeometry( 380, 120, 40, 20 );  ed_tiy->setMaxLength( 2 );
  
  // margins group 
  grp_m= new QGroupBox( dia, "grp_m" );
  grp_m->setGeometry( 20, 270, 410, 50 ); grp_m->setTitle( "Margins %" );
  lab= new QLabel( dia, "lm1" );
  lab->setGeometry( 30, 290, 30, 20 );  lab->setText( "Left" );
  ed_ml= new QLineEdit( dia, "leftMar" );
  ed_ml->setGeometry( 60, 290, 50, 20 );  ed_ml->setMaxLength( 4 );
  lab= new QLabel( dia, "lm2" );
  lab->setGeometry( 120, 290, 40, 20 );  lab->setText( "Top" );
  ed_mt= new QLineEdit( dia, "topMar" );
  ed_mt->setGeometry( 150, 290, 50, 20 );  ed_mt->setMaxLength( 4 );
  lab= new QLabel( dia, "lm3" );
  lab->setGeometry( 220, 290, 40, 20 );  lab->setText( "Right" );
  ed_mr= new QLineEdit( dia, "rightMar" );
  ed_mr->setGeometry( 260, 290, 50, 20 );  ed_mr->setMaxLength( 4 );
  lab= new QLabel( dia, "lm4" );
  lab->setGeometry( 320, 290, 40, 20 );  lab->setText( "Bottom" );
  ed_mb= new QLineEdit( dia, "bottomMar" );
  ed_mb->setGeometry( 360, 290, 50, 20 );  ed_mb->setMaxLength( 4 );
  
  lab= new QLabel( dia, "l_font" );
  lab->setGeometry( 30, 330, 100, 20 ); lab->setText( "Font" );
  ed_font = new QLineEdit( dia, "font_ed" );
  ed_font->setGeometry( 20, 350, 120, 20 ); ed_font->setMaxLength( 30 );
  lab= new QLabel( dia, "l_maxErr" );
  lab->setGeometry( 160, 330, 120, 20 ); lab->setText( "Max plot err" );
  ed_maxErr = new QLineEdit( dia, "maxErr_ed" );
  ed_maxErr->setGeometry( 160, 350, 80, 20 ); ed_maxErr->setMaxLength( 8 );

  bt_ok= new QPushButton( dia, "bt_ok" );
  bt_ok->setGeometry( 90, 410, 100, 30 );
  bt_ok->setText( "&Ok" );
  bt_ok->setDefault( true );
  bt_can= new QPushButton( dia, "bt_can" );
  bt_can->setGeometry( 260, 410, 100, 30 );
  bt_can->setText( "Cancel" );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept() ) );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject() ) );
  ed_rix->setText( QString::number( realMinX ) );
  ed_rax->setText( QString::number( realMaxX ) );
  ed_pix->setText( QString::number( plotMinX ) );
  ed_pax->setText( QString::number( plotMaxX ) );
  ed_grix->setText( QString::number( nxGrid ) );
  ed_tix->setText( QString::number( nxTick ) );
  cb_asx->setChecked( asX );
  cb_agx->setChecked( gsX );
  cb_zerox->setChecked( zeroX );
  cb_centerx->setChecked( centerX );
  cb_logx->setChecked( logX );
  cb_logsx->setChecked( logScaleX );
  ed_riy->setText( QString::number( realMinY ) );
  ed_ray->setText( QString::number( realMaxY ) );
  ed_piy->setText( QString::number( plotMinY ) );
  ed_pay->setText( QString::number( plotMaxY ) );
  ed_griy->setText( QString::number( nyGrid ) );
  ed_tiy->setText( QString::number( nyTick ) );
  cb_asy->setChecked( asY );
  cb_agy->setChecked( gsY );
  cb_zeroy->setChecked( zeroY );
  cb_centery->setChecked( centerY );
  cb_logy->setChecked( logY );
  cb_logsy->setChecked( logScaleY );
  ed_ml->setText( QString::number( leftMar ) );
  ed_mt->setText( QString::number( topMar ) );
  ed_mr->setText( QString::number( rightMar ) );
  ed_mb->setText( QString::number( bottomMar ) );
  ed_font->setText( fontfam );
  ed_maxErr->setText( QString::number( maxErr ) );

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    plotMinX = (ed_pix->text()).toDouble();
    plotMaxX = (ed_pax->text()).toDouble();
    asX = cb_asx->isChecked();
    gsX = cb_agx->isChecked();
    zeroX = cb_zerox->isChecked();
    centerX = cb_centerx->isChecked();
    logX = cb_logx->isChecked();
    logScaleX = cb_logsx->isChecked();
    nxGrid = (ed_grix->text()).toInt();
    nxTick = (ed_tix->text()).toInt();
    plotMinY = (ed_piy->text()).toDouble(); // y
    plotMaxY = (ed_pay->text()).toDouble();
    asY = cb_asy->isChecked();
    gsY = cb_agy->isChecked();
    zeroY = cb_zeroy->isChecked();
    centerY = cb_centery->isChecked();
    logY = cb_logy->isChecked();
    logScaleY = cb_logsy->isChecked();
    nyGrid = (ed_griy->text()).toInt();
    nyTick = (ed_tiy->text()).toInt();
    leftMar = (ed_ml->text()).toDouble(); // margins
    topMar = (ed_mt->text()).toDouble(); 
    rightMar = (ed_mr->text()).toDouble(); 
    bottomMar = (ed_mb->text()).toDouble();
    fontfam = ed_font->text();
    maxErr = (ed_maxErr->text()).toDouble();
  }; 
  delete dia;
  need_rescale = 1;
  update();
}

void QPlotView::setColors(void)
{
  int i, rc;
  QDialog *dia; QLabel *la;
  QPushButton *bt_ok, *bt_can;
  QColorBtn* colbtns[10]; 
  static const char *labels[10] = {
    "Background", "Scale", "Grid", "Labels", 
    "Line0", "Line1", "Line2", "Line3", "Line4", "Line5"
  };
  dia = new QDialog( this, "color_dial", true );  
  dia->resize( 250, 320 );
  dia->setCaption( PACKAGE ": Plot Colors" );
  for( i=0; i<10; i++ ) {
    la = new QLabel( dia, "" );
    la->setGeometry( 100, 10+i*20, 130, 20 );
    la->setText( labels[i] );
    colbtns[i] = new QColorBtn( dia, labels[i] );
    colbtns[i]->setGeometry( 60, 10+i*20, 30, 20 );
    if( i >= 4 ) {
      colbtns[i]->setColor( plotColor[i-4] );
    } else {
      switch( i ) {
	case 0: colbtns[i]->setColor( bgColor ); break;
	case 1: colbtns[i]->setColor( scaleColor ); break;
	case 2: colbtns[i]->setColor( gridColor ); break;
	case 3: colbtns[i]->setColor( labelColor ); break;
      };
    };
  };
  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setGeometry( 10, 280, 100, 30 );
  bt_ok->setText( "&Ok" );
  bt_can = new QPushButton( dia, "bt_can" );
  bt_can->setGeometry( 120, 280, 100, 30 );
  bt_can->setText( "&Cancel" );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject()) );

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    bgColor = colbtns[0]->color();
    scaleColor = colbtns[1]->color();
    gridColor = colbtns[2]->color();
    labelColor = colbtns[3]->color();
    plotColor[0] = colbtns[4]->color();
    plotColor[1] = colbtns[5]->color();
    plotColor[2] = colbtns[6]->color();
    plotColor[3] = colbtns[7]->color();
    plotColor[4] = colbtns[8]->color();
    plotColor[5] = colbtns[9]->color();
  };
  delete dia;
  update();
}

void QPlotView::dataInfo(void)
{
  double toolk, tooldx, tooldy, r, phi, lg_tx, lg_ty, lg_rx, lg_ry, 
         lg_toolk, lg_tooldx, lg_tooldy, lg_r, lg_phi;
  QString qs;
  
  toolk = 0; tooldx = tool_x - ref_x; tooldy = tool_y - ref_y;
  r = euRange( tooldx, tooldy  );
  phi = atan2( tooldy, tooldx );
  qs = QString( "Tool: (" ) + QString::number( tool_x ) + "; " 
     + QString::number( tool_y ) + " ) [" 
     + QString::number( sel_idx ) + "];\n";
  qs += QString( "Ref: (" ) + QString::number( ref_x ) + "; " 
     + QString::number( ref_y ) + " );\n";
  qs += QString( "Tool-Ref: (" ) + QString::number( tooldx ) + "; " 
     + QString::number( tooldy ) + " );\n";
  qs += QString("R = " ) + QString::number( r ) + "; k = ";
  if( tooldx != 0 ) {
    toolk = tooldy / tooldx;
    qs += QString::number( toolk );
  } else {
    qs += "inf";
  };    
  qs += "; Phi = " + QString::number( phi ) + ";\n\n";
  
  qs += "--- Logarithmical: ---\n\n";  
  lg_tx = log10( tool_x ); lg_ty = log10( tool_y );
  lg_rx = log10( ref_x ); lg_ry = log10( ref_y );
  lg_toolk = 0; lg_tooldx = lg_tx - lg_rx; lg_tooldy = lg_ty - lg_ry;
  lg_r = euRange( lg_tooldx, lg_tooldy  );
  lg_phi = atan2( lg_tooldy, lg_tooldx );
  qs += QString( "Tool: (" ) + QString::number( lg_tx ) + "; " 
     + QString::number( lg_ty) + " );\n"; 
  qs += QString( "Ref: (" ) + QString::number( lg_rx ) + "; " 
     + QString::number( lg_ry ) + " );\n";
  qs += QString( "Tool-Ref: (" ) + QString::number( lg_tooldx ) + "; " 
     + QString::number( lg_tooldy ) + " );\n";
  qs += QString("R = " ) + QString::number( lg_r ) + "; k = ";
  if( lg_tooldx != 0 ) {
    lg_toolk = lg_tooldy / lg_tooldx;
    qs += QString::number( lg_toolk );
  } else {
    qs += "inf";
  };    
  qs += "; Phi = " + QString::number( lg_phi ) + ";\n";

  qs += "--- Data info: ---\n\n"; 

  qs += "Points (nn): " + QString::number( nn ) + 
        ";\n Graphs (ng): " + QString::number( ng ) +
        "; Y-dim  (ny): " + QString::number( ny ) + 
        ";\n";
 
  QMessageBox::information( this, "Data info", qs );
}

void QPlotView::moveTool(void)
{
  int rc;
  QDialog *dia; QLabel *la;
  QPushButton *bt_ok, *bt_can;
  QLineEdit *toolx_ed, *tooly_ed;
  QString qs;
  dia = new QDialog( this, "move_dial", true );
  dia->resize( 230, 160 );
  dia->setCaption( "Move tool to point: " );
  la = new QLabel( dia, "l_x" );
  la->setGeometry( 60, 10, 100, 20 );
  la->setText( "X" );
  toolx_ed = new QLineEdit( dia, "toolx_ed" );
  toolx_ed->setGeometry( 50, 30, 120, 20 );
  qs = QString::number( tool_x );
  toolx_ed->setText( qs );
  la = new QLabel( dia, "l_y" );
  la->setGeometry( 60, 60, 100, 20 );
  la->setText( "Y" );
  tooly_ed = new QLineEdit( dia, "tooly_ed" );
  tooly_ed->setGeometry( 50, 80, 120, 20 );
  qs = QString::number( tool_y );
  tooly_ed->setText( qs );
  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setGeometry( 10, 110, 100, 30 );
  bt_ok->setText( "&Ok" );
  bt_can = new QPushButton( dia, "bt_can" );
  bt_can->setGeometry( 120, 110, 100, 30 );
  bt_can->setText( "&Cancel" );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject()) );

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    qs = toolx_ed->text();
    tool_x = qs.toDouble();
    qs = tooly_ed->text();
    tool_y = qs.toDouble();
  };
  delete dia;
}

const char QPlotView::helpstr[] = "<b>Hot keys:</b><br>\n"
 "<b>q</b> - close <br>\n"
 "<b>h/F1</b> - this help <br>\n"
 "<b>a</b> - set quality <br>\n"
 "<b>o/O</b> - on/off non-selected plots <br>\n"
 "<b>s/S</b> - scale dialog / standard scale <br>\n"
 "<b>c/C</b> - colors dialog / initial colors <br>\n"
 "<b>p/Ctrl-P</b> - print / set default colors for print<br>\n"
 "<b>m/M</b> - set mark to tool / zero point <br>\n"
 "<b>g</b> - move tool to given point <br>\n"
 "<b>l/L</b> - link/Unlink to data <br>\n"
 "<b>d</b> - data info <br>\n"
 "<b>PageUp, PageDown</b> - select prev/next plot <br>\n"
 "<b>+/-</b> - Zoom +/- (center = tool) <br>\n"
 "<b> - not in link state: - </b> <br>\n"
 "<b>[Shift]{Left, Top, Right, Bottom}</b> - shift tool on pixel/grid <br>\n"
 "<b>Ctrl {Left, Top, Right, Bottom}</b> - shift plot <br>\n"
 "<b>- in link state: -</b>  <br>\n"
 "<b>[Shift]{Left, Top}</b> - prev/next point [more] <br>\n"
 "<b>[Shift]{Top, Bottom}}</b> - find [global] max/min <br>\n";

void QPlotView::showHelp(void)
{
  QMessageBox::information(this,"Hot keys in plot window", helpstr);
}
 
// end of qplotview.cpp

