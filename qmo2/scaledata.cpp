/***************************************************************************
                          scaledata.cpp  -  ScaleData implementation
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "scaledata.h"

using namespace std;

const char* ScaleData::helpstr = "<H1>ScaleData</H1>\n"
 "Information about scales on plots";

STD_CLASSINFO(ScaleData,clpSpecial);


CTOR(ScaleData,TDataSet)
{
  reset_dfl();
}

ScaleData::~ScaleData()
{
}


DEFAULT_FUNCS_REG(ScaleData)

