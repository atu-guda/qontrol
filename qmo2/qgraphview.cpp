/***************************************************************************
                          qgraphview.cpp  -  description
                             -------------------
    begin                : Sat Aug 19 2000
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
#include <q3mainwindow.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpen.h>
#include <qbrush.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qmessagebox.h>
#include <q3popupmenu.h>
#include <qnamespace.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

#include "resource.h"
#include "miscfun.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qmo2view.h"
#include "qgraphview.h"

QGraphView::QGraphView( QMo2Doc *adoc, QMo2View *mview, QWidget *parent,
                        const char *name )
           : QWidget(parent,name)
{
  doc = adoc; mainview = mview;
  model = 0;
  grid_sz = 15;
  setBackgroundColor( QColor( 0, 128, 96 ) );
  setFixedWidth( 25 );
  setFocusPolicy( Qt::NoFocus );
}

QGraphView::~QGraphView()
{
}

void QGraphView::paintEvent( QPaintEvent * /*pe*/ )
{
  int h, w, nh, n_graph, graph_nu, out_nu, i, level;
  TGraph *gra;
  char out_name[MAX_NAMELEN], yname[12];
  QPainter p( this );
  QFont smlf;
  smlf.fromString( QMo2Win::qmo2win->getSettings()->smallFont );
  p.setFont( smlf );
  p.setPen( Qt::black );
  if( doc == 0 ) 
    return;
  model = doc->getModel();
  if( model == 0 ) return;
  level = mainview->getLevel();
  h = height(); 
  nh = 1 + h / grid_sz;
  nh += h; // FAKE TODO:
  w = width(); ++w; --w;
  p.setBrush( QColor( 0,128,96 ) );
  p.drawRect( 0, 0, w, h );
  
  n_graph = model->getNGraph();
  p.setBrush( Qt::yellow ); strcpy( yname, "y0name" );
  for( graph_nu=0; graph_nu < n_graph ; graph_nu++ ) {
    gra = model->getGraph( graph_nu );
    if( gra == 0 ) continue;
    p.setPen( Qt::black );
    p.drawRect( 14, 10 + graph_nu * grid_sz, 10, 10 );
    p.drawText( grid_sz+1, 18 + graph_nu*grid_sz, QString::number( graph_nu ) );
    if( level != graph_nu ) continue;
    out_name[0] = 0;
    gra->getDataSS( "xname", out_name, MAX_NAMELEN, 0 );
    out_nu = model->outname2out_nu( out_name );
    if( out_nu >= 0 ) {
      p.drawLine( 0, grid_sz + out_nu*grid_sz, 12, grid_sz + graph_nu*grid_sz );
    };
    p.setPen( Qt::yellow );
    for( i=0; i<6; i++ ) {
      out_name[0] = 0; yname[1] = char( '0' + i );
      gra->getDataSS( yname, out_name, MAX_NAMELEN, 0 );
      out_nu = model->outname2out_nu( out_name );
      if( out_nu >= 0 ) {
	p.drawLine( 0, grid_sz + out_nu*grid_sz, 12, grid_sz + graph_nu*grid_sz );
      };
    };
  }; // end loop on graphs
}

void QGraphView::mousePressEvent( QMouseEvent *me )
{
  int h, w, nh, graph_nu, n_graph, x, y, old_level;
  Q3PopupMenu *menu;
  TGraph *gra;
  const char *graname = "?bad?";
  if( model == 0 ) return;
  h = height(); 
  w = width(); 
  nh = h / grid_sz - 1;
  w+=nh; w-=nh; // TODO: fake
  x = me->x(); y = me->y();
  if( x < 12 || x > 24 ) return;
  graph_nu = ( y - 10 ) / grid_sz;
  n_graph = model->getNGraph();
  old_level = mainview->getLevel();
  if( graph_nu < 0 || graph_nu >= n_graph ) {
    graph_nu = -1;
  } else {
    mainview->changeLevel( graph_nu );
    gra = model->getGraph( graph_nu );
    if( gra != 0 ) {
      graname = gra->getName();
      if( graname == 0 || graname[0] == 0 )
        graname = "?unknown?";
    };
  };
  switch( me->button() ) {
    case Qt::LeftButton:
         if( graph_nu >= 0 ) {
           mainview->editGraph();
         };
         break;
    case Qt::RightButton:
         menu = new Q3PopupMenu( this, "graph_rbmenu" ); 
         if( graph_nu >=0 ) {
           menu->insertItem( graname, 0 );
           menu->insertSeparator();
         };
         menu->insertItem( "&New", mainview, SLOT(newGraph()), 0 );
         if( graph_nu >=0 ) {
           menu->insertItem( "&Edit", mainview, SLOT(editGraph()), 0 );
           menu->insertItem( "&Delete", mainview, SLOT(delGraph()), 0 );
           menu->insertSeparator();
           menu->insertItem( "&Show", mainview, SLOT(showGraph()), 0 );
           menu->insertItem( "Show D&ata", mainview, SLOT(showGraphData()), 0 );
           menu->insertItem( "D&ump", mainview, SLOT(exportGraphData()), 0 );
           menu->insertItem( "Gnu&plot", mainview, SLOT(gnuplotGraph()), 0 );
         };
         menu->exec( mapToGlobal(QPoint( x, y )) );
         delete menu; 
         break;
    case Qt::MidButton:
         if( graph_nu >=0 ) {
           mainview->showGraph();
         };
         break;
    default: break;// none
  };
  mainview->changeLevel( old_level );
}


// end of qgraphview.cpp

