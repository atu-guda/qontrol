/***************************************************************************
                          root.cpp  -  TRootData implementation
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "rootdata.h"
using namespace std;

const char* TRootData::helpstr = "<H1>TRootData</H1>\n"
 "Root element for all elemsnts tree";

STD_CLASSINFO(TRootData,clpContainer|clpSpecial );


CTOR(TRootData,TDataContainer)
{
  allow_add = allowObject | allowParam;
  allowed_types = "TModel";
}

TRootData::~TRootData()
{
}

DEFAULT_FUNCS_REG(TRootData)

// end of rootdata.cpp


