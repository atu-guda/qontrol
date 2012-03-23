/***************************************************************************
                          qcolorbtn.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2012 by atu
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

#include <QColorDialog>
#include <QPainter>
#include "qcolorbtn.h"

QColorBtn::QColorBtn( QWidget *parent )
          : QPushButton( parent )
{
  connect( this, SIGNAL( clicked() ), SLOT( slotClicked() ) );
  col = Qt::black;
}

QColorBtn::~QColorBtn()
{
}

int QColorBtn::colorInt(void) const
{
  return int( col.rgb() );
}

void QColorBtn::setColor( const QColor &c )
{
  col = c;
  repaint();
}

void QColorBtn::setColor( int ic )
{
  col = QRgb( ic );
  repaint();
}

void QColorBtn::slotClicked()
{
  QColor ncol;
  ncol = QColorDialog::getColor( col );
  if( ncol.isValid() )
    col = ncol;
  repaint();
  emit changed( col );
}

void QColorBtn::paintEvent( QPaintEvent * ev )
{
  QPushButton::paintEvent( ev );
  int bor_w = 2; // border width
  // QColor lnCol = Qt::black;
  QColor fillCol = isEnabled() ? col : Qt::gray;
  QPainter p( this );
  p.fillRect( bor_w*2, bor_w*2, width() - 4*bor_w, height() - 5*bor_w, fillCol );
  p.end();
}

// end of qcolorbtn.cpp
 
