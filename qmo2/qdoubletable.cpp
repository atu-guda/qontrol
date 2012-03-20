/***************************************************************************
                          qdoubletable.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

#include <QString>
#include <QPainter>
#include <Q3Table>
#include "miscfun.h"
#include "qdoubletable.h"

QDoubleTable::QDoubleTable( const GraphInfo *agi, 
                            QWidget *parent, const char *name )
             :Q3Table( agi->row, agi->col, parent, name )
{
  int i;
  gi = agi;
  setFrameStyle( QFrame::Panel | QFrame::Sunken );
  // setLineWidth( 2 );
  setMinimumSize( 190, 120 );
  // setCellWidth( 80 ); setCellHeight( 20 );
  // setTableFlags( Tbl_autoScrollBars );
  Q3Header *gh = horizontalHeader();
  if( gh != 0 ) {
    for( i=0; i<gi->col; i++ ) {
      gh->setLabel( i, QString::fromLocal8Bit( gi->label[i] ) );
    };
  };
  // updateTableSize();
}

QDoubleTable::~QDoubleTable()
{
  // we don't own any data
}

void QDoubleTable::paintCell( QPainter *p, int ro, int co, 
            const QRect &cr, bool selected  )
{
  QString qs;
  double v;
  if( selected ) {
    p->setBrush( Qt::blue ); p->setPen( Qt::white );
  } else {
    p->setBrush( Qt::white ); p->setPen( Qt::black );
  };
  p->drawRect( 0, 0, cr.width(), cr.height() );
  v = gi->dat[co][ro];
  qs.setNum( v, 'g', 8 );
  p->drawText( 2, 12, qs, -1 );
}


QWidget* QDoubleTable::createEditor ( int /*row*/, int /*col*/,
         bool  /* initFromCell */ ) const
{
  return 0;
}

