/***************************************************************************
                          root.cpp  -  TRootData implementation
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "rootdata.h"
using namespace std;

const char* TRootData::helpstr = "<H1>TRootData</H1>\n"
 "Root element for all elemsnts tree";


TClassInfo TRootData::class_info = 
 {  "TRootData",  TRootData::create, 
    &TDataContainer::class_info, "", clpContainer | clpSpecial };


TRootData::TRootData( TDataSet* apar )
          :TDataContainer( apar )
{
  allow_add = allowObject | allowParam;
}

TRootData::~TRootData()
{
}

DEFAULT_FUNCS_REG(TRootData)

// end of rootdata.cpp


