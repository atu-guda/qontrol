/***************************************************************************
                          colorbtn.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#include <QColorDialog>
#include <QPainter>
#include "colorbtn.h"
#include "defs.h"

ColorBtn::ColorBtn( QWidget *parent )
          : QPushButton( parent )
{
  connect( this, &ColorBtn::clicked, this, &ColorBtn::slotClicked );
  col = Qt::black;
}

ColorBtn::~ColorBtn()
{
}

int ColorBtn::colorInt(void) const
{
  return int( col.rgb() );
}

void ColorBtn::setColor( const QColor &c )
{
  col = c;
  repaint();
}

void ColorBtn::setColor( int ic )
{
  col = QRgb( ic );
  repaint();
}

void ColorBtn::slotClicked()
{
  QColor ncol;
  // DBGx( "dbg: Click: \"%s\"", qP( col.name() ) );
  ncol = QColorDialog::getColor( col, this, "Choose color", QColorDialog::ShowAlphaChannel );
  if( ncol.isValid() ) {
    col = ncol;
  }
  repaint();
  emit changed( col );
}

void ColorBtn::paintEvent( QPaintEvent * ev )
{
  QPushButton::paintEvent( ev );
  int bor_w = 2; // border width
  QColor fillCol = isEnabled() ? col : Qt::gray;
  QPainter p( this );
  p.fillRect( bor_w*2, bor_w*2, width() - 4*bor_w, height() - 5*bor_w, fillCol );
  p.end();
}

// end of colorbtn.cpp

