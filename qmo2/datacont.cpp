/***************************************************************************
                          datacont.cpp  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
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

DEFAULT_FUNCS_REG(TDataContainer)

