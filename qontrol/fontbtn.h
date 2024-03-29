#pragma once
/***************************************************************************
  fontbtn.h - widget to edit font by button
                             -------------------
    begin                : 2015.11.19
    copyright            : (C) 2015-2022 by atu
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


#include <QWidget>
#include <QPushButton>
#include <QFont>

/**allow select font by pressing button
  *@author atu
  */

class FontBtn : public QPushButton  {
 Q_OBJECT
 public:
   explicit FontBtn( QWidget *parent );
   virtual ~FontBtn();
   const QFont& font() const { return cfont; }
   void set_Font( const QFont &c );
   void set_Font( const QString &s );
 Q_SIGNALS:
   void changed( const QFont &newFont );
 protected Q_SLOTS:
   void slotClicked();
 protected:
   virtual void paintEvent( QPaintEvent *ev );
 protected:
   QFont cfont;
};


