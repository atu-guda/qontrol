/***************************************************************************
                          qplotview.cpp  -  description
                             -------------------
    begin                : Sun Aug 20 2000
    copyright            : (C) 2000-2013 by atu
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

// for debug:
// #include <stdio.h>

#include <cmath>
#include <QtGui>

#include "miscfun.h"
#include "dataset.h"
#include "toutarr.h"
#include "tgraph.h"
#include "qcolorbtn.h"
#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"

#include "datawidget.h"
#include "qplotview.h"

using namespace std;

QPlotView::QPlotView( QMo2Doc *adoc, TGraph *agra, QWidget *parent )
          : QWidget( parent )
{
  int i;
  doc = adoc; gra = agra;
  scd = static_cast<ScaleData*>( gra->getObj( "scd", "ScaleData" ) );
  if( !scd ) {
    DBG1( "ScaleData not found, recreatind" );
    scd = new ScaleData( "scd", gra, 0, "scale", "default scale data" ); // ???
  }
  
  datax = 0; plpbuf = 0;
  for( i=0; i<6; i++ ) { datay[i] = 0; plotOn[i] = 1; plp[i] = 0; };
  modelSign = -1; devTp = 0;
  plotMinLX = plotMinLY = 0;
  pix_h = 450; pix_w = 550; 
  leg_sz = 30; 
  sel_g = -1; min_sel_g = -1; max_sel_g = -1;
  sel_idx = -1; qual = 0; linew = 0; need_rescale = 1;
  tool_x = tool_y = ref_x = ref_y = 0;
  ng = 0; nn = 0; ny = 0;
  errstr = 0;
  setStartColors();
  // setBackgroundMode( Qt::NoBackground );
  setFocusPolicy( Qt::StrongFocus );
  setMinimumSize( 350, 280 );
  setCursor( Qt::CrossCursor );
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
  QPainter p( this );
  p.fillRect( 0, 0, width(), height(), QBrush( bgColor ) );
  drawAll( p );
  p.end();
}

void QPlotView::printPlot( void )
{
  QPrinter *pr;
  if( QMo2Win::qmo2win == 0 ) return;
  pr = QMo2Win::qmo2win->getPrinter();
  if( pr == 0 ) return;
  QPrintDialog pr_dialog( pr, this );
  if( pr_dialog.exec() ) {
    devTp = 1;
    pr->setFullPage( FALSE );
    pr->newPage();
    QPainter p( pr ); 
    drawAll( p );
    p.end();
  };
}

void QPlotView::exportPlot()
{
  
  QString fn = QFileDialog::getSaveFileName(this, "Save Picture", "out.png", 
               "PNG files (*.png);;All files (*)" );
  if( fn.isNull() ) {
    return;
  };
  
  QImage timg( pix_w, pix_h, QImage::Format_RGB32 );
  timg.fill( 0xFF );
  QPainter painter( &timg );
  render( &painter );

  if( ! timg.save( fn, "PNG", 50 ) ) {
    QString err = strerror(errno);
    QMessageBox::warning(this, "Fail to open file", 
			 QString("Fail fo open picture file: \"%1\": %2").arg(fn).arg(err), 
	                 QMessageBox::Ok );
  }
  
}

void QPlotView::drawAll( QPainter &p )
{
  int currSgnt;
  pix_h = height(); pix_w = width(); 
  root = doc->getRoot(); model = doc->getModel();
  currSgnt = -2;
  if( model != 0 )
    model->getData( "sgnt", &currSgnt );
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
    const QFont & smlf = QMo2Win::qmo2win->getPlotFont();
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
  if( scd->gridX > 0 ) {
    for( i=0; i<=scd->gridX; i++ ) {  // vertical
      if( scd->logX && scd->logScaleX ) {
	vx = pow10( plotMinLX + i*(log10(scd->plotMaxX)-plotMinLX) / scd->gridX );
	lvx = log10( vx );
      } else {
	vx = scd->plotMinX + ( scd->plotMaxX - scd->plotMinX ) * i / scd->gridX; lvx = vx;
      };
      phys2vis( vx, 0, &ix, 0 );
      p.setPen( QPen( gridColor, 0 /*linew*/) );
      p.drawLine( ix, pix_y0,  ix, pix_y0 - pix_y );
      p.setPen( QPen( labelColor, linew ) );
      p.drawText( ix - 20, pix_y0 + 2, 40, 12, Qt::AlignHCenter,
                QString::number( lvx ) );
    };
  };
  if( scd->gridY > 0 ) {
  for( j=0; j<=scd->gridY; j++ ) {  // horizontal
      if( scd->logY && scd->logScaleY ) {
	vy = pow10( plotMinLY + j*(log10(scd->plotMaxY)-plotMinLY) / scd->gridY );
	lvy = log10( vy );
      } else {
	vy = scd->plotMinY + ( scd->plotMaxY - scd->plotMinY ) * j / scd->gridY; lvy = vy;
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
  if( scd->gridX > 0  &&  scd->tickX > 0  &&  scd->tickX < 10 ) {
    j = scd->gridX * (scd->tickX + 1);
    for( i=0; i<j; i++ ) {
      if( scd->logX && scd->logScaleX ) {
	vx = pow10( plotMinLX + i*(log10(scd->plotMaxX)-plotMinLX) / j );
      } else {
	vx = scd->plotMinX + ( scd->plotMaxX - scd->plotMinX ) * i / j;
      };
      phys2vis( vx, 0, &ix, 0 );
      p.drawLine( ix, pix_y0, ix, pix_y0-3 );
    };
  };
  if( scd->gridY > 0  &&  scd->tickY > 0  &&  scd->tickY < 10 ) {
    j = scd->gridY * (scd->tickY + 1);
    for( i=0; i<j; i++ ) {
      if( scd->logY && scd->logScaleY ) {
	vy = pow10( plotMinLY + i*(log10(scd->plotMaxY)-plotMinLY) / j );
      } else {
	vy = scd->plotMinY + ( scd->plotMaxY - scd->plotMinY ) * i / j;
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
  if( ref_x >= scd->plotMinX && ref_y >= scd->plotMinY && // draw reference point
      ref_x <= scd->plotMaxX && ref_y <= scd->plotMaxY ) {
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
    % QString::number( tool_x, 'g', 4 ) + QString("; " )
    % QString::number( tool_y, 'g', 4 ) + QString(") [" )
    % QString::number( sel_idx ) + QString("]; ref= (" )
    % QString::number( ref_x, 'g', 4 ) + QString("; " ) 
    % QString::number( ref_y, 'g', 4 ) + QString(") dlt = (" ) 
    % QString::number( tooldx, 'g', 4 ) + QString("; " ) 
    % QString::number( tooldy, 'g', 4 ) + QString(") R = " ) 
    % QString::number( hypot( tooldx, tooldy  ), 'g', 4 ) 
    % QString(" ; f = " ) 
    % QString::number( atan2( tooldy, tooldx ), 'g', 4 ) 
    % QString(" ; k = " ) + QString::number( toolk, 'g', 4 );
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
	  if( scd->maxErr > 0 ) {
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
  k = ke->key(); st = ke->modifiers();
  btnShift = (( st & Qt::ShiftModifier ) != 0 );
  btnCtrl = (( st & Qt::ControlModifier ) != 0 );
  if( btnShift ) {
    dvx = ( scd->plotMaxX - scd->plotMinX ) / 20;
    dvy = ( scd->plotMaxY - scd->plotMinY ) / 20;
    di = nn / 50;
  } else {
    dvx = 1 / kx; dvy = 1 / ky; di = 1;
  };
  switch( k ) {
    case Qt::Key_Q: parentWidget()->close(); break;
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
	   scd->autoScX = scd->goodScX = scd->autoScY = scd->goodScY = 1;
	   scd->zeroX = scd->centerX = scd->logX = scd->zeroY = scd->centerY = scd->logY = 0;
	   need_rescale = 1;
	 } else {
	   setScale();
	 };  
	 break;
    case Qt::Key_Plus:
	 scd->autoScX = scd->autoScY = 0;
	 adx = ( scd->plotMaxX - scd->plotMinX ) / 4;
         ady = ( scd->plotMaxY - scd->plotMinY ) / 4;
	 scd->plotMinX = tool_x - adx; 
	 scd->plotMaxX = tool_x + adx;
         scd->plotMinY = tool_y - ady; 
	 scd->plotMaxY = tool_y + ady;
	 need_rescale = 1;
	 break;
    case Qt::Key_Minus:
	 scd->autoScX = scd->autoScY = 0;
	 adx = scd->plotMaxX - scd->plotMinX;
         ady = scd->plotMaxY - scd->plotMinY;
	 scd->plotMinX = tool_x - adx; scd->plotMaxX = tool_x + adx;
         scd->plotMinY = tool_y - ady; scd->plotMaxY = tool_y + ady;
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
	   ref_x = scd->logX; ref_y = scd->logY;
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
	     scd->autoScX = 0; 
	     adx = ( scd->plotMaxX - scd->plotMinX ) / (( scd->gridX > 0 ) ? scd->gridX : 10);
	     scd->plotMinX += adx; scd->plotMaxX += adx;
	     need_rescale = 1;
	   } else {
	     tool_x -= dvx;
	     if( tool_x < scd->plotMinX ) tool_x = scd->plotMinX;
	   };  
	 } else {
	   sel_idx -= di; if( sel_idx < 0 ) sel_idx = 0;
	   tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_Right:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     scd->autoScX = 0; 
	     adx = ( scd->plotMaxX - scd->plotMinX ) / (( scd->gridX > 0 ) ? scd->gridX : 10);
	     scd->plotMinX -= adx; scd->plotMaxX -= adx;
	     need_rescale = 1;
	   } else {
	     tool_x += dvx;
	     if( tool_x > scd->plotMaxX ) tool_x = scd->plotMaxX;
	   };  
	 } else {
	   sel_idx += di; if( sel_idx >= nn ) sel_idx = nn - 1;
	   tool_x = datax[sel_idx]; tool_y = datay[sel_g][sel_idx];
	 };
	 break;
    case Qt::Key_Up:
	 if( sel_idx < 0 ) {
	   if( btnCtrl ) {
	     scd->autoScY = 0; 
	     ady = ( scd->plotMaxY - scd->plotMinY ) / (( scd->gridY > 0 ) ? scd->gridY : 10);
	     scd->plotMinY -= ady; scd->plotMaxY -= ady;
	     need_rescale = 1;
	   } else {
	     tool_y += dvy;
	     if( tool_y > scd->plotMaxY ) tool_y = scd->plotMaxY;
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
	     scd->autoScY = 0; 
	     ady = ( scd->plotMaxY - scd->plotMinY ) / (( scd->gridY > 0 ) ? scd->gridY : 10);
	     scd->plotMinY += ady; scd->plotMaxY += ady;
	     need_rescale = 1;
	   } else {
	     tool_y -= dvy;
	     if( tool_y < scd->plotMinY ) tool_y = scd->plotMinY;
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
    case Qt::Key_E: 
	 exportPlot();
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
  scd->realMinX = 0; scd->realMaxX = 3; scd->realMinY = 0.2; scd->realMaxY = 5.5;
  calcScale();
}

void QPlotView::initArrs(void)
{
  int i, k, tp, out_tp, out_nn, out_ny, start_from;
  double vmin, vmax;
  QString outname, buf;
  TOutArr *arr;
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

  gra->getData( "xname", outname );
  if( !isGoodName( outname ) ) { 
    initFakeArrs(); 
    errstr = "Not found X array";
    return; 
  };
  
  arr = model->getOutArr( outname );
  if( arr == 0 ) { 
    initFakeArrs(); 
    errstr = "Fail to get X array object";
    return; 
  };

  const dvector *adr = arr->getArray();
  out_tp = -1; out_nn = -1; out_ny = 1;
  arr->getData( "type", &out_tp ); 
  arr->getData( "n", &out_nn ); arr->getData( "ny", &out_ny );
  if( out_ny < 1 ) out_ny = 1;
  ny = out_ny;
  if( adr == 0 || out_tp < 0 || out_nn < 2 ) { 
    initFakeArrs(); 
    errstr = "Bad or missed data in X array";
    return; 
  };
  vmin = 0; vmax = 1; 
  datax = adr->data();
  arr->getData( "dmin", &vmin ); 
  arr->getData( "dmax", &vmax );
  if( vmin >= vmax ) vmax = vmin + 1;
  tp = out_tp; ++tp; --tp; // TODO: fake
  nn = out_nn;
  scd->realMinX = scd->plotMinX = vmin; scd->realMaxX = scd->plotMaxX = vmax;
  arr->getData( "label", buf );
  if( buf.size() < 1 ) { buf = "x"; };
  xLabel = buf;
  for( i=0; i<6; i++ ) { // TODO: 6 is number of plots
    start_from = 0;
    if( ny > 1 ) 
      start_from = 1; // skip y in autoscale for 3D-like plot
    buf = "y" + QString::number(i) + "name";
    outname = "";
    gra->getData( buf, outname );
    if( !isGoodName( outname ) ) continue;
    arr = model->getOutArr( outname );
    if( !arr ) 
      continue;
    adr = arr->getArray();
    out_tp = -1; out_nn = -1; out_ny = -1;
    arr->getData( "type", &out_tp );
    arr->getData( "n", &out_nn );
    arr->getData( "ny", &out_ny );
    if( adr == 0 || /*out_tp != tp ||*/ out_nn != nn || out_ny != ny ) 
      continue;
    datay[ng] = adr->data();
    if( i >= start_from ) {
      k = arr->getData( "dmin", &vmin );
      k += arr->getData( "dmax", &vmax );
      if( k == 2 ) {
	if( ng <= start_from ) {
	  scd->realMinY = vmin; scd->realMaxY = vmax;
	} else {
	  if( scd->realMinY > vmin )
	    scd->realMinY = vmin;
	  if( scd->realMaxY < vmax )
	    scd->realMaxY = vmax;
	};
      };
    };
    buf = "";
    arr->getData( "label", buf );
    if( buf.size() < 1 ) {
      buf = "y" + QString::number(i);
    };
    yLabel[ng] = buf;
    ng++;
  };
  tool_x = ref_x = scd->realMinX; tool_y = ref_y = scd->realMinY;
  ref_x -= 1; // disable ref from start
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
    if( scd->logX ) {
      *idx = pix_x0 + ( log10(x) - plotMinLX ) * kx;
    } else { 
      *idx = pix_x0 + ( x - scd->plotMinX ) * kx;
    };
  };
  if( idy != 0 ) {
    if( scd->logY ) {
      *idy =  pix_y0 - ( log10(y) - plotMinLY ) * ky;
    } else {
      *idy = pix_y0 - ( y - scd->plotMinY ) * ky;
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
    if( scd->logX ) {
      *x = pow10( log10(scd->plotMinX) + (( ix - pix_x0 ) / kx) );
    } else {
      *x = scd->plotMinX + ( ix - pix_x0 ) / kx;
    };
  };
  if( y != 0 ) {
    if( scd->logY ) {
      *y = pow10( log10(scd->plotMinY) + (( pix_y0 - iy) / ky) );
    } else {
      *y = scd->plotMinY + ( pix_y0 - iy ) / ky;
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
      if( mcr < fabs( scd->maxErr ) ) { 
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
  if( scd->leftMar + scd->rightMar > 0.6 ) {
    scd->leftMar = 0.1; scd->rightMar = 0.02;
  };
  if( scd->topMar + scd->bottomMar > 0.6 ) {
    scd->topMar = scd->bottomMar = 0.05;
  };
  pix_w = width(); pix_h = height();
  pix_l = int( pix_w * scd->leftMar ); pix_r = int( pix_w * scd->rightMar );
  pix_t = int( pix_h * scd->topMar );  pix_b = int( pix_h * scd->bottomMar );
  pix_x = pix_w - pix_l - pix_r; 
  pix_y = pix_h - pix_t - pix_b;
  pix_x0 = pix_l; pix_y0 = pix_t + pix_y;
  need_rescale = 0;

  if( ng < 1 ) return;

  if( scd->autoScX ) { // autoscale x
     imin = scd->realMinX;  imax = scd->realMaxX;
     if( imin > imax ) { t = imin;  imin = imax;  imax = t; } ;
     if( imin == imax ) { imin -= 0.5;  imax += 0.5; };
     if( scd->zeroX  && ( imin > 0 ) ) imin = 0;
     if( scd->goodScX ) { // good scale x
           dlt = imax - imin;  k1 = log10( dlt * 0.98 );
           ki = (int) floor( k1 );  k1 = pow10( (double)ki );
           k2 = 0.1 * dlt / k1;
           k2 = ( k2 > 0.5 ) ? 1 : ( ( k2 > 0.2 ) ? 0.5 : 0.2 ) ;
           k1 *= k2;
           imin = k1 * floor( imin / k1 );
           imax = imin + k1 * 10;
     };
     if( scd->centerX ) { 
         if( imin + imax > 0 )  imin = -imax;
         else imax = -imin;
     };
     scd->plotMinX = imin;  scd->plotMaxX = imax;
  };
  if( scd->plotMinX <= 0 || scd->realMinX <= 0 )
    scd->logX = 0;
  
  if( scd->autoScY ) { // autoscale y
     imin = scd->realMinY;  imax = scd->realMaxY;
     if( imin > imax ) { t = imin;  imin = imax;  imax = t; } ;
     if( imin == imax ) { imin -= 0.5;  imax += 0.5; };
     if( scd->zeroY  && ( imin > 0 ) ) imin = 0;
     if( scd->goodScY ) { // good scale y
           dlt = imax - imin;  k1 = log10( dlt * 0.98 );
           ki = (int) floor( k1 );  k1 = pow10( (double)ki );
           k2 = 0.1 * dlt / k1;
           k2 = ( k2 > 0.5 ) ? 1 : ( ( k2 > 0.2 ) ? 0.5 : 0.2 ) ;
           k1 *= k2;
           imin = k1 * floor( imin / k1 );
           imax = imin + k1 * 10;
     };
     if( scd->centerY ) { 
         if( imin + imax > 0 )  imin = -imax;
         else imax = -imin;
     };
     scd->plotMinY = imin;  scd->plotMaxY = imax;
  };
  if( fabs( scd->plotMinX - scd->plotMaxX ) < 1e-200 )
    scd->plotMaxX += 0.1;
  if( fabs( scd->plotMinY - scd->plotMaxY ) < 1e-200 )
    scd->plotMaxY += 0.1;
  if( scd->plotMinY <= 0 || scd->realMinY <= 0 )
    scd->logY = 0;
  if( scd->logX ) {
    plotMinLX = log10( scd->plotMinX );
    kx = pix_x / ( log10( scd->plotMaxX ) - plotMinLX );
  } else { 
    kx = pix_x / ( scd->plotMaxX - scd->plotMinX );
  };
  if( scd->logY ) {
    plotMinLY = log10( scd->plotMinY );
    ky = pix_y / ( log10( scd->plotMaxY ) - plotMinLY );
  } else {
    ky = pix_y / ( scd->plotMaxY - scd->plotMinY );
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
  scaleColor = Qt::white; labelColor = scaleColor;
  gridColor = QColor( 128, 128, 128 );
  xLabel = "x";
  yLabel[0] = "y0"; yLabel[1] = "y1"; yLabel[2] = "y2";
  yLabel[3] = "y3"; yLabel[4] = "y4"; yLabel[5] = "y5";
  if( gra != 0 ) {
    gra->getData( "bgcolor", &c );
    bgColor = QRgb( c );
    for( i=0; i<6; i++ ) {
      gra->getData( "y" + QString::number(i) + "color", &c );
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

void QPlotView::setScale()
{
  DataDialog *dia = new DataDialog( *scd, this );

  int rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    need_rescale = 1;
    update();
  }; 
}

void QPlotView::setColors(void)
{
  int i;
  static const char *labels[10] = {
    "Background", "Scale", "Grid", "Labels", 
    "Line0", "Line1", "Line2", "Line3", "Line4", "Line5"
  };
  
  QDialog *dia = new QDialog( this );  
  dia->setWindowTitle( PACKAGE ": Plot Colors" );
  QGridLayout *lay = new QGridLayout( dia );
  QColorBtn* colbtns[10]; 
  QLabel *la;
  for( i=0; i<10; i++ ) {
    la = new QLabel( labels[i], dia );
    lay->addWidget( la, i, 0 );
    colbtns[i] = new QColorBtn( dia );
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
    lay->addWidget( colbtns[i], i, 1 );
  };
  
  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, i, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));

  int rc = dia->exec();
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
  r = hypot( tooldx, tooldy  );
  phi = atan2( tooldy, tooldx );
  qs = QString( "Tool: (" ) % QString::number( tool_x ) % "; " 
     % QString::number( tool_y ) % " ) [" 
     % QString::number( sel_idx ) % "];\n"
     % QString( "Ref: (" ) % QString::number( ref_x ) % "; " 
     % QString::number( ref_y ) % " );\n"
     % QString( "Tool-Ref: (" ) % QString::number( tooldx ) % "; " 
     % QString::number( tooldy ) % " );\n"
     % QString("R = " ) % QString::number( r ) + "; k = ";
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
  lg_r = hypot( lg_tooldx, lg_tooldy  );
  lg_phi = atan2( lg_tooldy, lg_tooldx );
  QString qs1 = QString( "Tool: (" ) % QString::number( lg_tx ) % "; " 
     % QString::number( lg_ty) % " );\n"
     % QString( "Ref: (" ) % QString::number( lg_rx ) % "; " 
     % QString::number( lg_ry ) % " );\n"
     % QString( "Tool-Ref: (" ) % QString::number( lg_tooldx ) % "; " 
     % QString::number( lg_tooldy ) % " );\n"
     % QString("R = " ) % QString::number( lg_r ) % "; k = ";
  if( lg_tooldx != 0 ) {
    lg_toolk = lg_tooldy / lg_tooldx;
    qs1 += QString::number( lg_toolk );
  } else {
    qs1 += "inf";
  };    
  qs1 += "; Phi = " + QString::number( lg_phi ) + ";\n";

  qs1 += "--- Data info: ---\n\n"; 

  qs1 += "Points (nn): " + QString::number( nn ) + 
        ";\n Graphs (ng): " + QString::number( ng ) +
        "; Y-dim  (ny): " + QString::number( ny ) + 
        ";\n";
 
  QMessageBox::information( this, "Data info", qs + qs1);
}

void QPlotView::moveTool(void)
{
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( "Move tool to point: " );
  QGridLayout *lay = new QGridLayout( dia );
  
  QLineEdit *toolx_ed = new QLineEdit( dia );
  toolx_ed->setValidator( new QDoubleValidator() );
  toolx_ed->setText( QString::number( tool_x ) );
  lay->addWidget( toolx_ed, 0, 1 );
  QLineEdit *tooly_ed = new QLineEdit( dia );
  tooly_ed->setValidator( new QDoubleValidator() );
  tooly_ed->setText( QString::number( tool_y ) );
  lay->addWidget( tooly_ed, 1, 1 );
  
  QLabel *la_x = new QLabel( "the &X value", dia );
  la_x->setBuddy( toolx_ed );
  lay->addWidget( la_x, 0, 0 );
  QLabel *la_y = new QLabel( "the &Y value", dia );
  la_y->setBuddy( tooly_ed );
  lay->addWidget( la_y, 1, 0 );

  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));

  int rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    tool_x = toolx_ed->text().toDouble();
    tool_y = tooly_ed->text().toDouble();
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
 "<b>e</b> - export image to png <br>\n"
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

