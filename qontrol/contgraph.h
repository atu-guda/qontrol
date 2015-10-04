/***************************************************************************
                          contgraph.h  -  contrainer for TGraph
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2015 by atu
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

#ifndef _CONTGRAPH_H
#define _CONTGRAPH_H

#include "dataset.h"

class ContGraph : public TDataSet {
   Q_OBJECT
  public:
   DCL_CTOR(ContGraph);
   virtual ~ContGraph() override;
   DCL_CREATE;
   DCL_STD_INF;
  private:
   DCL_DEFAULT_STATIC;
};

#endif

