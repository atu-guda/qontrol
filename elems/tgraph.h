/***************************************************************************
                          tgraph.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#ifndef TGRAPH_H
#define TGRAPH_H

#include "dataset.h"

#define CLASS_ID_TGraph 700

/**Description of graph
  *@author atu
  */

class TGraph : public TDataSet  {
 public:
    /** constructor */ 
   explicit TGraph( TDataSet *apar );
   /** destructor */
   virtual ~TGraph();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** returns help string */
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
   /** title of graph ++title_s80: */
   char title[80];
   /** name of output array for x ++xname_s32 */
   char xname[MAX_NAMELEN];
   /** names of output array for y0 ++yname{0-5}_s32 */
   char yname[6][MAX_NAMELEN];
   /** back color ++bgcolor_ic */
   int bgcolor;
   /** color of lines ++colors{0..5}_ic */
   int ycolor[6];
   // TODO: legend
 
   /** data descriptors */ 
   static TDataInfo tgraph_d_i[28]; 
   /** class decription */
   static TClassInfo class_info;       
   /** help str */
   static const char* helpstr;
};
typedef TGraph *PTGraph;

#endif
