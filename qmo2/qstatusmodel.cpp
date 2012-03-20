/***************************************************************************
                          qstatusmodel.cpp  -  description
                             -------------------
    begin                : Sat Aug 17 2002 
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
#include <QFont>
#include <QLabel>

#include "resource.h"
#include "miscfun.h"
#include "qstatusmodel.h"

QStatusModel::QStatusModel( QMo2View *mview, QWidget *parent,
                        const char *name )
           : QStatusBar( parent, name )
{
  mainview = mview;
  const int label_lw = 1;
  const int label_sep = 8;
  // const int label_fs = QFrame::Box | QFrame::Raised;
  const int label_fs = QFrame::Panel | QFrame::Sunken;
  int mw;
  
  // labels creation
  l_mod = new QLabel( "m", this, "l_mod" );
  QFontMetrics fm = l_mod->fontMetrics();
  mw = fm.width("9");
  l_mod->setFixedWidth( mw + label_sep );
  l_mod->setLineWidth( label_lw ); l_mod->setFrameStyle( label_fs );
  addWidget( l_mod );
  
  l_level = new QLabel( "N", this, "l_level" );
  l_level->setFixedWidth( mw + label_sep );
  l_level->setLineWidth( label_lw ); l_level->setFrameStyle( label_fs );
  addWidget( l_level );
  
  l_stat = new QLabel( "s", this, "l_stat" );
  l_stat->setFixedWidth( 4 * mw + label_sep );
  l_stat->setLineWidth( label_lw ); l_stat->setFrameStyle( label_fs );
  addWidget( l_stat );
  
  l_nums = new QLabel( "n/n", this, "l_nums" );
  l_nums->setFixedWidth( 10 * mw + label_sep );
  l_nums->setLineWidth( label_lw ); l_nums->setFrameStyle( label_fs );
  addWidget( l_nums );
  
  l_name = new QLabel( "name", this, "l_name" );
  l_name->setLineWidth( label_lw ); l_name->setFrameStyle( label_fs );
  addWidget( l_name, 1 );
  
  l_desc = new QLabel( "desk", this, "l_desc" );
  l_desc->setLineWidth( label_lw ); l_desc->setFrameStyle( label_fs );
  addWidget( l_desc, 2 );
  
  l_val = new QLabel( "val", this, "l_val" );
  l_val->setFixedWidth( 16 * mw + label_sep );
  l_val->setLineWidth( label_lw ); l_val->setFrameStyle( label_fs );
  addWidget( l_val );
  
  setFixedHeight( fm.height() + 16 );
}

QStatusModel::~QStatusModel()
{
}

void QStatusModel::update()
{
  static const char* modChar[] = { " ", "+", "#", "*", "?", "." };
  char ob_descr[128];
  int mod, stat, sel;
  TModel *model;
  TMiso *ob;
  double val;
  QString s_nums("");
  l_level->setText( QString::number( mainview->getLevel() ) );
  l_name->setText( "." );  l_desc->setText( "." );  l_val->setText( "" );
  model = mainview->getModel();
  if( model != 0 ) {
    sel = mainview->getSel();
    ob = model->getMiso( sel );
    s_nums.sprintf( "%d[%d;%d] %d", sel, 
	            mainview->getSelX(), mainview->getSelY(), 
	           model->getNMiso() );
    l_nums->setText( s_nums );
    stat = model->getState();
    l_stat->setText( getStateString( stat ) );
    mod = model->getModified();
    l_mod->setText( modChar[mod] );
    if( ob != 0 ) {
      l_name->setText( ob->getName() );
      ob_descr[0] = 0;
      ob->getDataSS( "descr", ob_descr, 120, 0 );
      l_desc->setText( QString::fromLocal8Bit( ob_descr ) );
      if( stat > 1 ) {
        val = model->xout( sel );
        l_val->setText( QString::number( val, 'g', 12 ) );
      };
    };
  } else { // w/o model -- error case
    l_mod->setText( "X" );
    l_stat->setText( "Error" );
  };
  QStatusBar::update();
}


// end of qstatusmodel.cpp

