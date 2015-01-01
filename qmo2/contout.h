/***************************************************************************
                          contout.h  -  contrainer for TOutArr
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

#ifndef _CONTOUT_H
#define _CONTOUT_H

#include "datacont.h"

class TOutArr;

class ContOut : public TDataContainer {
   Q_OBJECT
  public:
   DCL_CTOR(ContOut);
   virtual ~ContOut() override;
   DCL_CREATE;
   DCL_STD_INF;

   //* called before all runs
   int preRun( int run_tp, int an, int anx, int any, double adt );
   //* will be called after all actions
   int postRun( int good );
   //* called before each inner param loop
   int startLoop( int acnx, int acny );
   /** will be called after each inner loop */
   int endLoop( int acnx, int acny );

   //* get all needed ordinary (outSimple) values
   void takeAllVals();
  private:
   std::vector<TOutArr*> vo;
   DCL_DEFAULT_STATIC;
};

#endif

