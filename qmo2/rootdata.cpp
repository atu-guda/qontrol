/***************************************************************************
                          root.cpp  -  TRootData implementation
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include "rootdata.h"
using namespace std;

const char* TRootData::helpstr = "<H1>TRootData</H1>\n"
 "Root element for all elemsnts tree";


TClassInfo TRootData::class_info = 
 {  CLASS_ID_TRootData, "TRootData",  TRootData::create, 
    &TDataContainer::class_info, "", clpContainer | clpSpecial };


TRootData::TRootData( TDataSet* apar )
          :TDataContainer( apar )
{
  allow_add = 3;  // obj(1) and params(2)
}

TRootData::~TRootData()
{
}

const TClassInfo* TRootData::getClassInfo(void) const
{
  return &class_info;
}

TDataSet* TRootData::create( TDataSet* apar )
{
  return new TRootData( apar );
}

const char *TRootData::getHelp(void) const
{
  return helpstr;
}


int TRootData::registered = reg();

int TRootData::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of rootdata.cpp


