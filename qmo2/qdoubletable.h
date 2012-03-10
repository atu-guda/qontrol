/***************************************************************************
                          qdoubletable.h  -  description
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

#ifndef QDOUBLETABLE_H
#define QDOUBLETABLE_H

#include <qwidget.h>
#include <q3table.h>
class QPainer; struct GraphInfo;

/**draws a table filled by double values
  *@author atu
  */

class QDoubleTable : public Q3Table {
  Q_OBJECT
 public:
  QDoubleTable( const GraphInfo *agi, QWidget *parent, const char *name );
  ~QDoubleTable();
 protected:
  virtual void paintCell( QPainter *p, int ro, int co,
            const QRect &cr, bool selected );
  virtual QWidget* createEditor ( int row, int col, bool
                 initFromCell ) const;
 protected:
  const GraphInfo *gi;
  int nr, nc, cw[64], ch;
  const double **data;
  const char **nms;
};

#endif

