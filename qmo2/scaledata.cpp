/***************************************************************************
                          scaledata.cpp  -  ScaleData implementation
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "scaledata.h"

using namespace std;

const char* ScaleData::helpstr = "<H1>ScaleData</H1>\n"
 "Information about scales on plots";


TClassInfo ScaleData::class_info = 
 {  "ScaleData",  ScaleData::create, 
    &TDataSet::class_info, "", clpSpecial };


ScaleData::ScaleData( TDataSet* apar )
          :TDataSet( apar )
{
  realMinX = plotMinX = 0; realMaxX =  plotMaxX = 1; 
  autoScX = goodScX = 1;  logX = logScaleX =  zeroX = centerX = 0;
  gridX = 10; tickX = 1,
  realMinY = plotMinY = 0; realMaxY =  plotMaxY = 1; 
  autoScY = goodScY = 1;  logY = logScaleY =  zeroY = centerY = 0;
  gridY = 10; tickY = 1,
  leftMar = 0.1; topMar = 0.02; rightMar = 0.1; bottomMar = 0.1; maxErr = 0.5;
}

ScaleData::~ScaleData()
{
}

const TClassInfo* ScaleData::getClassInfo(void) const
{
  return &class_info;
}

TDataSet* ScaleData::create( TDataSet* apar )
{
  return new ScaleData( apar );
}

const char *ScaleData::getHelp(void) const
{
  return helpstr;
}


int ScaleData::registered = reg();

int ScaleData::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of scaledata.cpp


