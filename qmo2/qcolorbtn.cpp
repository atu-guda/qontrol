/***************************************************************************
                          qcolorbtn.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000 by atu
    email                : atu@dmeti.dp.ua
    based on KDE KColorButton
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcolordialog.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include "qcolorbtn.h"

QColorBtn::QColorBtn( QWidget *parent, const char *name )
          : QPushButton( parent, name )
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
  repaint( false );
}

void QColorBtn::setColor( int ic )
{
  col = QRgb( ic );
  repaint( false );
}

void QColorBtn::slotClicked()
{
  QColor ncol;
  ncol = QColorDialog::getColor( col );
  if( ncol.isValid() )
    col = ncol;
  repaint( false );
  emit changed( col );
}

void QColorBtn::drawButtonLabel( QPainter *p )
{
  int w = 11;
  QColor lnCol = colorGroup().text();
  QColor fillCol = isEnabled() ? col : backgroundColor();
  qDrawPlainRect( p, w/2, w/2, width()-w, height()-w, lnCol, 1, 0 );
  w += 2;
  p->fillRect( w/2, w/2, width() - w, height() - w, fillCol );
}

// end of qcolorbtn.cpp
 
