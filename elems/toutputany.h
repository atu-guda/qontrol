/***************************************************************************
                          toutputany.h  -  description
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

#ifndef TOUTPUTANY_H
#define TOUTPUTANY_H

#include <tmiso.h>

/**gets data from different source by name
  *@author atu
  */

class TOutputAny : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TOutputAny);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to set source */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;

   /** name of source */
   PRM_STRING( name, efNRC, "Sink name", "Name of sink to output", "ncol=2\nmax=80" );
   /** enabler = u[1]  */
   PRM_SWITCH( useEnable, 0,  "u[1] is Enable", "Use u[1] as enable", "");
   /** type of output: -1:bad, 0:object, 1-vars.  */
   int type;
   /** idx of sink */
   int ne;
   /** pointer to element  */
   TDataSet *pel;
   /** name of last subelement xx.xx.xxx.lastname */
   QString lastname;
   
   DCL_DEFAULT_STATIC;
};

#endif


