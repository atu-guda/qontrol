/***************************************************************************
                          qgraphview.cpp  -  description
                             -------------------
    begin                : Sat Aug 19 2000
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

#include "miscfun.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qmo2view.h"
#include "qgraphview.h"

QGraphView::QGraphView( QMo2Doc *adoc, QMo2View *mview, QWidget *parent )
           : QWidget(parent)
{
  doc = adoc; mainview = mview;
  model = 0;
  grid_sz = 15;
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
  QString out_name, yname;
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
  p.setBrush( Qt::yellow ); 
  yname = "y0name";
  for( graph_nu=0; graph_nu < n_graph ; graph_nu++ ) {
    gra = model->getGraph( graph_nu );
    if( gra == 0 ) continue;
    p.setPen( Qt::black );
    p.drawRect( 14, 10 + graph_nu * grid_sz, 10, 10 );
    p.drawText( grid_sz+1, 18 + graph_nu*grid_sz, QString::number( graph_nu ) );
    if( level != graph_nu ) continue;
    gra->getData( "xname", out_name );
    out_nu = model->outname2out_nu( out_name );
    if( out_nu >= 0 ) {
      p.drawLine( 0, grid_sz + out_nu*grid_sz, 12, grid_sz + graph_nu*grid_sz );
    };
    p.setPen( Qt::yellow );
    for( i=0; i<6; i++ ) {
      yname[1] = char( '0' + i );
      gra->getData( yname, out_name );
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
  QMenu *menu;
  TGraph *gra;
  QString graname( "?bad?" );
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
      graname = gra->objectName();
      if( graname.isEmpty() )
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
         menu = new QMenu( this ); 
         QAction *act;
         if( graph_nu >=0 ) {
	   (void) menu->addAction( graname ); // fake: title
           menu->addSeparator();
         };
	 act = menu->addAction( "&New" );
	 connect( act, SIGNAL( activated() ), mainview, SLOT(newGraph() ) );
         if( graph_nu >=0 ) {
	   act = menu->addAction( "&Edit" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(editGraph() ) );
	   act = menu->addAction( "&Delete" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(delGraph() ) );
           menu->addSeparator();
	   act = menu->addAction( "&Show" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(showGraph() ) );
	   act = menu->addAction( "Show D&ata" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(showGraphData() ) );
	   act = menu->addAction( "D&ump" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(exportGraphData() ) );
	   act = menu->addAction( "Gnu&plot" );
	   connect( act, SIGNAL( activated() ), mainview, SLOT(gnuplotGraph() ) );
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

