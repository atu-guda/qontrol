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
     DataC0, DataC1, DataC2, DataC3, DataC4, DataC5, // aux data for misc plots
     DataPlot,  DataStep, DataTape,
     DataStem, DataBars, DataBarh, DataTens, DataArea,
     DataRegion, DataOHLC, DataBoxPlot, DataCandle, DataCones,
     DataError, DataMark, DataTube,
     DataSurf, DataMesh, DataFall, DataBelt, DataDens,
     DataCont, DataContF, DataContD
   };
   Q_ENUMS( DataType );
   Q_CLASSINFO( "enum_DataType_0",   "None   " );     // DataNone
   Q_CLASSINFO( "enum_DataType_1",   "AxisX  " );     // DataAxisX
   Q_CLASSINFO( "enum_DataType_2",   "AxisY  " );     // DataAxisY
   Q_CLASSINFO( "enum_DataType_3",   "AxisZ  " );     // DataAxisZ
   Q_CLASSINFO( "enum_DataType_4",   "C0     " );     // DataC0
   Q_CLASSINFO( "enum_DataType_5",   "C1     " );     // DataC1
   Q_CLASSINFO( "enum_DataType_6",   "C2     " );     // DataC2
   Q_CLASSINFO( "enum_DataType_7",   "C3     " );     // DataC3
   Q_CLASSINFO( "enum_DataType_8",   "C4     " );     // DataC4
   Q_CLASSINFO( "enum_DataType_9",   "C5     " );     // DataC5
   Q_CLASSINFO( "enum_DataType_10",  "Plot   " );     // DataPlot
   Q_CLASSINFO( "enum_DataType_11",  "Step   " );     // DataStep
   Q_CLASSINFO( "enum_DataType_12",  "Tape   " );     // DataTape
   Q_CLASSINFO( "enum_DataType_13",  "Stem   " );     // DataStem
   Q_CLASSINFO( "enum_DataType_14",  "Bars   " );     // DataBars
   Q_CLASSINFO( "enum_DataType_15",  "Barh   " );     // DataBarh
   Q_CLASSINFO( "enum_DataType_16",  "Tens  1" );     // DataTens
   Q_CLASSINFO( "enum_DataType_17",  "Area   " );     // DataArea
   Q_CLASSINFO( "enum_DataType_18",  "Region 1" );    // DataRegion
   Q_CLASSINFO( "enum_DataType_19",  "OHLC  3" );     // DataOHLC
   Q_CLASSINFO( "enum_DataType_20",  "BoxPlot 5" );   // DataBoxPlot
   Q_CLASSINFO( "enum_DataType_21",  "Candle  3" );   // DataCandle
   Q_CLASSINFO( "enum_DataType_22",  "Cones  " );     // DataCones
   Q_CLASSINFO( "enum_DataType_23",  "Error 2" );     // DataError
   Q_CLASSINFO( "enum_DataType_24",  "Mark  1" );     // DataMark
   Q_CLASSINFO( "enum_DataType_25",  "Tube   " );     // DataTube
   Q_CLASSINFO( "enum_DataType_26",  "Surf 2D" );     // DataSurf
   Q_CLASSINFO( "enum_DataType_27",  "Mesh 2D" );     // DataMesh
   Q_CLASSINFO( "enum_DataType_28",  "Fall 2D" );     // DataFall
   Q_CLASSINFO( "enum_DataType_29",  "Belt 2D" );     // DataBelt
   Q_CLASSINFO( "enum_DataType_30",  "Dens 2D" );     // DataDens
   Q_CLASSINFO( "enum_DataType_31",  "Cont 2D" );     // DataCont
   Q_CLASSINFO( "enum_DataType_32",  "ContF 2D" );    // DataContF
   Q_CLASSINFO( "enum_DataType_33",  "ContD 2D" );    // DataContD

 protected:
   PRM_LIST( type,    efNRC, "Type", "Data Type", "enum=DataType" );
   PRM_SWITCH( is2D,  efNRC, "2D", "Data for 2D plot", "" );
   PRM_STRING( src,   efNRC, "Source", "Name of source for values", "max=128" );
   PRM_STRING( label, efNRC, "Label", "Label", "max=128" );
   PRM_COLOR(  color, efNRC, "line color", "plot line color", "def=black" );
   PRM_INT(    lw,    efNRC, "line width", "plot line width", "def=1\nmin=0\nmax=9" );
   PRM_STRING( extra, efNRC, "Extra", "Extra options to plot", "max=128\ndef=-" );

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

   enum PlotType {
     PlotPlot = 0, PlotRadar, PlotStep, PlotArea, PlotRegion, // 1D
     PlotStem, PlotBars, PlotBarh, PlotChart,
     PlotSurf, PlotMesh, PlotFall, PlotBelt, PlotCont // 2D. TODO: more
   };
   Q_ENUMS( PlotType );
   Q_CLASSINFO( "enum_PlotType_0",  "Plot   " );      // PlotPlot
   Q_CLASSINFO( "enum_PlotType_1",  "Radar  " );      // PlotRadar
   Q_CLASSINFO( "enum_PlotType_2",  "Step   " );      // PlotStep
   Q_CLASSINFO( "enum_PlotType_3",  "Area   " );      // PlotArea
   Q_CLASSINFO( "enum_PlotType_4",  "Region " );      // PlotRegion
   Q_CLASSINFO( "enum_PlotType_5",  "Stem   " );      // PlotStem
   Q_CLASSINFO( "enum_PlotType_6",  "Bars   " );      // PlotBars
   Q_CLASSINFO( "enum_PlotType_7",  "Barh   " );      // PlotBarh
   Q_CLASSINFO( "enum_PlotType_8",  "Chart  " );      // PlotChart
   Q_CLASSINFO( "enum_PlotType_9",  "Surf 2D" );      // PlotSurf
   Q_CLASSINFO( "enum_PlotType_10", "Mesh 2D" );      // PlotMesh
   Q_CLASSINFO( "enum_PlotType_11", "Fall 2D" );      // PlotFall
   Q_CLASSINFO( "enum_PlotType_12", "Belt 2D" );      // PlotBelt
   Q_CLASSINFO( "enum_PlotType_13", "Cont 2D" );      // PlotCont

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
   PRM_LIST( type,  efNRC, "Type", "Plot Type", "enum=PlotType" );
   PRM_STRING( title, efNRC, "Title", "Plot title", "max=128\nsep=col\ndef=fig. " );

   // old values: TODO: remove after conversion
   /** name of output array for x  */
   PRM_STRING( xname, efNRC, "X  name", "Name of source for X values", "max=64\nsep=block" );
   PRM_COLOR( bgcolor, efOld, "BG color", "Background color", "sep=col\ndef=#FFFFFF");
   /** names of output array for y[i]  */
   PRM_STRING( y0name, efNRC, "Y0 name", "Name of source for Y0 values", "max=64" );
   PRM_STRING( y1name, efNRC, "Y1 name", "Name of source for Y1 values", "max=64" );
   PRM_STRING( y2name, efNRC, "Y2 name", "Name of source for Y2 values", "max=64" );
   PRM_STRING( y3name, efNRC, "Y3 name", "Name of source for Y3 values", "max=64" );
   PRM_STRING( y4name, efNRC, "Y4 name", "Name of source for Y4 values", "max=64" );
   PRM_STRING( y5name, efNRC, "Y5 name", "Name of source for Y5 values", "max=64" );
   /** color of lines */
   PRM_COLOR( y0color, efNRC, "Y0 color", "Color for Y0", "def=white\nsep=col");
   PRM_COLOR( y1color, efNRC, "Y1 color", "Color for Y1", "def=yellow");
   PRM_COLOR( y2color, efNRC, "Y2 color", "Color for Y2", "def=#ff0000");
   PRM_COLOR( y3color, efNRC, "Y3 color", "Color for Y3", "def=#00ff00");
   PRM_COLOR( y4color, efNRC, "Y4 color", "Color for Y4", "def=#0000ff");
   PRM_COLOR( y5color, efNRC, "Y5 color", "Color for Y5", "def=#ff00ff");
   // TODO: legend
   ScaleData *scd;

   // just to test
   // mglData mgd;

   DCL_DEFAULT_STATIC;
};
typedef TGraph *PTGraph;

#endif

