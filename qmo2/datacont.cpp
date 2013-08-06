/***************************************************************************
                          datacont.cpp  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/


#include "datacont.h"

const char* TDataContainer::helpstr = "<H1>TDataContainer</H1>\n"
 "Base class for all elements, which can hold \n"
 "unspecified at compile time elements";

TClassInfo TDataContainer::class_info = 
 {  "TDataContainer",  TDataContainer::create, 
    &TDataSet::class_info, helpstr, clpContainer };


TDataContainer::TDataContainer( TDataSet *aparent )
               :TDataSet( aparent )
{
  allow_add = 3; // obj and params
  state = 1;
}

TDataContainer::~TDataContainer()
{
}

const TClassInfo* TDataContainer::getClassInfo(void) const
{
  return &class_info;
}

/** creates instance */
TDataSet* TDataContainer::create( TDataSet* apar )
{
  return new TDataContainer( apar );
}

const char *TDataContainer::getHelp(void) const
{
  return helpstr;
}

int TDataContainer::registered = reg();

int TDataContainer::reg()
{
  return EFACT.registerElemType( &class_info  );
}

