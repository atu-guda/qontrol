/***************************************************************************
                          tinputany.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TINPUTANY_H
#define TINPUTANY_H

#include <tmiso.h>


/**gets data from different source by name
  *@author atu
  */

class TInputAny : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TInputAny);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to set source */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** name of source */
   PRM_STRING( name, efNoRunChange, "Name",
       "Name of element (and may be parameter) to get values", "max=80");
   /** fake source */
   double fake_so;
   /** pointer to source */
   const double *so;

   DCL_DEFAULT_STATIC;
};

#endif


