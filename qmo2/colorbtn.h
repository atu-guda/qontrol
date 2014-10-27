/***************************************************************************
                          colorbtn.h  -  description
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

#ifndef QCOLORBTN_H
#define QCOLORBTN_H

#include <QWidget>
#include <QPushButton>
#include <QColor>

/**allow select color by pressing button
  *@author atu
  */

class ColorBtn : public QPushButton  {
 Q_OBJECT
 public:
   explicit ColorBtn( QWidget *parent );
   virtual ~ColorBtn();
   const QColor color(void) const { return col; }
   int colorInt(void) const;
   void setColor( const QColor &c );
   void setColor( int ic );
 signals:
   void changed( const QColor &newColor );
 protected slots:
   /** called if clicked */
   void slotClicked();
 protected:
   virtual void paintEvent( QPaintEvent *ev );
 protected:
   QColor col;
};

#endif

