/***************************************************************************
                          qoutview.cpp  -  description
                             -------------------
    begin                : Fri Aug 18 2000
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

#include <QtGui>

#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"

#include "qoutview.h"

QOutView::QOutView(  QMo2Doc *adoc,  QMo2View *mview, 
                     QWidget *parent )
         : QWidget( parent )
{
  doc = adoc; mainview = mview;
  model = 0; grid_sz = 15; 
  QPalette palette;
  palette.setColor(backgroundRole(), QColor(0,0,128) );
  setPalette(palette);
  setFixedWidth( 17 );
  setFocusPolicy( Qt::NoFocus );
}

QOutView::~QOutView()
{
}

void QOutView::paintEvent( QPaintEvent * /*pe*/ )
{
  int h, w, nh, n_out, out_nu, out_type, elnu, out_st, level;
  TOutArr *arr;
  QString elmname;
  if( doc == 0 ) return;
  model = doc->getModel();
  if( model == 0 ) return;
  QPainter p( this );
  QFont smlf;
  smlf.fromString( QMo2Win::qmo2win->getSettings()->smallFont ); // TODO: global
  p.setFont( smlf );
  QFontMetrics fm { smlf };
  ex_sz = fm.width("W") + 4;
  grid_sz = ex_sz + 6;
  fwidth = grid_sz * 4;// good/min number of letters in name
  setFixedWidth( fwidth + 8 ); 

  p.setPen( Qt::black );
  p.setBrush( QColor( 80,128,255 ) );
  h = height(); w = width(); 
  nh = 1 + h / grid_sz; 
  p.drawRect( 0, 0, w, h );

  n_out = model->getNOutArr();
  level = mainview->getLevel();
  
  for( out_nu=0; out_nu < n_out && out_nu < nh; out_nu++ ) {
    arr = model->getOutArr( out_nu );
    if( arr == 0 ) 
      continue;
    arr->getData( "name", elmname );
    elnu = model->oname2elnu( elmname );
    out_type = -1;
    arr->getData( "type", &out_type );
    switch( out_type ) {
      case 0: p.setBrush( Qt::white ); break;
      case 1: p.setBrush( Qt::green ); break;
      case 2: p.setBrush( Qt::cyan ); break;
      case 3: p.setBrush( Qt::gray ); break;
      default: p.setBrush( Qt::red ); break;
    };
    if( elnu >= 0 || out_type == 3) {
      p.setPen( Qt::black );
    } else {
      if( elnu < -9  )
        p.setPen( Qt::magenta );
      else
        p.setPen( Qt::red );
    };

    // label
    p.drawRect( 3, 10 + out_nu*grid_sz, fwidth, ex_sz );
    p.drawText( 5, grid_sz + out_nu*grid_sz, 
        QString::number( out_nu ) + ": " + elmname );
    out_st = arr->getState();

    if( out_st > 1 ) {
      p.setBrush( Qt::NoBrush ); p.setPen( Qt::yellow );
      p.drawRect( 2, 9 + out_nu*grid_sz, fwidth+2, ex_sz+2 );
    };
  }; // end loop on outs
  if( level >= 0 ) {
    p.setPen( Qt::yellow );
    p.drawLine( 0, grid_sz + level*grid_sz, 4, grid_sz + level*grid_sz );
  };
}

void QOutView::mousePressEvent( QMouseEvent *me )
{
  int h, nh, out_nu, n_out, x, y, nn, old_level;
  QMenu *menu;
  TOutArr *arr;
  QString outname( "?bad?" );
  QString elmname;
  QString title;
  if( model == 0 ) return;
  h = height(); nh = h / grid_sz - 1; ++nh; --nh; // TODO: FAKE
  x = me->x(); y = me->y();
  if( x < 2 || x > fwidth ) return;
  out_nu = ( y - grid_sz/2 ) / grid_sz; // TODO: why 10?
  n_out = model->getNOutArr();
  if( out_nu < 0 || out_nu >= n_out ) return;
  title = "??bad??";
  arr = model->getOutArr( out_nu );
  if( arr != 0 ) {
    outname = arr->objectName();
    arr->getData( "name", elmname );
    if( outname.isEmpty() )
      outname = "?unknown?";
    nn = -1;
    arr->getData( "n", &nn );
    title = outname
            % QString( "[" ) % QString::number(nn) % QString("]")
            % QString(" -> " ) % elmname;
  };
  old_level = mainview->getLevel();
  mainview->changeLevel( out_nu );
  QAction *act;
  switch( me->button() ) {
    case Qt::LeftButton: 
         mainview->editOut();
         break;
    case Qt::RightButton: 
         menu = new QMenu( this ); 
	 (void) menu->addAction( title ); // fake action: title
         menu->addSeparator();
	 act = menu->addAction( "&New" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(newOut() ) );
	 act = menu->addAction( "&Edit" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(editOut() ) );
	 act = menu->addAction( "&Delete" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(delOut() ) );
         menu->addSeparator();
	 act = menu->addAction( "D&ump" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(exportOut() ) );
	 act = menu->addAction( "&Show data" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(showOutData() ) );
         menu->exec( mapToGlobal(QPoint( x, y )) );
         delete menu;
         break;
    case Qt::MidButton:
	 old_level = out_nu;
         break;
    default: break;// none
  };
  mainview->changeLevel( old_level );
}

// end of qoutview.cpp


