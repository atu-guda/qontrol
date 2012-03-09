/***************************************************************************
                          qoutview.cpp  -  description
                             -------------------
    begin                : Fri Aug 18 2000
    copyright            : (C) 2000 by atu
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

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpen.h>
#include <qbrush.h>
#include <qfont.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qkeycode.h>

#include "resource.h"
#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"

#include "qoutview.h"

QOutView::QOutView(  QMo2Doc *adoc,  QMo2View *mview, 
                     QWidget *parent, const char *name )
         : QWidget( parent, name )
{
  doc = adoc; mainview = mview;
  model = 0; grid_sz = 15; 
  setBackgroundColor( QColor( 0, 0, 128) );
  setFixedWidth( 15 );
  setFocusPolicy( NoFocus );
}

QOutView::~QOutView()
{
}

void QOutView::paintEvent( QPaintEvent * /*pe*/ )
{
  int h, w, nh, n_out, out_nu, out_type, elnu, out_st, level;
  TOutArr *arr;
  char elmname[MAX_NAMELEN];
  if( doc == 0 ) return;
  model = doc->getModel();
  if( model == 0 ) return;
  QPainter p( this );
  QFont smlf;
  smlf.fromString( QMo2Win::qmo2win->getSettings()->smallFont );
  p.setFont( smlf );
  p.setPen( Qt::black );
  h = height(); w = width(); nh = 1 + h / grid_sz; 
  n_out = model->getNOutArr();
  level = mainview->getLevel();
  for( out_nu=0; out_nu < n_out && out_nu < nh; out_nu++ ) {
    arr = model->getOutArr( out_nu );
    if( arr == 0 ) continue;
    elmname[0] = 0;
    arr->getDataSS( "name", elmname, MAX_NAMELEN, 0 );
    elnu = model->oname2elnu( elmname );
    out_type = -1;
    arr->getDataSI( "type", &out_type, 1 );
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
    p.drawRect( 3, 10 + out_nu*grid_sz, 10, 10 );
    p.drawText( 5, 18 + out_nu*grid_sz, QString::number( out_nu ) );
    out_st = arr->getState();
    if( out_st > 1 ) {
      p.setBrush( NoBrush ); p.setPen( Qt::yellow );
      p.drawRect( 2, 9 + out_nu*grid_sz, 12, 12 );
    };
  }; // end loop on outs
  if( level >= 0 ) {
    p.setPen( Qt::yellow );
    p.drawLine( 0, grid_sz + level*grid_sz, 4, grid_sz + level*grid_sz );
  };
}

void QOutView::mousePressEvent( QMouseEvent *me )
{
  int h, w, nh, out_nu, n_out, x, y, nn, old_level;
  QPopupMenu *menu;
  TOutArr *arr;
  const char *outname = "?bad?";
  char elmname[MAX_NAMELEN];
  QString title;
  if( model == 0 ) return;
  h = height(); w = width(); nh = h / grid_sz - 1; 
  x = me->x(); y = me->y();
  if( x < 2 || x > 12 ) return;
  out_nu = ( y - 10 ) / grid_sz;
  n_out = model->getNOutArr();
  if( out_nu < 0 || out_nu >= n_out ) return;
  elmname[0] = 0; title = "??bad??";
  arr = model->getOutArr( out_nu );
  if( arr != 0 ) {
    outname = arr->getName();
    arr->getDataSS( "name", elmname, MAX_NAMELEN, 0 );
    if( outname == 0 || outname[0] == 0 )
      outname = "?unknown?";
    nn = -1;
    arr->getDataSI( "n", &nn, 1 );
    title = QString(outname)
            + QString( "[" ) +QString::number(nn) + QString("]")
            + QString(" -> " ) + QString(elmname);
  };
  old_level = mainview->getLevel();
  mainview->changeLevel( out_nu );
  switch( me->button() ) {
    case LeftButton: 
         mainview->editOut();
         break;
    case RightButton: 
         menu = new QPopupMenu( this, "out_rbmenu" ); 
         menu->insertItem( title, 0 );
         menu->insertSeparator();
         menu->insertItem( "&New", mainview, SLOT(newElm()), Key_Insert );
         menu->insertItem( "&Edit", mainview, SLOT(editOut()), 0 );
         menu->insertItem( "&Delete", mainview, SLOT(delOut()), 0 );
         menu->insertSeparator();
         menu->insertItem( "D&ump", mainview, SLOT(exportOut()), 0 );
         menu->insertItem( "&Show data", mainview, SLOT(showOutData()), 0 );
         menu->exec( mapToGlobal(QPoint( x, y )) );
         delete menu;
         break;
    case MidButton:
	 old_level = out_nu;
         break;
    default: break;// none
  };
  mainview->changeLevel( old_level );
}

// end of qoutview.cpp


