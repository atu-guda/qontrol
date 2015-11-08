/***************************************************************************
                          statusmodel.cpp  -  description
                             -------------------
    begin                : Sat Aug 17 2002
    copyright            : (C) 2000-2015 by atu
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
  l_val->setFixedWidth( (DOUBLE_PREC+6) * mw + label_sep );
  l_val->setLineWidth( label_lw ); l_val->setFrameStyle( label_fs );
  l_val->setTextInteractionFlags( Qt::TextSelectableByMouse );
  addWidget( l_val );

  setFixedHeight( fm.height() + 12 );
}

StatusModel::~StatusModel()
{
}

void StatusModel::update()
{
  l_level->setText( QSN( up_view->getLevel() ) );
  l_name->setText( "." );  l_desc->setText( "." );  l_val->setText( "" );

  // QString s_nums = QSL("[") % QSN(up_view->getSelX()) % QSL(";")
  //     % QSN( up_view->getSelY() ) % QSL("]");
  // l_nums->setText( s_nums );

  LaboDoc *doc = up_view->getDocument();
  if( !doc ) {
    l_stat->setText( QSL("NO Doc!") );
    return;
  }
  TRootData *root = doc->getRoot();
  if( !root ) {
    l_stat->setText( QSL("NO root!") );
    return;
  }
  TModel *model = root->getObjT<TModel*>( "model" );
  if( !model ) {
    l_stat->setText( QSL("NO model!") );
    return;
  }

  int state = model->getState();
  l_stat->setText( getStateString( state ) );
  int mod = model->getModified();
  l_mod->setText( modificationChar[mod] );

  HolderData *ob = up_view->getSelectedInFocus();
  if( ob ) {
    QString ob_nm_tp = ob->dataObj( 0, Qt::StatusTipRole ).toString();
    l_name->setText( ob_nm_tp );
    QString ob_descr = ob->getDataD( "descr", QString() );
    l_desc->setText( ob_descr );
    if( state > stateGood ) {
      TMiso *el = qobject_cast<TMiso*>( ob );
      if( el ) {
        double val = el->getDataD( "out0", 0.0 ); // TODO: StatusTipRole(col=2)
        l_val->setText( QSN( val, 'g', DOUBLE_PREC ) );
      }
    };
  };

  QStatusBar::update();
}


// end of statusmodel.cpp

