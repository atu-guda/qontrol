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
 {  CLASS_ID_TDataContainer, "TDataContainer",  TDataContainer::create, 
    &TDataSet::class_info, helpstr, clpContainer };


TDataContainer::TDataContainer( TDataSet *aparent )
               :TDataSet( aparent )
{
  allow_add = 1; 
  ptrs.reserve(512); // must be good for containers
  d_i = new TDataInfo[MAX_DATAELEM];
  d_i_alloc = 1;
  d_i[0].tp = dtpEnd; d_i[0].descr = d_i[0].listdata = 0;
  ptrs.push_back( 0 );
  state = 1;
  initHash(); // realy not need for this class ?
}

TDataContainer::~TDataContainer()
{
}

int TDataContainer::getClassId(void) const 
{ 
  return CLASS_ID_TDataContainer; 
}

const char* TDataContainer::getClassName(void) const
{ 
  return "TDataContainer"; 
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


