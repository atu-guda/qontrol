/***************************************************************************
                          structview.cpp  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2015 by atu
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

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>

#include "miscfun.h"
#include "structview.h"
#include "doubletable.h"
#include "labowin.h"
#include "labodoc.h"
#include "laboview.h"
#include "runview.h"
#include "outdataview.h"
#include "addelemdia.h"



StructView::StructView( QWidget *a_par, Scheme *a_sch,  LaboView *mview, OutDataView *a_oview )
            : QWidget( a_par ), sch( a_sch ), mainview( mview ), oview( a_oview )
{
  em = LaboWin::Em();
  QPalette pal;
  pal.setBrush( QPalette::Window, QBrush( Qt::white ) );
  setPalette( pal );
  setAutoFillBackground(true);

  setMaximumSize( grid_sz*MODEL_MX, grid_sz*MODEL_MY );
  // setMinimumSize( grid_sz*MODEL_MX, grid_sz*MODEL_MY ); // debug
  setMinimumSize( grid_sz*8, grid_sz*6 );
  setFocusPolicy( Qt::StrongFocus );
  // setFocus();
}

StructView::~StructView()
{

}

QSize StructView::getElemsBound() const
{
  if( !sch ) {
    return QSize( 10, 10 );
  }
  QSize mss = sch->getMaxXY().expandedTo( QSize( sel_x, sel_y ) );
  mss += QSize( 3, 3 ); // free bound
  mss *= grid_sz;       // to grid scale

  return mss;
  // TODO: rethink size
}

QSize StructView::sizeHint() const
{
  return getElemsBound();
}

QPoint StructView::getSelCoords() const
{
  return QPoint( sel_x * grid_sz + grid_sz/2, sel_y * grid_sz + grid_sz/2 );
}


void StructView::update()
{
  resize( getElemsBound() );
  QWidget::update();
}

void StructView::print()
{
  printAll();
}


void StructView::changeSel( int x, int y, int rel )
{
  TMiso *ob;
  if( ! sch ) {
    return;
  }

  selObj = nullptr; int n_obj = -1;
  switch( rel ) {
    case 0: sel_x = x; sel_y = y; break;   // absolute
    case 1: sel_x += x; sel_y += y; break; // relative
    case 2:                                // next elem
            ob = sch->xy2Miso( sel_x, sel_y );
            if( ob ) {
              n_obj = ob->getMyIndexInParent();
            }
            ++n_obj;
            ob = qobject_cast<TMiso*>( sch->getElem( n_obj ) );
            if( ob ) {
              sel_x = ob->getDataD( "vis_x", 0 );
              sel_y = ob->getDataD( "vis_y", 0 );
            };
            break;
    default: break;
  };
  sel_x = qBound( 0, sel_x, MODEL_MX-1 );
  sel_y = qBound( 0, sel_y, MODEL_MY-1 );
  sel = -1;
  ob = sch->xy2Miso( sel_x, sel_y );
  if( ob ) {
    selObj = ob;
    sel = ob->getMyIndexInParent();
  }
  update();
  emit viewChanged();
}


void StructView::changeLevel( int lev )
{
  level = lev;
  if( level < 0 || level >= 64 ) {
    level = 0;
  }
  update();
  emit viewChanged();
}



void StructView::paintEvent( QPaintEvent * /*pe*/ )
{
  if( ! sch ) {
    return;
  }
  devTp = 0;
  QPainter p( this );

  drawAll( p );
  p.end();
}

void StructView::printAll()
{
  if( !sch ) {
    return;
  }

  QPrinter *pr = LaboWin::win()->getPrinter();
  if( !pr ) {
    qWarning() << "Printer not found!";
    return;
  }

  QPrintDialog pr_dialog( pr, this );
  if( ! pr_dialog.exec() ) {
    return;
  }

  devTp = 1;
  pr->setFullPage( false );
  pr->newPage();
  QPainter p( pr );
  drawAll( p );
  p.end();
}

bool StructView::fill_elmInfo( const TMiso * ob, ElemInfo &ei ) const
{
  if( ! ob )
    return false;

  ei.name = ob->objectName();
  ei.type = ob->getType();
  ei.vis_x  = ei.vis_y =  ei.ord = ei.locked = ei.onlyFirst
    =  ei.onlyLast = ei.flip = ei.noIcon = 0;

  ob->getData( "vis_x", &ei.vis_x );
  ob->getData( "vis_y", &ei.vis_y );
  ob->getData( "ord", &ei.ord );
  ob->getData( "locked", &ei.locked );
  ob->getData( "onlyFirst", &ei.onlyFirst );
  ob->getData( "onlyLast", &ei.onlyLast );
  ob->getData( "flip", &ei.flip );
  ob->getData( "noIcon", &ei.noIcon );
  ei.flip_factor = ei.flip ? 1 : -1;
  ei.n_inp = ob->inputsCount();

  ei.xs0 = lm + ei.vis_x * grid_sz;
  ei.ys0 = lm + ei.vis_y * grid_sz;
  ei.xs = ei.xs0 + el_marg;
  ei.ys = ei.ys0 + el_marg;
  ei.xc = ei.xs + obj_sz/2;
  ei.yc = ei.ys + obj_sz/2;
  ei.li_dst_x = ei.xc + ei.flip_factor*(obj_sz/2);
  ei.pre_dst_x = ei.li_dst_x + el_marg * ei.flip_factor;
  ei.li_src_x = ei.xc - ei.flip_factor*(obj_sz/2);
  ei.li_pdst_y = ei.yc + (obj_sz/2);

  ei.pis = ob->getElemT<const InputParams*>("pis");
  ei.n_pinp = ei.pis->size();

  return true;
}

void StructView::drawAll( QPainter &p )
{
  int h, w, nh, nw;
  int line_busy;
  int li_src_y, li_dst_y;
  int st_y; /* label on elems start y */
  QString src_name;
  h = height(); w = width(); nh = 1 + h / grid_sz; nw = 1 + w / grid_sz;
  if( !sch ) {
    p.setBrush( Qt::red );
    p.drawRect( 0, 0, w, 8 );
    return;
  };

  Mo2Settings *psett = LaboWin::win()->getSettings();
  int s_icons = psett->showicons;
  const QFont &strf = LaboWin::win()->getStructFont();
  p.setFont( strf );
  const QFont &smlf = LaboWin::win()->getSmallFont();
  QFontMetrics small_fm( smlf );
  em_small = small_fm.width( 'W' );
  ex_small = small_fm.height();
  el_marg = (grid_sz-obj_sz) / 2;
  if( nh >= MODEL_MY ) { nh = MODEL_MY-1; };
  if( nw >= MODEL_MX ) { nh = MODEL_MX-1; };


  // if( isActiveWindow() ) {
  p.setBrush( Qt::white );
  // } else {
  //   p.setBrush( QColor( 200, 200, 200 ) );
  // }
  p.drawRect( 0, 0, w, h );

  // ---------- draw grid
  if( psett->showgrid ) {
    p.setPen( QPen(QColor(190,210,210), 0, Qt::DotLine ) ); // TODO: config
    for( int i=0; i<nw; i++ ) {
      int x = lm + i*grid_sz;
      p.drawLine( x, tm, x, h );
    }
    for( int i=0; i<nh; i++ ) {
      int y = tm + i*grid_sz;
      p.drawLine( lm, y, w, y );
    }
  };

  p.setPen( Qt::black );
  // --------- draw elems ---------------------
  int cr_diff = el_marg/3;
  QPoint p_crp {  cr_diff, cr_diff }; // difs for cross and arrow
  QPoint p_crm { -cr_diff, cr_diff };

  ElemInfo ei, sei;
  for( auto ob : sch->TCHILD(TMiso*) ) {
    if( ! fill_elmInfo( ob, ei ) ) {
      qWarning() << "Fail fo fill info for " << ob->getFullName() << WHE;
      continue;
    }
    line_busy = 0;
    if( ei.vis_x < 0 || ei.vis_y < 0  ) {
      continue;
    }
    p.setPen( Qt::black );

    // --------------------- draw element icon or frame
    if( ei.noIcon || !s_icons ) {
      p.setPen( QPen(Qt::black,2) );  p.setBrush( Qt::NoBrush );
      p.drawRect( ei.xs, ei.ys, obj_sz, obj_sz );
    } else {
      QIcon el_ico = ob->getIcon();
      el_ico.paint( &p, ei.xs, ei.ys, obj_sz, obj_sz );
    };
    p.setPen( Qt::black );  p.setBrush( Qt::red );

    // special marks: TODO: icons
    if( ei.locked ) {
      p.drawRect( ei.xs + 4, ei.ys + 8, 20, 8 );
    }
    if( ei.onlyFirst ) {
      p.drawRect( ei.xs + 4, ei.ys + 14, 6, 6 );
    }
    if( ei.onlyLast ) {
      p.drawRect( ei.xs + 20, ei.ys +14, 6, 6 );
    }


    // order mark
    if( psett->showord ) {
      if( s_icons && ! ei.noIcon )  {
        p.setPen( Qt::NoPen ); p.setBrush( QColor(240,240,255) );
        p.drawRect( ei.xs, ei.ys, obj_sz-1, ex_small );
      };
      p.setPen( Qt::black );
      p.drawText( ei.xs + 2, ei.ys + ex_small, QSN( ei.ord ) );
      line_busy++;
    };
    // --------------------- draw element name
    if( psett->shownames || ei.noIcon || !s_icons ) {
      st_y = ei.ys + line_busy*ex_small;
      if( s_icons && ! ei.noIcon )  {
        p.setPen( Qt::NoPen ); p.setBrush( QColor(255,255,225) );
        p.drawRect( ei.xs, st_y, obj_sz-1, ex_small );
      };
      p.setPen( Qt::black );
      p.drawText( ei.xs + 2, st_y+ex_small, ei.name );
      line_busy++;
    };

    int mod = ob->getModified(); // ---------- modification marks
    if( mod & modifManual ) {
      p.setPen( Qt::NoPen );  p.setBrush( QColor( 255, 150, 150, 128 ) );
      p.drawRect( ei.xs, ei.ys+1, obj_sz-3, obj_sz/2 );
    }
    if( mod & modifAuto ) {
      p.setPen( Qt::NoPen );  p.setBrush( QColor( 255, 255, 100, 128 ) );
      p.drawRect( ei.xs+1, ei.ys+obj_sz/2, obj_sz-3, obj_sz/3 );
    }
    // TODO: vector icon here
    p.setBrush( Qt::NoBrush ); p.setPen( QPen(Qt::black,1) );
    st_y = ei.ys + line_busy*ex_small;




    if( ! psett->showLinks ) {
      continue;
    }

    //  input marks
    int in_sep_sz = obj_sz/(ei.n_inp+1);
    p.setFont( smlf );
    p.setBrush( Qt::NoBrush );

    // ordinary inputs
    for( int i_in=0; i_in < ei.n_inp; ++i_in ) {
      const InputSimple *in = ob->getInput( i_in );
      if( ! in ) {
        continue;
      }
      ltype_t lt = in->getLinkType();
      const TDataSet* sobj = in->getSourceObj();
      li_dst_y = ei.ys + (i_in+1)*in_sep_sz;
      int line_width = in->getDataD( "line_w", 1 );
      int x_shift    = in->getDataD( "x_shift", 0 );
      int y_shift    = in->getDataD( "y_shift", 0 );
      QColor lco = in->getDataD( "line_color", Qt::black );
      p.setPen( QPen( lco, line_width ) );

      if( lt == LinkNone ) {
        p.drawEllipse( QPoint(ei.li_dst_x, li_dst_y), el_marg/3, el_marg/3 );
        continue;
      }
      int x_vert = ei.pre_dst_x + x_shift * ei.flip_factor;
      // arrow
      // p.drawLine( pre_dst_x, li_dst_y,   li_dst_x, li_dst_y  );
      p.drawLine( ei.li_dst_x, li_dst_y, x_vert, li_dst_y );
      p.drawLine( ei.li_dst_x +  3*ei.flip_factor, li_dst_y-2, ei.li_dst_x, li_dst_y  );
      p.drawLine( ei.li_dst_x +  3*ei.flip_factor, li_dst_y+2, ei.li_dst_x, li_dst_y  );

      QString lbl = in->getDataD( "label", QString() );
      if( ! lbl.isEmpty() ) {
        p.drawText( x_vert-2*ei.flip_factor, li_dst_y-2, lbl );
      }

      if( lt == LinkBad ) {
        p.setPen( QPen( Qt::red, 2 ) );
        p.drawLine( x_vert-el_marg/3, li_dst_y-el_marg/3,
                    x_vert+el_marg/3, li_dst_y+el_marg/3 );
        p.drawLine( x_vert-el_marg/3, li_dst_y+el_marg/3,
                    x_vert+el_marg/3, li_dst_y-el_marg/3 );
        continue;
      }

      if( lt == LinkSpec ) {
        p.setPen( QPen( Qt::magenta, 2 ) );
        p.drawRect( x_vert-el_marg/4, li_dst_y-el_marg/4, el_marg/2, el_marg/2 );
        continue;
      }

      // here must be ordinary sources -> large mark if not so
      const TMiso *so_obj = nullptr;
      if( !sobj || ( (so_obj = qobject_cast<const TMiso*>(sobj)) == nullptr ) ) {
        p.setPen( QPen( Qt::red, 4 ) );
        p.drawEllipse( QPoint(x_vert, li_dst_y), el_marg, el_marg );
        continue;
      }

      // get info about source and calc coords
      fill_elmInfo( so_obj, sei );
      QString so = in->getDataD( "source", QString() );

      if( ei.vis_y != sei.vis_y ) {
        li_src_y = sei.yc - y_shift;
      } else {
        li_src_y = li_dst_y - y_shift; // special case: one line
      }

      if( so.contains(".") ) { // complex - not 'out0' source
        QChar qshc = (so.right(1))[0];
        char shc = qshc.toLatin1();
        li_src_y += 2 + ( shc  & 0x07 );
        p.drawLine( sei.li_src_x + el_marg*sei.flip_factor/2, li_src_y,
                    sei.li_src_x + el_marg*sei.flip_factor/2, li_src_y+3 );
      }

      p.drawLine( sei.li_src_x, li_src_y,
                  sei.li_src_x + el_marg*sei.flip_factor, li_src_y );

      int only_lbl = in->getDataD( "onlyLabel", 0 );
      if( only_lbl ) {
        if( ! lbl.isEmpty() ) {
          p.drawText( sei.li_src_x+(3+el_marg)*sei.flip_factor, li_src_y-2, lbl );
        }
        continue;
      }

      p.drawLine( x_vert, li_dst_y, x_vert, li_src_y ); // vertical part
      p.drawLine( x_vert, li_src_y, sei.li_src_x, li_src_y); // horiz. from src

    } // end of simple input loop


    // ----------- parametric inputs
    int i_in=0;
    in_sep_sz = obj_sz/(ei.n_pinp+1);
    for( const auto ips : ei.pis->TCHILD(const InputParam*) ) { // const? TODO: check

      int line_width = ips->getDataD( "line_w", 2 );
      int x_shift = ips->getDataD( "x_shift", 0 );
      int y_shift = ips->getDataD( "y_shift", 0 );
      QColor lco = ips->getDataD( "line_color", Qt::black );
      p.setPen( QPen( lco, line_width ) );

      int li_pdst_x = ei.xs + (1+i_in) * in_sep_sz;
      QPoint p_bott { li_pdst_x, ei.li_pdst_y+el_marg };
      QPoint p_dst  { li_pdst_x, ei.li_pdst_y };

      // arrow
      p.drawLine( p_bott, p_dst );
      p.drawLine( p_dst+p_crp, p_dst );
      p.drawLine( p_dst+p_crm, p_dst );

      ltype_t lt = ips->getLinkType();
      if( lt == LinkNone ) {
        // unlike signal input, must be unlikly
        p.drawEllipse( p_bott, el_marg/3, el_marg/3 );
        continue;
      }

      if( lt == LinkBad ) {
        p.setPen( QPen( Qt::red, 2 ) );
        p.drawLine( p_bott+p_crp, p_bott-p_crp );
        p.drawLine( p_bott+p_crm, p_bott-p_crm );
        continue;
      }

      if( lt == LinkSpec ) {
        p.setPen( QPen( Qt::magenta, 2 ) );
        p.drawRect( QRect( p_bott, QSize(el_marg/2, el_marg/2) ) );
        continue;
      }

      // must be ordinary link
      const TDataSet* sobj = ips->getSourceObj();
      const TMiso *so_obj = nullptr;
      if( !sobj || ( (so_obj = qobject_cast<const TMiso*>(sobj)) == nullptr ) ) {
        p.setPen( QPen( Qt::red, 4 ) );
        p.drawEllipse( p_bott, el_marg, el_marg );
        continue;
      }

      fill_elmInfo( so_obj, sei );

      li_src_y = sei.yc - y_shift;

      // TODO: label

      int x_vert = sei.li_src_x - ( x_shift + el_marg ) * sei.flip_factor;

      p.drawLine( sei.li_src_x, li_src_y, x_vert, li_src_y ); // from src
      p.drawLine( x_vert, ei.li_pdst_y+el_marg,  x_vert, li_src_y ); // vert line
      p.drawLine( p_bott, QPoint( x_vert, ei.li_pdst_y+el_marg ) );  // to dst.bottom

      ++i_in;
    } // end of param input loop

  }; // end loop on elems
  p.setPen( Qt::black );
  p.setFont( smlf );

  // -------------- output marks

  if( oview ) {
    ContOut *outs = qobject_cast<ContOut*>( oview->model() );
    if( !outs ) {
      qWarning() << "not found 'outs' in model" << WHE;
      return;
    }
    auto sel_mod = oview->currentIndex();
    int sel_out = sel_mod.row();
    for( auto arr : outs->TCHILD(TOutArr*) ) {
      int out_nu = arr->getMyIndexInParent();

      src_name = arr->getDataD( "name", QString() );
      int out_tp = arr->getDataD( "type", -1 );
      ltype_t lt  = LinkBad;
      const TDataSet *lob = nullptr;
      const double *fp = sch->getDoublePtr( src_name, &lt, &lob );

      if( !fp || lt != LinkElm || !lob ) {
        continue;
      }
      const TMiso *src_obj = qobject_cast<const TMiso*>(lob);
      if( ! src_obj ) {
        continue;
      }
      fill_elmInfo( src_obj, sei );

      if( sei.vis_x < 0 || sei.vis_y < 0 ) {
        continue;
      }

      if( sel_out == out_nu ) {
        p.setPen( QPen(QColor(100,100,255), 3, Qt::SolidLine ) );
        p.setBrush( Qt::NoBrush );
        p.drawRect( sei.xs0+lm-1, sei.ys0+tm-1, obj_sz+4, obj_sz+4 );
      }
      p.setPen( Qt::black );

      switch( out_tp ) {
        case 0: p.setBrush( Qt::white ); break; // TODO: named colors
        case 1: p.setBrush( Qt::green ); break;
        case 2: p.setBrush( Qt::cyan ); break;
        case 3: p.setBrush( Qt::gray ); break;
        default: p.setBrush( Qt::red ); break;
      };
      int l_out_nu = 1 + em_small * ( 1 + (int)log10( out_nu + 0.7 ) );
      int omark_x = sei.xs0 + obj_sz - l_out_nu  - 2 * ( out_nu & 7 );
      int omark_y = sei.ys0 +  1;
      p.drawRect( omark_x, omark_y, l_out_nu, ex_small );
      if( src_name.contains('.') ) { // inner link mark
        p.setBrush( Qt::red );
        p.drawRect( omark_x, omark_y+9, l_out_nu, 2 );
      }
      p.drawText( omark_x+2, omark_y+9,  QSN( out_nu ) );

    } // ------------ end output marks
  }


  // ----------- draw selection
  if( devTp != 1 ) {
    QPainter::CompositionMode old_op = p.compositionMode();
    p.setCompositionMode( QPainter::RasterOp_SourceXorDestination );
    p.setPen( Qt::NoPen );
    p.setBrush( QColor(64,64,32) );
    p.drawRect( lm + sel_x*grid_sz, tm + sel_y*grid_sz, grid_sz, grid_sz );
    p.setCompositionMode( old_op );
  };

  // ----------- draw mark;
  if( markObj ) { // red rect around marked element
    if( fill_elmInfo( markObj, ei ) ) {
      p.setBrush( Qt::NoBrush ); p.setPen( Qt::red );
      p.drawRect( ei.xs0, ei.ys0, grid_sz, grid_sz );
    }
  };

}

int StructView::checkState( CheckType ctp )
{
  QString msg;
  int state;
  if( !sch ) {
    handleError( this, "Scheme is absent!" );
    return 0;
  };
  switch( ctp ) {
    case validCheck: break;
    case selCheck:
                     if( !selObj )
                       msg = "You must select object to do this";
                     break;
    case linkToCheck:
                     if( !selObj || !markObj || level < 0  || level >=4 )
                       msg = "You need selected and marked objects to link";
                     break;
    case noselCheck: if( selObj != nullptr )
                       msg = "You heed empty sell to do this";
                     break;
    case moveCheck: if( selObj != nullptr || !markObj )
                      msg = "You need marked object and empty cell to move";
                    break;
    case doneCheck:
                    state = sch->getState();
                    if( state < stateDone ) {
                      msg = QString("Nothing to plot: state '%1', not 'Done' !").arg(
                          getStateString(state)  );
                    };
                    break;
    default: msg = "Unknown check?";
  };
  if( ! msg.isEmpty() ) {
    handleWarn( this, msg );
    return 0;
  };
  return 1;
}

QString StructView::getSchemeName() const
{
  if( !sch ) {
    return "? No Scheme! ?";
  }
  return sch->getFullName();
}

// ==== element related

void StructView::newElm()
{
  if( !checkState( noselCheck ) ) {
    return;
  }

  // sch->setParm( "bad_types", "TLorenz" ); // to test bad
  QString objName;
  QString tp = SelectTypeDialog::getTypeAndName( sch, this, objName, "TMiso" );
  if( tp.isEmpty() ) {
    return;
  }

  int hint_order = sch->hintOrd(); // TODO: to dialog?
  bool ok;
  int order = QInputDialog::getInt( this, "New element order",
      "Input element order",  hint_order, 0, IMAX, 1, &ok );
  if( !ok ) {
    return;
  }

  // addElemInfo aei;
  // aei.name = QSL("obj_")+ QSN( sch->countElemsOfType( QSL("TMiso"), QSL("obj_") ) );
  // aei.order = sch->hintOrd();
  // auto dia = new AddElemDialog( &aei, sch, this, "TMiso" );
  //                                         // limit to such elements here
  //
  // int rc = dia->exec();
  // delete dia; dia = 0;
  //
  // if( rc != QDialog::Accepted || aei.type.isEmpty() ) {
  //   return;
  // }
  // if( ! isGoodName( objName )  ) {
  //   handleError( this, QString("Fail to add Elem: bad object name \"%1\"").arg(objName) );
  //   return;
  // }

  TMiso *ob = sch->insElem( tp, objName, order, sel_x, sel_y );
  if( !ob  ) {
    handleError( this, QString("Fail to add Elem: type \"%1\" \"%2\"").arg(tp).arg(objName) );
    return;
  }
  changeSel( 0, 0, 1 ); // update sel
  editElm();
}

void StructView::delElm()
{
  if( ! checkState( selCheck ) )
    return;

  QString oname = selObj->objectName();

  bool sel_is_mark = ( selObj == markObj );

  if( confirmDelete( this, "element", oname) ) {
    sch->delElem( oname );
    if( sel_is_mark ) {
      markObj = nullptr;
    }

    changeSel( 0, 0, 1 ); // update sel
    emit viewChanged();
  };
}

void StructView::editElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }
  bool ok = editObj( this, selObj );
  if( ok ) {
    update();
    // model->reset();
    emit viewChanged();
  }
}

void StructView::renameElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  QString old_name = selObj->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + selObj->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( ok ) {
    if( sch->rename_obj( old_name, new_name ) ) {
      // model->reset();
      emit viewChanged();
    }
  }

  // TODO: change links named
}


void StructView::qlinkElm()
{
  QString toname;
  if( ! checkState( linkToCheck ) ) {
    return;
  }

  if( !selObj || !markObj ) {
    return;
  }
  toname = markObj->objectName();

  // TODO: its model action. really? model dont know about selected and marked.

  InputSimple *in = selObj->getInput( level );
  if( ! in ) {
    return;
  }
  if( ! in->setData( "source", toname ) ) {
    qWarning() << "fail to set source " << toname << " in " << in->getFullName() << WHE;
    return;
  }
  sch->reportStructChanged();
  sch->reset();
  emit viewChanged();
}


void StructView::qplinkElm()
{
  QString oldlink;
  if( ! checkState( linkToCheck ) ) {
    return;
  }

  if( !selObj || !markObj  ) {
    return;
  }

  QString toname = markObj->objectName();

  InputParams *pis = selObj->getElemT<InputParams*>("pis");
  if( !pis ) {
    qWarning() << "no pis object in " <<  selObj->getFullName() << WHE;
    return;
  }
  int n_pi = pis->size();
  QString pi_name = QString("pi_") + QSN(n_pi);
  InputParam *pi = pis->addObj<InputParam>( pi_name );
  if( !pi ) {
    return;
  }

  pi->setData( "source", toname );
  pi->setData( "line_w", 2 );
  pi->setData( "line_color", "red" );

  sch->reportStructChanged();
  sch->reset();
  emit viewChanged();
}

void StructView::unlinkElm()
{
  if( ! checkState( selCheck ) ) { // no need marked to unlink
    return;
  }

  QString lnkname;
  QString empty_str("");
  int ni = selObj->inputsCount();
  for( int i=0; i<ni; ++i ) {
    InputSimple *in = selObj->getInput( i );
    if( ! in ) {
      continue;
    }
    in->setData( "source", empty_str );
  }

  InputParams *pis = selObj->getElemT<InputParams*>("pis");
  if( !pis ) {
    qWarning() << "no pis object in " <<  selObj->getFullName() << WHE;
    return;
  }
  qDeleteAll( pis->children() );

  sch->reportStructChanged();
  sch->reset();
  emit viewChanged();
}

void StructView::lockElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  int lck = selObj->getDataD( "locked", 0 );
  lck = !lck;
  selObj->setData( "locked", lck );

  sch->reset();
  emit viewChanged();
}

void StructView::ordElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  bool ok;
  int old_ord = selObj->getDataD( "ord", 0 );
  int new_ord = QInputDialog::getInt( this, "New element order",
      "Input new element order",  old_ord, 0, IMAX, 1, &ok );
  if( ok ) {
    sch->newOrder( selObj->objectName(), new_ord ); // reset implied
    emit viewChanged();
  };
}

void StructView::markElm()
{
  markObj = selObj;
  emit viewChanged();
}

void StructView::moveElm()
{
  if( ! checkState( moveCheck )  ||  !markObj  ) {
    return;
  }

  sch->moveElem( markObj->objectName(), sel_x, sel_y );
  changeSel( 0, 0, 1 ); // update sel
  emit viewChanged();
}

void StructView::infoElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Structure of ") + selObj->getFullName() );

  auto lay = new QVBoxLayout();

  auto tv = new QTableWidget( 100, 6, dia );
  QStringList hlabels;
  hlabels << "Name" << "Type" << "Value" << "Descr" << "Target"<< "Flags";
  tv->setHorizontalHeaderLabels( hlabels );

  QObjectList childs = selObj->children();

  int i = 0;
  for( auto o :  childs ) {
    QObject *ob = o;
    tv->setItem( i, 0, new  QTableWidgetItem( ob->objectName() ) );
    if( ob->inherits("TDataSet" ) ) {
      TDataSet *ds = qobject_cast<TDataSet*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ds->getType()) );
      tv->setItem( i, 2, new QTableWidgetItem( ds->toString() ) );
    } else if( ob->inherits("HolderData" ) ) {
      HolderData *ho = qobject_cast<HolderData*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ho->getType() ) );
      tv->setItem( i, 2, new QTableWidgetItem(ho->toString() ) );
      tv->setItem( i, 3, new QTableWidgetItem(ho->getParm("vis_name") + " \""
                    + ho->getParm("descr" ) + "\"" ) );
      tv->setItem( i, 4, new QTableWidgetItem( ho->objectName() ) );
      tv->setItem( i, 5, new QTableWidgetItem( flags2str(ho->getFlags()) ) );

    } else { // unknown
      tv->setItem( i, 1, new QTableWidgetItem("???unknown???" ) );
      tv->setItem( i, 2, new QTableWidgetItem(ob->metaObject()->className() ) );
    }
    ++i;
  }



  lay->addWidget( tv );

  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 72*em, 50*em ); // TODO: adjust to inner table width
  dia->exec();
  delete dia;
  emit viewChanged();
}

void StructView::showTreeElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Element tree: ") + selObj->objectName() );

  auto lay = new QVBoxLayout();


  auto treeView = new QTreeView( dia );
  treeView->setModel( selObj );
  lay->addWidget( treeView );


  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 86*em, 50*em ); // TODO: unmagic
  treeView->setColumnWidth( 0, 35*em );
  treeView->setColumnWidth( 1, 10*em );
  treeView->setColumnWidth( 2, 35*em );
  treeView->setColumnWidth( 3,  6*em );
  treeView->expandAll();
  dia->exec();
  delete dia;
  emit viewChanged();
  return;
}


void StructView::testElm1()
{
  QString buf;
  if( ! checkState( selCheck ) )
    return;

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": test1 ") + selObj->objectName() );

  buf = selObj->toString();


  auto lay = new QVBoxLayout();

  auto la = new QLabel( dia );
  la->setText( buf );
  auto scroll = new QScrollArea( dia );
  scroll->setWidget( la );
  lay->addWidget( scroll );


  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 60*em, 30*em ); // TODO: unmagic
  dia->exec();
  delete dia;
  emit viewChanged();
}

void StructView::testElm2()
{
  if( ! checkState( selCheck ) ) {
    return;
  }
  if( selObj == 0 ) {
    return;
  }
  // place for action here

  return;
}

void StructView::cutElm()
{
  copyElm();
  delElm();
}

void StructView::copyElm()
{
  if( !selObj ) {
    return;
  }
  QString s = selObj->toString();
  QClipboard *clp = QApplication::clipboard();
  if( clp ) {
    clp->setText( s );
  }
}

void StructView::pasteElm()
{
  if( selObj ) {
    return;
  }
  QClipboard *clp = QApplication::clipboard();
  if( !clp ) {
    return;
  }
  QString s = clp->text();
  int err_line, err_column;
  QString errstr;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    handleWarn( this, tr("Cannot parse clipboard string:\n%2\nLine %3 column %4.")
                .arg(errstr).arg(err_line).arg(err_column) );
    return;
  }
  QDomElement ee = x_dd.documentElement();

  QString tagname = ee.tagName();
  if( tagname != "obj" ) {
    handleWarn( this, tr("element tag is not 'obj':  %2").arg( tagname ) );
    return;
  }

  QString eltype = ee.attribute( "otype" );
  QString elname = ee.attribute( "name" );
  elname = sch->hintName( eltype, elname );

  int oord = sch->hintOrd();

  auto dia = new QDialog( this );
  auto lay = new QGridLayout( dia );

  auto la_name = new QLabel( "&Name", dia );
  lay->addWidget( la_name, 0, 0 );

  auto oname_ed = new QLineEdit( dia );
  oname_ed->setText( elname );
  oname_ed->setFocus();
  lay->addWidget( oname_ed, 1, 0  );

  auto la_ord = new QLabel( "&Order", dia );
  lay->addWidget( la_ord, 0, 1 );

  auto oord_ed = new QLineEdit( dia );
  oord_ed->setText( QSN(oord) );
  lay->addWidget( oord_ed, 1, 1 );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  elname = oname_ed->text();
  oord = oord_ed->text().toInt();
  delete dia;

  if( rc != QDialog::Accepted ) {
    return;
  };

  if( ! isGoodName( elname )  ) {
    handleError( this, QString("Fail to add Elem: bad object name \"%1\"").arg(elname) );
    return;
  }

  TMiso *ob = sch->insElem( eltype, elname, oord, sel_x, sel_y) ; // reset() implied
  if( !ob  ) {
    handleError( this, QString("Fail to add Elem: %1 %2").arg(eltype).arg(elname) );
    return;
  }
  if( !ob->fromDom( ee, errstr ) ) {
    handleWarn( this, tr("fail to set params:  %1").arg( errstr ) );
  }
  ob->setData( "vis_x", sel_x );
  ob->setData( "vis_y", sel_y );
  ob->setData( "ord", oord );
  changeSel( 0, 0, 1 ); // update sel

}



void StructView::mousePressEvent( QMouseEvent *me )
{
  int h, w, nh, nw, ex, ey, x, y;
  TMiso *ob = 0;
  QString elmname;
  if( !sch ) {
    return;
  }
  h = height(); w = width(); nh = h / grid_sz - 1; nw = w / grid_sz - 1;
  x = me->x(); y = me->y();
  ex = ( x - lm ) / grid_sz; ey = ( y - tm ) / grid_sz;
  if( ex < 0 || ex >= nw || ey < 0 || ey >= nh ) {
    return;
  }

  changeSel( ex, ey, 0 );
  ob = sch->xy2Miso( ex, ey );
  if( ob ) {
    elmname = ob->getFullName();
    // double outval = ob->getDataD( "out0", 0.0 );
    if( elmname.isEmpty() ) {
      elmname = "?unknown?";
    }
  };

  switch( me->button() ) {
    case Qt::LeftButton:
                break;
    case Qt::RightButton:
                {
                QMenu *menu = createPopupMenu( elmname, ob != nullptr );
                menu->exec( mapToGlobal(QPoint( x, y )) );
                delete menu;
                }
                break;
    case Qt::MidButton:
                editElm();
                break;
    default:
                break;// none
  };

}

QMenu* StructView::createPopupMenu( const QString &title, bool has_elem )
{
  auto menu = new QMenu( this );
  QAction *act;
  (void) menu->addSeparator();
  if( title.isEmpty() ) {
    (void) menu->addSection( "  ** Empty **  ");
  } else {
    (void) menu->addSection( title );
  }
  if( has_elem ) {
    menu->addSeparator();
    act = menu->addAction( QIcon( ":icons/editelm.png" ), "&Edit element" );
    connect( act, &QAction::triggered, this, &StructView::editElm );
    act = menu->addAction( QIcon( ":icons/delelm.png" ), "&Delete element" );
    connect( act, &QAction::triggered, this, &StructView::delElm );
    menu->addSeparator();
    act = menu->addAction( "Rename element" );
    connect( act, &QAction::triggered, this, &StructView::renameElm );
    menu->addSeparator();
    act = menu->addAction( QIcon::fromTheme("insert-link"), "&Link" );
    connect( act, &QAction::triggered, this, &StructView::qlinkElm );
    act = menu->addAction(  QIcon( ":icons/orderelm.png" ), "&Reorder" );
    connect( act, &QAction::triggered, this, &StructView::ordElm );
    menu->addSeparator();
    act = menu->addAction( QIcon::fromTheme("edit-copy"), "&Copy" );
    connect( act, &QAction::triggered, this, &StructView::copyElm );
    menu->addSeparator();
  } else {
    act = menu->addAction(  QIcon( ":icons/newelm.png" ), "&New element" );
    connect( act, &QAction::triggered, this, &StructView::newElm );
    if( markObj ) {
      act = menu->addAction( "&Move to" );
      connect( act, &QAction::triggered, this, &StructView::moveElm );
    }
    act = menu->addAction( QIcon::fromTheme("edit-pase"), "&Paste" );
    connect( act, &QAction::triggered, this, &StructView::pasteElm );
  };
  if( oview != nullptr && mainview != nullptr ) {
    menu->addSeparator(); //TODO: revive if need
    act = menu->addAction( QIcon( ":icons/newout.png" ), "New outp&ut" );
    connect( act, &QAction::triggered, mainview, &LaboView::newOut );
    menu->addSeparator();
    act = menu->addAction( QIcon( ":icons/editmodel.png" ), "Edit model" );
    connect( act, &QAction::triggered, mainview, &LaboView::editModel );
  }
  menu->addSeparator();
  act = menu->addAction( QIcon::fromTheme("document-print"),"Print model" );
  connect( act, &QAction::triggered, this, &StructView::print );

  return menu;
}

void StructView::mouseDoubleClickEvent( QMouseEvent * /*me*/ )
{
  editElm();
}


void StructView::keyPressEvent( QKeyEvent *ke )
{
  int k, /*h, w, nh, nw,*/ st, btnShift, /*btnCtrl,*/ xy_delta;
  k = ke->key(); st = ke->modifiers();
  btnShift = ( ( st & Qt::ShiftModifier ) != 0 );
  QString title;
  if( selObj ) {
    title = selObj->getFullName();
  }
  // btnCtrl = ( ( st & Qt::ControlModifier ) != 0 );
  // h = height();
  // w = width();
  // nh = h / grid_sz - 1;
  // nw = w / grid_sz - 1;
  xy_delta = btnShift ? 5 : 1;
  switch( k ) {
    case Qt::Key_Return: editElm(); break; // to catch both keys
    case Qt::Key_Home:  changeSel( 0, 0, 0 ); break;
    case Qt::Key_Left:  changeSel( -xy_delta, 0, 1 );  break;
    case Qt::Key_Right: changeSel( xy_delta, 0, 1 );  break;
    case Qt::Key_Up:    changeSel( 0, -xy_delta, 1 );  break;
    case Qt::Key_Down:  changeSel( 0, xy_delta, 1 );  break;
    case Qt::Key_N:     changeSel( 1, 0, 2 ); break;
    case Qt::Key_0:     changeLevel(0); break;
    case Qt::Key_1:     changeLevel(1); break;
    case Qt::Key_2:     changeLevel(2); break;
    case Qt::Key_3:     changeLevel(3); break;
    case Qt::Key_4:     changeLevel(4); break;
    case Qt::Key_5:     changeLevel(5); break;
    case Qt::Key_6:     changeLevel(6); break;
    case Qt::Key_7:     changeLevel(7); break;
    case Qt::Key_8:     changeLevel(8); break;
    case Qt::Key_9:     changeLevel(9); break;

    case Qt::Key_F10:
            {
              QMenu* menu = createPopupMenu( title, selObj != nullptr ); // FIXME: use
              menu->exec( mapToGlobal( getSelCoords() )  );
              delete menu;
            }
            break;
    default: ke->ignore();
  };
}

// ============================== StructSubwin =======================================

StructSubwin::StructSubwin( QWidget *a_par, LaboDoc *a_doc, Scheme *a_sch,  LaboView *mview, OutDataView *a_oview )
    : CommonSubwin( a_par, a_doc, a_sch->objectName() )
{
  main_win = false;
  title_prefix = QSL("scheme");

  scrollArea = new QScrollArea( this );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );

  sview = new StructView( this, a_sch, mview, a_oview );

  scrollArea->setWidget( sview );
  setCentralWidget( scrollArea );
}

StructSubwin::~StructSubwin()
{
}

bool StructSubwin::callSlot( const char *nm )
{
  return QMetaObject::invokeMethod( sview, nm, Qt::AutoConnection );
}

bool StructSubwin::checkSlot( const char *nm )
{
  return checkSlotSub( sview, nm );
}



// end of structview.cpp


