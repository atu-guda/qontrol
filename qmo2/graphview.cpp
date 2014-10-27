/***************************************************************************
                          graphview.cpp  -  description
                             -------------------
    begin                : Sat Aug 19 2000
    copyright            : (C) 2000-2014 by atu
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
#include <QtWidgets>

#include "miscfun.h"
#include "labodoc.h"
#include "labowin.h"
#include "laboview.h"
#include "graphview.h"

GraphView::GraphView( LaboDoc *adoc, LaboView *mview, QWidget *parent )
           : QWidget(parent)
{
  doc = adoc; mainview = mview;
  model = 0;
  grid_sz = 15;
  setFixedWidth( 25 );
  setFocusPolicy( Qt::NoFocus );
  QPalette pal;
  pal.setBrush( QPalette::Window, QBrush( QColor( 0,128,96 ) ) );
  setPalette( pal );
  setAutoFillBackground(true);
}

GraphView::~GraphView()
{
}

void GraphView::paintEvent( QPaintEvent * /*pe*/ )
{
  int h, w, nh, n_graph, graph_nu;
  TGraph *gra;
  QString out_name, yname;
  QPainter p( this );

  const QFont &smlf = LaboWin::labowin->getSmallFont();
  p.setFont( smlf );
  QFontMetrics fm { smlf };
  ex_sz = fm.width("W") + 4;
  grid_sz = ex_sz + 6;
  fwidth = grid_sz * 3;
  setFixedWidth( fwidth + 8 );

  p.setPen( Qt::black );

  if( ! doc )
    return;
  model = doc->getModel();
  if( ! model ) 
    return;

  h = height();
  nh = 1 + h / grid_sz;
  nh += h; // FAKE TODO:
  w = width(); ++w; --w;

  n_graph = model->getNGraph();
  p.setBrush( Qt::yellow );
  yname = "y0name";
  for( graph_nu=0; graph_nu < n_graph ; graph_nu++ ) {
    gra = model->getGraph( graph_nu );
    if( ! gra )
      continue;
    p.setPen( Qt::black );
    p.drawRect( 14, 10 + graph_nu * grid_sz, fwidth-10, ex_sz );
    p.drawText( grid_sz+1, grid_sz + graph_nu*grid_sz,
                QSN( graph_nu ) + ":" + gra->objectName() );
  }; // end loop on graphs
}

void GraphView::mousePressEvent( QMouseEvent *me )
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
  if( x < 12 || x > fwidth ) return;
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
         connect( act, &QAction::triggered, mainview, &LaboView::newGraph );
         if( graph_nu >=0 ) {
           act = menu->addAction( "&Edit" );
           connect( act, &QAction::triggered, mainview, &LaboView::editGraph );
           act = menu->addAction( "&Delete" );
           connect( act, &QAction::triggered, mainview, &LaboView::delGraph );
           menu->addSeparator();
           act = menu->addAction( "&Show" );
           connect( act, &QAction::triggered, mainview, &LaboView::showGraph );
           act = menu->addAction( "Show D&ata" );
           connect( act, &QAction::triggered, mainview, &LaboView::showGraphData );
           act = menu->addAction( "D&ump" );
           connect( act, &QAction::triggered, mainview, &LaboView::exportGraphData );
           act = menu->addAction( "Gnu&plot" );
           connect( act, &QAction::triggered, mainview, &LaboView::gnuplotGraph );
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


// end of graphview.cpp

