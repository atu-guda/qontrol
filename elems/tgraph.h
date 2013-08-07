/***************************************************************************
                          tgraph.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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
    /** constructor */ 
   explicit TGraph( TDataSet *apar );
   /** destructor */
   virtual ~TGraph();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const char* getHelp(void) const;
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** dumps data to file */
   int  dump( const char *fn, char delim = ' ' );
   /** dumps data to file to be used by gnuplot */
   int gnuPlot( int otp, const char *fn, const char *title,
                const char *eps_fn, const char *dat_fn );
   /** fills fields in GraphInfo structure */
   int fillGraphInfo( GraphInfo *gi ) const;
 protected:
   /** title of graph  */
   PRM_STRING1( title, efNRC, "Title", "Plot title", "max=128\nsep=blockend\ncol=-1" );
   /** name of output array for x  */
   PRM_STRING1( xname, efNRC, "X  name", "Name of source for X values", "max=64\nsep=block" );
   /** names of output array for y[i]  */
   PRM_STRING1( y0name, efNRC, "Y0 name", "Name of source for Y0 values", "max=64" );
   PRM_STRING1( y1name, efNRC, "Y1 name", "Name of source for Y1 values", "max=64" );
   PRM_STRING1( y2name, efNRC, "Y2 name", "Name of source for Y2 values", "max=64" );
   PRM_STRING1( y3name, efNRC, "Y3 name", "Name of source for Y3 values", "max=64" );
   PRM_STRING1( y4name, efNRC, "Y4 name", "Name of source for Y4 values", "max=64" );
   PRM_STRING1( y5name, efNRC, "Y5 name", "Name of source for Y5 values", "max=64" );
   /** back color  */
   PRM_COLOR1( bgcolor, efNRC, "BG color", "Background color", "sep=col");
   /** color of lines */
   PRM_COLOR1( y0color, efNRC, "Y0 color", "Color for Y0", "");
   PRM_COLOR1( y1color, efNRC, "Y0 color", "Color for Y1", "");
   PRM_COLOR1( y2color, efNRC, "Y0 color", "Color for Y2", "");
   PRM_COLOR1( y3color, efNRC, "Y0 color", "Color for Y3", "");
   PRM_COLOR1( y4color, efNRC, "Y0 color", "Color for Y4", "");
   PRM_COLOR1( y5color, efNRC, "Y0 color", "Color for Y5", "");
   // TODO: legend
   ScaleData *scd;
   PRM_OBJ1( scd, 0, "Scale data", "Information about scales", "sep=block" );
 
   /** class decription */
   static TClassInfo class_info;       
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};
typedef TGraph *PTGraph;

#endif

