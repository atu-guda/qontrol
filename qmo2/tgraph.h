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

//* decription of one line in plot

class GraphElem : public TDataSet {
   Q_OBJECT
 public:
   DCL_CTOR(GraphElem);
   DCL_CREATE;
   DCL_STD_INF;

   enum DataType {
     DataNone = 0, // just to ignore data
     DataAxisX, DataAxisY, DataAxisZ, // coords (Z unused for now)
     DataPlot,  DataStep, DataTape,
     DataStem, DataBars, DataBarh, DataTens, DataArea,
     DataRegion, DataOHLC, DataBoxPlot, DataCandle, DataCones,
     DataError, DataMark, DataTube,
     DataSurf, DataSurfC, DataSurfA,
     DataMesh, DataFall, DataBelt, DataDens,
     DataCont, DataContF, DataContD,
     DataC0, DataC1, DataC2, DataC3, DataC4, DataC5, // aux data for misc plots
   };
   Q_ENUMS( DataType );
   Q_CLASSINFO( "enum_DataType_0",   "None   " );     // DataNone
   Q_CLASSINFO( "enum_DataType_1",   "AxisX  " );     // DataAxisX
   Q_CLASSINFO( "enum_DataType_2",   "AxisY  " );     // DataAxisY
   Q_CLASSINFO( "enum_DataType_3",   "AxisZ  " );     // DataAxisZ
   Q_CLASSINFO( "enum_DataType_4",   "Plot   " );     // DataPlot
   Q_CLASSINFO( "enum_DataType_5",   "Step   " );     // DataStep
   Q_CLASSINFO( "enum_DataType_6",   "Tape   " );     // DataTape
   Q_CLASSINFO( "enum_DataType_7",   "Stem   " );     // DataStem
   Q_CLASSINFO( "enum_DataType_8",   "Bars   " );     // DataBars
   Q_CLASSINFO( "enum_DataType_9",   "Barh   " );     // DataBarh
   Q_CLASSINFO( "enum_DataType_10",  "Tens  1" );     // DataTens
   Q_CLASSINFO( "enum_DataType_11",  "Area   " );     // DataArea
   Q_CLASSINFO( "enum_DataType_12",  "Region 1" );    // DataRegion
   Q_CLASSINFO( "enum_DataType_13",  "OHLC  3" );     // DataOHLC
   Q_CLASSINFO( "enum_DataType_14",  "BoxPlot 5" );   // DataBoxPlot
   Q_CLASSINFO( "enum_DataType_15",  "Candle  3" );   // DataCandle
   Q_CLASSINFO( "enum_DataType_16",  "Cones  " );     // DataCones
   Q_CLASSINFO( "enum_DataType_17",  "Error 2" );     // DataError
   Q_CLASSINFO( "enum_DataType_18",  "Mark  1" );     // DataMark
   Q_CLASSINFO( "enum_DataType_19",  "Tube   " );     // DataTube
   Q_CLASSINFO( "enum_DataType_20",  "Surf 2D" );     // DataSurf
   Q_CLASSINFO( "enum_DataType_21",  "SurfC 2D" );    // DataSurfC
   Q_CLASSINFO( "enum_DataType_22",  "SurfA 2D" );    // DataSurfA
   Q_CLASSINFO( "enum_DataType_23",  "Mesh 2D" );     // DataMesh
   Q_CLASSINFO( "enum_DataType_24",  "Fall 2D" );     // DataFall
   Q_CLASSINFO( "enum_DataType_25",  "Belt 2D" );     // DataBelt
   Q_CLASSINFO( "enum_DataType_26",  "Dens 2D" );     // DataDens
   Q_CLASSINFO( "enum_DataType_27",  "Cont 2D" );     // DataCont
   Q_CLASSINFO( "enum_DataType_28",  "ContF 2D" );    // DataContF
   Q_CLASSINFO( "enum_DataType_29",  "ContD 2D" );    // DataContD
   Q_CLASSINFO( "enum_DataType_30",   "C0     " );    // DataC0
   Q_CLASSINFO( "enum_DataType_31",   "C1     " );    // DataC1
   Q_CLASSINFO( "enum_DataType_32",   "C2     " );    // DataC2
   Q_CLASSINFO( "enum_DataType_33",   "C3     " );    // DataC3
   Q_CLASSINFO( "enum_DataType_34",   "C4     " );    // DataC4
   Q_CLASSINFO( "enum_DataType_35",   "C5     " );    // DataC5     

 protected:
   PRM_LIST( type,    efNRC, "Type", "Data Type", "enum=DataType" );
   PRM_SWITCH( is2D,  efNRC, "2D", "Data for 2D plot", "" );
   PRM_STRING( src,   efNRC, "Source", "Name of source for values", "max=128" );
   PRM_STRING( label, efNRC, "Label", "Label", "max=128" );
   PRM_COLOR(  color, efNRC, "line color", "plot line color", "def=black" );
   PRM_INT(    lw,    efNRC, "line width", "plot line width", "def=1\nmin=0\nmax=9" );
   PRM_SWITCH(noColor,efNRC, "NO color", "ignore color and line width", "" );
   PRM_STRING( extra, efNRC, "Extra", "Extra pen params to plot", "max=128\ndef=-" );
   PRM_STRING( opt,   efNRC, "Options", "Options to plot", "max=128" );

   DCL_DEFAULT_STATIC;
};


// --------------------------------------------------------------------

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
   void migrate1(); // to migrate
 protected:

   /** title of graph  */
   PRM_STRING( title, efNRC, "Title", "Plot title", "max=128\nncol=-1\ndef=fig. " );

   // old values: TODO: remove after conversion
   /** name of output array for x  */
   PRM_STRING( xname,  efOld, "X  name", "Name of source for X values", "max=64\nsep=block" );
   PRM_COLOR( bgcolor, efOld, "BG color", "Background color", "sep=col\ndef=#FFFFFF");
   /** names of output array for y[i]  */
   PRM_STRING( y0name, efOld, "Y0 name", "Name of source for Y0 values", "max=64" );
   PRM_STRING( y1name, efOld, "Y1 name", "Name of source for Y1 values", "max=64" );
   PRM_STRING( y2name, efOld, "Y2 name", "Name of source for Y2 values", "max=64" );
   PRM_STRING( y3name, efOld, "Y3 name", "Name of source for Y3 values", "max=64" );
   PRM_STRING( y4name, efOld, "Y4 name", "Name of source for Y4 values", "max=64" );
   PRM_STRING( y5name, efOld, "Y5 name", "Name of source for Y5 values", "max=64" );
   /** color of lines */
   PRM_COLOR( y0color, efOld, "Y0 color", "Color for Y0", "def=white\nsep=col");
   PRM_COLOR( y1color, efOld, "Y1 color", "Color for Y1", "def=yellow");
   PRM_COLOR( y2color, efOld, "Y2 color", "Color for Y2", "def=#ff0000");
   PRM_COLOR( y3color, efOld, "Y3 color", "Color for Y3", "def=#00ff00");
   PRM_COLOR( y4color, efOld, "Y4 color", "Color for Y4", "def=#0000ff");
   PRM_COLOR( y5color, efOld, "Y5 color", "Color for Y5", "def=#ff00ff");
   // TODO: legend
   ScaleData *scd;

   // just to test
   // mglData mgd;

   DCL_DEFAULT_STATIC;
};
typedef TGraph *PTGraph;

#endif

