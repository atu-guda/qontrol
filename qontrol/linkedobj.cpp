/***************************************************************************
                          linkedobj.h - object with link capabilities
                             -------------------
    begin                : 2015.10.24
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "linkedobj.h"
#include "miscfun.h"

using namespace std;


STD_CLASSINFO(LinkedObj,clpAbstract);


const char* LinkedObj::helpstr =
 "<H1>LinkedObj</H1>\nThis is a base element for all linked with double elements\n"
 "Never to be used directly";

CTOR(LinkedObj,TDataSet)
{
}

DEFAULT_FUNCS_REG(LinkedObj);

const double* LinkedObj::getDoublePtr( const QString &nm, int *lt,
              const LinkedObj **targ, int lev  ) const
{
  int clt; // fake value to safe ptr usage
  int *plt = lt ? lt : &clt;
  if( nm.isEmpty() ) {
    *plt = LinkNone; return nullptr;
  }
  if( isIgnored() ) {
    *plt = LinkBad;
    return nullptr;
  }
  QString first, rest;

  int idx;
  auto nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    *plt = LinkBad;
    qWarning() << "bad source name " << nm << NWHE;
    // abort();
    return nullptr;
  }

  HolderData *ho = getObj( first );

  if( !ho ) {
    *plt = LinkBad;
    // qWarning() << "elem" << first << "not found, nm= " << nf << NWHE;
    return nullptr;
  }

  if( nm_type == simpleName ) { // -- simple name ----- first only -----
    LinkedObj *ds= qobject_cast<LinkedObj*>(ho);
    if( ds ) {
      return ds->getDoublePtr( QSL("out0"), plt, targ, lev+1 ); // default output
    }

    HolderDouble *hod = qobject_cast<HolderDouble*>(ho);
    if( hod ) {
      *plt = ( lev == 1 ) ? LinkElm : LinkSpec;
      if( targ ) {  *targ = this;   }
      return hod->caddr();
    }

    auto *hoda = qobject_cast<HolderDoubleArray*>(ho); // array special case
    if( hoda ) {
      auto na = hoda->arrSize();
      if( (unsigned long)idx >= na ) {
        qWarning() << "Bad index " << idx << " while access to " << hoda->getFullName()
                   << " size " << na << NWHE;
        *plt = LinkBad;
        return nullptr;
      }
      *plt = ( lev == 1 ) ? LinkElm : LinkSpec;
      if( targ ) {  *targ = this;   }
      return & ( hoda->operator[](idx) );
    }

    return nullptr;
  } // -------------------- simple name end ----------------------------

  // both part of name exists
  LinkedObj *ds = qobject_cast<LinkedObj*>(ho);
  if( !ds ) {
    *plt = LinkBad;
    qWarning() << "Complex name " << nm << " is given for simple object "
               << ho->getFullName() << NWHE;
    return nullptr;
  }
  return ds->getDoublePtr( rest, plt, targ, lev+1 ); // pass to child

}


const double* LinkedObj::getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev) const
{
  int clt = LinkNone;
  int *plt = ( lt ) ? lt : &clt;  // failsafe link

  if( isIgnored() || nm.isEmpty() ) {
    *plt = LinkNone;
    return nullptr;
  }

  LinkedObj *ds = qobject_cast<LinkedObj*>( par );  // parent-less object or root
  if( !ds ) {
    *plt = LinkBad;
    return nullptr;
  }
  return ds->getSchemeDoublePtr( nm, lt, src_ob, lev );
}

// not const - change param
double* LinkedObj::getDoublePrmPtr( const QString &nm, int *flg )
{
  if( nm.isEmpty() ) {
    return nullptr;
  }

  HolderDouble *hod = getObjT<HolderDouble*>( nm );

  if( !hod ) {
    // qWarning() << "fail to find name " << nm << NWHE;
    return 0;
  }

  if( flg ) {
    *flg = hod->getFlags();
  }
  return hod->addr();
}

bool LinkedObj::isIgnored() const
{
  int ign = 0;
  for( const HolderData *p = this; p; p=p->getParent() ) {
    p->getData( "ignored", &ign, false ); // not direct access, TMiso+... property
    if( ign ) {
      return true;
    }
  }
  return false;
}


void LinkedObj::do_structChanged()
{
  // TDataSet::do_structChanged(); // empty - do not call
  inps.clear(); inps_a.clear(); inps_s.clear(); inps_ap.clear();

  for( auto in : TCHILD(InputAbstract*) ) {
    in->set_link();
    inps.append( in );

    int lt = in->getLinkType();
    if( ( lt == LinkElm || lt == LinkSpec ) && ! in->getOnlyFirst()  ) {
      inps_a.append( in );
    }

    InputSimple *in_s = qobject_cast<InputSimple*>( in );
    if( in_s ) {
      inps_s.append( in_s );
    }

    ParamDouble *in_p = qobject_cast<ParamDouble*>( in );
    if( in_p && (( lt == LinkElm || lt == LinkSpec )) ) {
      inps_ap.append( in_p );
      prm_will_mod += in_p->isFixparmNeed();
    }
  }
  readAllInputs(); // realy fake: just for visual representation
}

void LinkedObj::readInputs() noexcept
{
  if( needReadInputsRecurse ) {
    for( auto in : TCHILD(LinkedObj*) ) {
      in->readInputs();
    }
  }

  for( auto in : inps_a ) {
    in->readInput();
  }
  prm_mod += prm_will_mod;
}

void LinkedObj::readAllInputs() noexcept
{
  if( needReadInputsRecurse ) {
    for( auto in : TCHILD(LinkedObj*) ) {
      in->readAllInputs();
    }
  }

  for( auto in : inps  ) {
    in->readInput();
  }
  prm_mod += prm_will_mod;
}

void LinkedObj::iterateSources( int dn )
{
  for( auto ia : TCHILD(InputAbstract*) ) {
    ia->set_link(); // to have correct type
    ia->iterateSource( dn );
  }
}

int LinkedObj::preRun( const RunInfo *rinf_ )
{
  if( !rinf_ ) {
    qWarning() << "rinf_ is nullptr in " << getFullName() << NWHE;
    return 0;
  }
  rinf = rinf_;
  // reset(); ????
  iter_c = IterNo; state = stateRun;
  prm_mod = 0;

  ctdt = rinf->tdt; p_t_model = rinf->p_t_model;
  int rc =  do_preRun();
  if( !rc ) {
    qWarning() << "preRun failed for object " << getFullName() << NWHE;
    return 0;
  }

  for( auto ob : TCHILD(LinkedObj*) ) {
    rc = ob->preRun( rinf );
    if( !rc ) {
      return 0;
    }
  };
  return ( state > stateBad ) ? 1 : 0;
}

int LinkedObj::do_preRun()
{
  return 1;
}


int LinkedObj::postRun( int good )
{
  for( auto ob : TCHILD(LinkedObj*) ) {
    ob->postRun( good );
  };
  do_postRun( good );

  state = good ? stateDone : stateGood; // TODO: or state Bad?
  iter_c = IterNo;
  return 1;
}

int LinkedObj::do_postRun(  int /*good*/  )
{
  return 1;
}


int LinkedObj::startLoop( long acnx, long acny )
{
  readAllInputs();
  int rc = do_startLoop( acnx, acny );
  if( rc == 0 ) {
    qWarning() << "Fail to start " << NWHE;
    return 0;
  }

  for( auto ob : TCHILD(LinkedObj*) ) {
    rc = ob->startLoop( acnx, acny );
    if( !rc ) {
      return 0;
    }
  };

  rc = post_startLoop();
  if( rc == 0 ) {
    qWarning() << "Fail to make post_startLoop " << NWHE;
    return 0;
  }
  return 1;
}

int LinkedObj::do_startLoop( long /*acnx*/, long /*acny*/ )
{
  return 1;
}

int LinkedObj::post_startLoop()
{
  return 1;
}


int LinkedObj::endLoop()
{
  for( auto ob : TCHILD(LinkedObj*) ) {
    ob->endLoop();
  };
  return do_endLoop();
}


int LinkedObj::do_endLoop()
{
  return 1;
}

QStringList LinkedObj::getInputsNames() const
{
  QStringList r;
  for( auto in : TCHILD(InputAbstract*) ) {
    if( !in ) { continue; };
    r << in->objectName();
  }

  return r;
}

QString LinkedObj::lsInputs() const
{
  QString r;
  for( auto in : TCHILD(InputAbstract*) ) {
    if( !in ) { continue; };
    r += in->getType() % QSL(" ") % in->objectName() % QSL(";  //  ")
      % in->getParm( QSL("descr") ) % QSL("\n");
  }

  return r;
}


// ------------------------------------ InputAbstract ---------
//
STD_CLASSINFO(InputAbstract,clpSpecial|clpAbstract);


CTOR(InputAbstract,LinkedObj)
{
  if( getParm( QSL("props") ) == QSL("OBJ") ) {
    setParm( QSL("props"), QSL("OBJ,INPUT") );
  }
}

void InputAbstract::reset_dfl()
{
  LinkedObj::reset_dfl();
  QString s = getParm( QSL("def") );
  source = s;
}

bool InputAbstract::fromString( const QString &s )
{
  if( s[0] == '<' || s.length() > 256 ) {
    return LinkedObj::fromString( s );
  }
  QString t = s;
  if( t.endsWith( QSL("!") ) ) {
    onlyFirst = 1;
    t.resize( t.length() - 1 );
  }
  if( t.endsWith( QSL("$") ) ) {
    onlyLabel = 1;
    t.resize( t.length() - 1 );
  }
  source = t;
  set_link(); // in special case only
  return true;
}


void InputAbstract::do_structChanged()
{
  LinkedObj::do_structChanged();
  set_link();
}

void InputAbstract::set_link()
{
  direct_in = 0; out0 = 0;
  p = &direct_in; src_obj = nullptr; linkType = LinkBad; srcObjName = QSL(":BAD:");
  if( isIgnored() || source.cval().isEmpty() ) {
    linkType = LinkNone; srcObjName = QSL(":NONE:"); return;
  }

  bool ok = false;
  double v = toDoubleEx( source.cval() , &ok );
  if( ok ) {
    direct_in = v * factor + shift; out0 = direct_in;
    linkType = LinkNone;  srcObjName = QSL("=") % QSN( v );
    return;
  }


  int lt;
  const LinkedObj *srct = nullptr;

  const double *cp = getSchemeDoublePtr( source, &lt, &srct, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;  src_obj = srct; linkType = lt;
    if( srct ) {
      srcObjName = srct->getFullName();
    } else {
      srcObjName = QSL("$");
    }
  } else {
    qWarning() << "ptr not found for " << source << NWHE;
  }

}


QVariant InputAbstract::dataObj( int col, int role ) const
{
  if( role == Qt::StatusTipRole && col < 2 ) { // used for button labels in dialogs
    return prepTextVisual( false );
  }

  return LinkedObj::dataObj( col, role );
}

QString InputAbstract::prepTextVisual( bool isLong ) const
{
  bool isScaled = ( fabs(factor-1) > D_AZERO ) || ( fabs(shift) > D_AZERO );
  QString s = isScaled ? QSL("!") : QSL("");

  int lt  = linkType;
  switch( lt ) {
    case LinkNone:
      if( ! isLong ) {
        s += QSL("=");
      }
      break;
    case LinkElm:
      if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
        s += QChar( 0x2794 ); // ->
      } else {
        s += QChar( 0x27BC ); // >>-> // unlikely
      }
      if( isLong ) {
        s += source.cval();
      }
      break;
    case LinkSpec:
      if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
        s += QChar( 0x27A0 ); // =>
      } else {
        s += QChar( 0x27A4 ); // >>
      }
      if( isLong ) {
        s += source.cval();
      }
      break;
    case LinkBad:
      s += QChar( 0x274C ); // X
      break;
    default:
      s += QSL("?");
      break;
  }

  if( onlyFirst ) {
    s += QChar( 0x271D ); // +
  }

  if( isLong ) {
    if( src_obj ) {
      s += QSL("(") % src_obj->getFullName() % QSL(")");
    }
    s += QSL("= ") % QSND( out0 );
  }

  return s;
}

QString InputAbstract::textVisual() const
{
  return prepTextVisual( true );
}

int InputAbstract::iterateSource( int dn )
{
  if( fixedSource || linkType != LinkElm ) {
    return -1000000;
  }
  QString bname;
  int ne = splitNameNum( source, bname );
  if( ne < 0 ) {
    return -1000000;
  }
  ne += dn;
  int ene = ne;
  QString so_new = bname;
  if( ne < 0 ) {
    ene = -ene; so_new += 'Z';
  }
  so_new += QSN( ene );
  source = so_new;
  set_link();
  return ne;
}

const char* InputAbstract::helpstr { "Abstract link" };

DEFAULT_FUNCS_REG(InputAbstract);

// ------------------------------------ InputSimple ---------
STD_CLASSINFO(InputSimple,0);

CTOR(InputSimple,InputAbstract)
{
}


const char* InputSimple::helpstr { "Link to source of simple double data" };

DEFAULT_FUNCS_REG(InputSimple);


// ------------------------------------ InputLogic ---------
STD_CLASSINFO(InputLogic,0);

CTOR(InputLogic,InputSimple)
{
  HolderColor *co = getObjT<HolderColor*>( QSL("line_color" ) );
  if( co ) {
    co->setParm( QSL("def"), QSL("blue" ) );
  }
}

void InputLogic::post_readInput()
{
  bool cll = sll;
  switch( (InputLogicType)(int)(type ) ) {
    case itLevel: sll = cll = (out0 >= l1);                    break;
    case itRise:  sll = cll = ( (out0 - old_out0) >=  l1 );    break;
    case itFall:  sll = cll = ( (out0 - old_out0) <= -l1 );    break;
    case itBoth:  sll = cll = ( fabs(out0 - old_out0) >= l1 ); break;
    case itShmitt:
         if( out0 >= l1 ) { cll = true;  break; }
         if( out0 <  l0 ) { cll = false; break; }
         sll = cll;
         break;
    case itShmittRise:
         cll = 0;
         if( out0 >= l1 ) {
           if( !sll ) {
             cll = true;
           }
           sll = true;  break;
         }
         if( out0 <  l0 ) {
           sll = cll = false; break;
         }
         break;
    default: sll = cll = false; // unlikely
  }
  if( inv_in ) {
    cll = !cll;
  }
  ll = cll; llv = cll ? 1.0 : 0.0;
  old_out0 = out0;
}

const char* InputLogic::helpstr { "Link to source, converted to logic value (lval)" };

DEFAULT_FUNCS_REG(InputLogic);



// ------------------------------------ ParamDouble ---------
STD_CLASSINFO(ParamDouble,0);

CTOR(ParamDouble,InputAbstract)
{
  need_fixparm = getParmInt( "fixparm", 0 );
  HolderColor *co = getObjT<HolderColor*>( QSL("line_color" ) );
  if( co ) {
    co->setParm( QSL("def"), QSL("red" ) );
  }
}


const char* ParamDouble::helpstr { "Double parameter" };

DEFAULT_FUNCS_REG(ParamDouble);




// end of linkedobj.cpp


