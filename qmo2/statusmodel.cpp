/***************************************************************************
                          statusmodel.cpp  -  description
                             -------------------
    begin                : Sat Aug 17 2002
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
#include "statusmodel.h"

StatusModel::StatusModel( LaboView *mview, QWidget *parent )
           : QStatusBar( parent )
{
  mainview = mview;
  const int label_lw = 1;
  const int label_sep = 6;
  // const int label_fs = QFrame::Box | QFrame::Raised;
  const int label_fs = QFrame::Panel | QFrame::Sunken;
  int mw;

  // labels creation
  l_mod = new QLabel( "m", this );
  QFontMetrics fm = l_mod->fontMetrics();
  mw = fm.width("9");
  l_mod->setFixedWidth( mw + label_sep );
  l_mod->setLineWidth( label_lw ); l_mod->setFrameStyle( label_fs );
  addWidget( l_mod );

  l_level = new QLabel( "N", this );
  l_level->setFixedWidth( mw + label_sep );
  l_level->setLineWidth( label_lw ); l_level->setFrameStyle( label_fs );
  addWidget( l_level );

  l_stat = new QLabel( "s", this );
  l_stat->setFixedWidth( 4 * mw + label_sep );
  l_stat->setLineWidth( label_lw ); l_stat->setFrameStyle( label_fs );
  addWidget( l_stat );

  l_nums = new QLabel( "n/n", this );
  l_nums->setFixedWidth( 10 * mw + label_sep );
  l_nums->setLineWidth( label_lw ); l_nums->setFrameStyle( label_fs );
  addWidget( l_nums );

  l_name = new QLabel( "name", this );
  l_name->setLineWidth( label_lw ); l_name->setFrameStyle( label_fs );
  addWidget( l_name, 1 );

  l_desc = new QLabel( "desk", this );
  l_desc->setLineWidth( label_lw ); l_desc->setFrameStyle( label_fs );
  addWidget( l_desc, 2 );

  l_val = new QLabel( "val", this );
  l_val->setFixedWidth( 16 * mw + label_sep );
  l_val->setLineWidth( label_lw ); l_val->setFrameStyle( label_fs );
  addWidget( l_val );

  setFixedHeight( fm.height() + 12 );
}

StatusModel::~StatusModel()
{
}

void StatusModel::update()
{
  static const char* modChar[] = { " ", "+", "#", "*", "?", "." };
  QString ob_descr, ob_nm_tp;
  int mod, stat = 0;
  TModel *model;
  TMiso *ob;
  double val;
  QString s_nums("");
  l_level->setText( QSN( mainview->getLevel() ) );
  l_name->setText( "." );  l_desc->setText( "." );  l_val->setText( "" );
  model = mainview->getModel();
  if( !model ) {
    l_mod->setText( "X" );
    l_stat->setText( "Error" );
    return;
  }

  s_nums.sprintf( "[%d;%d] ",  mainview->getSelX(), mainview->getSelY() );
  l_nums->setText( s_nums );

  stat = model->getState();
  l_stat->setText( getStateString( stat ) );
  mod = model->getModified();
  l_mod->setText( modChar[mod] );
  ob = mainview->getSelObj();
  if( ob != 0 ) {
    ob_nm_tp = ob->objectName()
      % "  (" % ob->getType() % ")";
    l_name->setText( ob_nm_tp );
    ob->getData( "descr", ob_descr );
    l_desc->setText( ob_descr );
    if( stat > 1 ) {
      ob->getData( "out0", &val );
      l_val->setText( QSN( val, 'g', 12 ) );
    };
  };

  QStatusBar::update();
}


// end of statusmodel.cpp

