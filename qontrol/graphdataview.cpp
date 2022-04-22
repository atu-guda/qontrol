/***************************************************************************
                          graphdataview.cpp - view for plots
                             -------------------
    begin                : 2014.11.15
    copyright            : (C) 2014-2022 by atu
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

#include "dataset.h"
#include "graphdataview.h"
#include "labowin.h"
#include "mglview.h"
#include "miscfun.h"
#include "doubledialog.h"
#include "doubletable.h"


GraphDataView::GraphDataView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdListView( a_mod, a_par )
{
  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 96,252,192 ) );
  setPalette( s_pal );
}

void GraphDataView::init_actions()
{
  auto a = new QAction( QIcon( QSL(":icons/showgraph.png") ), QSL("&Show"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showObj()) );

  a = new QAction(  QIcon::fromTheme(QSL("document-export")), QSL("Export"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(exportObj()) );

  a = new QAction( QIcon( QSL(":icons/showgraphdata.png") ), QSL("show Data"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showDataObj()) );

}

bool GraphDataView::showObj()
{
  TGraph *gra = qobject_cast<TGraph*>( getSelObj() );
  if( !gra ) {
    return false;
  }

  LaboWin *mwin = MAINWIN;
  if( !mwin ) { return false;  }

  LaboDoc *doc = par->getDocument();
  if( !doc ) { return false;  }

  QString fileName = doc->pathName();
  // like CommonSubwin
  QString wtit = QSL( "plot: " ) % gra->objectName() % QSL(" - " ) % fileName;
  QMdiSubWindow *swin = mwin->findMdiByTitle( wtit, true ); // true = activate
  if( swin ) {
    return false;
  }

  auto plotWnd = new MglSubwin( mwin, doc, gra );
  mwin->addChild( plotWnd );
  return true;
}

bool GraphDataView::showDataObj()
{
  TGraph *gra = qobject_cast<TGraph*>( getSelObj() );
  if( !gra ) {
    return false;
  }

  DatasInfo di;
  int k = gra->fillDatasInfo( &di );
  if( !k ) {
    return false;
  }

  auto dia = new DoubleDialog( di, QES, this );

  dia->exec();
  delete dia;

  return true;
}

bool GraphDataView::exportObj()
{
  TGraph *gra = qobject_cast<TGraph*>( getSelObj() );
  if( !gra ) {
    return false;
  }

  QString fnq = QFileDialog::getSaveFileName( this,
      QSL("Export data from \"") % gra->objectName() % QSL("\""),
      gra->hintFileName() % data_file_suff, data_files_sel );
  if( fnq.isEmpty() ) {
    return false;
  }

  gra->dump( fnq, QSL(" ") );
  return true;
}

