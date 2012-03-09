/***************************************************************************
                          qcolorbtn.h  -  description
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

#ifndef QCOLORBTN_H
#define QCOLORBTN_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qcolor.h>

/**allow select color by pressing button
  *@author atu
  */

class QColorBtn : public QPushButton  {
 Q_OBJECT
 public:
   explicit QColorBtn( QWidget *parent, const char *name = 0 );
   virtual ~QColorBtn();
   const QColor color(void) const { return col; };
   int colorInt(void) const;
   void setColor( const QColor &c );
   void setColor( int ic );
 signals: 
   void changed( const QColor &newColor );
 protected slots:
   /** called if clicked */
   void slotClicked();
 protected:
   /** paint color on top of button */
   virtual void drawButtonLabel( QPainter *p );   
 protected:
   QColor col;
};

#endif

