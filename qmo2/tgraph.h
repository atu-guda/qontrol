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
class DatasInfo;

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


   /** fills fields in DatasInfo structure, return number of elements (nn) */
   int fillDatasInfo( DatasInfo *di ) const;
   /** dumps data to file */
   int  dump( const QString &fn, const QString &delim = " " );
   //* add new GraphElem to given TOutArr
   int addOutArr( const QString &o_name );
 protected:

   /** title of graph  */
   PRM_STRING( title, efNRC, "Title", "Plot title", "max=128\nncol=-1\ndef=fig. " );

   ScaleData *scd;

   DCL_DEFAULT_STATIC;
};
typedef TGraph *PTGraph;

#endif

