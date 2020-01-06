/***************************************************************************
                          statusmodel.cpp  -  description
                             -------------------
    begin                : Sat Aug 17 2002
    copyright            : (C) 2000-2020 by atu
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
#include "labodoc.h"

StatusModel::StatusModel( CommonSubwin *a_up_view )
           : QStatusBar( a_up_view ), up_view( a_up_view )
{
  const int label_lw = 1;
  const int label_sep = 6;
  // const int label_fs = QFrame::Box | QFrame::Raised;
  const int label_fs = QFrame::Panel | QFrame::Sunken;
  int mw;

  // labels creation
  l_mod = new QLabel( this );
  QFontMetrics fm = l_mod->fontMetrics();
  mw = fm.horizontalAdvance( QSL("9") );
  l_mod->setFixedWidth( mw + label_sep );
  l_mod->setLineWidth( label_lw ); l_mod->setFrameStyle( label_fs );
  addWidget( l_mod );

  l_level = new QLabel( this );
  l_level->setFixedWidth( mw + label_sep );
  l_level->setLineWidth( label_lw ); l_level->setFrameStyle( label_fs );
  addWidget( l_level );

  l_stat = new QLabel( this );
  l_stat->setFixedWidth( 4 * mw + label_sep );
  l_stat->setLineWidth( label_lw ); l_stat->setFrameStyle( label_fs );
  addWidget( l_stat );

  l_name = new QLabel( this );
  l_name->setLineWidth( label_lw ); l_name->setFrameStyle( label_fs );
  addWidget( l_name, 1 );

  l_desc = new QLabel( this );
  l_desc->setLineWidth( label_lw ); l_desc->setFrameStyle( label_fs );
  addWidget( l_desc, 2 );

  l_val = new QLabel( this );
  l_val->setMinimumSize( QSize( (DOUBLE_PREC+6) * mw + label_sep, fm.height() ) );
  l_val->setLineWidth( label_lw ); l_val->setFrameStyle( label_fs );
  l_val->setTextInteractionFlags( Qt::TextSelectableByMouse );
  addWidget( l_val );

  setFixedHeight( fm.height() + 12 );
}


void StatusModel::update()
{
  l_level->setText( QSN( up_view->getLevel() ) );
  l_name->setText( QSL(".") );  l_desc->setText( QSL(".") );  l_val->setText( QES );

  LaboDoc *doc = up_view->getDocument();
  if( !doc ) {
    l_stat->setText( QSL("NO Doc!") );
    return;
  }
  TModel *model = doc->getModel();
  if( !model ) {
    l_stat->setText( QSL("NO model!") );
    return;
  }

  int state = model->getState();
  l_stat->setText( getStateString( state ) );
  QString s1;
  if(  model->getModified() ) { s1 += QSL("*"); };
  l_mod->setText( s1 );

  HolderData *ob = up_view->getSelectedInFocus();
  if( ob ) {
    QString ob_nm_tp = ob->dataObj( 0, Qt::StatusTipRole ).toString(); // TODO: make better
    if( ob_nm_tp.size() < 3 ) {
      ob_nm_tp = ob->objectName() % QSL(" ") % ob->getType();
    }
    l_name->setText( ob_nm_tp );
    QString ob_descr = ob->getDataD( QSL("descr"), QString() );
    l_desc->setText( ob_descr );
    l_val->setText( ob->textVisual() );
  };

  QStatusBar::update();
}


// end of statusmodel.cpp

