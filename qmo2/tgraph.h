/***************************************************************************
                          tgraph.h  -  description
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

#ifndef TGRAPH_H
#define TGRAPH_H

#include <QColor>
#include <QString>
#include "dataset.h"
#include "scaledata.h"
class GraphInfo;


/**Description of graph
  *@author atu
  */

class TGraph : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(TGraph);
   DCL_CREATE;
   DCL_STD_INF;
   /** dumps data to file */
   int  dump( const char *fn, char delim = ' ' );
   /** dumps data to file to be used by gnuplot */
   int gnuPlot( int otp, const char *fn, const char *title,
                const char *eps_fn, const char *dat_fn );
   /** fills fields in GraphInfo structure */
   int fillGraphInfo( GraphInfo *gi ) const;
 protected:
   /** title of graph  */
   PRM_STRING( title, efNRC, "Title", "Plot title", "max=128\nsep=blockend\ncol=-1\ndef=fig. " );
   /** name of output array for x  */
   PRM_STRING( xname, efNRC, "X  name", "Name of source for X values", "max=64\nsep=block" );
   /** names of output array for y[i]  */
   PRM_STRING( y0name, efNRC, "Y0 name", "Name of source for Y0 values", "max=64" );
   PRM_STRING( y1name, efNRC, "Y1 name", "Name of source for Y1 values", "max=64" );
   PRM_STRING( y2name, efNRC, "Y2 name", "Name of source for Y2 values", "max=64" );
   PRM_STRING( y3name, efNRC, "Y3 name", "Name of source for Y3 values", "max=64" );
   PRM_STRING( y4name, efNRC, "Y4 name", "Name of source for Y4 values", "max=64" );
   PRM_STRING( y5name, efNRC, "Y5 name", "Name of source for Y5 values", "max=64" );
   /** back color  */
   PRM_COLOR( bgcolor, efNRC, "BG color", "Background color", "sep=col\ndef=#000060");
   /** color of lines */
   PRM_COLOR( y0color, efNRC, "Y0 color", "Color for Y0", "def=white");
   PRM_COLOR( y1color, efNRC, "Y0 color", "Color for Y1", "def=yellow");
   PRM_COLOR( y2color, efNRC, "Y0 color", "Color for Y2", "def=#ff0000");
   PRM_COLOR( y3color, efNRC, "Y0 color", "Color for Y3", "def=#00ff00");
   PRM_COLOR( y4color, efNRC, "Y0 color", "Color for Y4", "def=#0000ff");
   PRM_COLOR( y5color, efNRC, "Y0 color", "Color for Y5", "def=#ff00ff");
   // TODO: legend
   ScaleData *scd;
   // PRM_OBJ( scd, 0, "Scale data", "Information about scales", "sep=block" );

   DCL_DEFAULT_STATIC;
};
typedef TGraph *PTGraph;

#endif

