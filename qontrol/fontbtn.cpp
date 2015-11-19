/***************************************************************************
  fontbtn.cpp - widget to edit font by button
                             -------------------
    begin                : 2015.11.19
    copyright            : (C) 2015-2015 by atu
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

#include <QFontDialog>
#include <QPainter>
#include "fontbtn.h"
#include "defs.h"

FontBtn::FontBtn( QWidget *parent )
          : QPushButton( parent ), cfont( QFont() )
{
  connect( this, &FontBtn::clicked, this, &FontBtn::slotClicked );
}

FontBtn::~FontBtn()
{
}

void FontBtn::set_Font( const QFont &f )
{
  cfont = f;
  repaint();
}

void FontBtn::set_Font( const QString &s )
{
  cfont.fromString( s );
  repaint();
}

void FontBtn::slotClicked()
{
  QFont nfont;
  bool ok = false;
  nfont =  QFontDialog::getFont( &ok, cfont, this, QSL("Select font"),
              QFontDialog::DontUseNativeDialog );
  cfont = nfont;
  repaint();
  emit changed( cfont );
}

void FontBtn::paintEvent( QPaintEvent * ev )
{
  setFont( cfont );
  QString s = cfont.key();
  s.truncate( 16 );
  setText( s );
  QPushButton::paintEvent( ev );
}

// end of fontbtn.cpp

