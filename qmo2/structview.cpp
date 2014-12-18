/***************************************************************************
                          structview.cpp  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2014 by atu
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



StructView:: StructView( Scheme *a_sch, LaboView *mview )
            : QWidget( mview ), sch( a_sch ), mainview( mview )
{
  devTp = 0;
  // grid_sz = 40;
  grid_sz = 46;
  lm = tm = 4;  obj_sz = 32;

  QPalette pal;
  pal.setBrush( QPalette::Window, QBrush( Qt::white ) );
  setPalette( pal );
  setAutoFillBackground(true);

  setMaximumSize( grid_sz*MODEL_MX, grid_sz*MODEL_MY );
  // setMinimumSize( grid_sz*MODEL_MX, grid_sz*MODEL_MY ); // debug
  setMinimumSize( grid_sz*8, grid_sz*6 );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
}

StructView::~StructView()
{

}

QSize StructView::getElemsBound() const
{
  int sel_x = mainview->getSelX();
  int sel_y = mainview->getSelY();
  QSize mss = sch->getMaxXY().expandedTo( QSize( sel_x, sel_y ) );
  mss += QSize( 3, 3 ); // free bound
  mss *= grid_sz;       // to grid scale

  return mss;
  // TODO: revive
  // QSize pas = mainview->svSize() - QSize(16,16);
  // if( mx > pas.width() ) 
  //   pas.setWidth( mx );
  // if( my > pas.height() ) 
  //   pas.setHeight( my );
  // return pas;
}

QSize StructView::sizeHint() const
{
  return getElemsBound();
}

QPoint StructView::getSelCoords() const
{
  int sel_x = mainview->getSelX();
  int sel_y = mainview->getSelY();
  return QPoint( sel_x * grid_sz + grid_sz/2, sel_y * grid_sz + grid_sz/2 );
}


void StructView::update()
{
  resize( getElemsBound() );
  QWidget::update();
}


void StructView::paintEvent( QPaintEvent * /*pe*/ )
{
  if( ! sch )
    return;
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

  QPrinter *pr = LaboWin::labowin->getPrinter();
  if( !pr ) {
    return;
  }

  QPrintDialog pr_dialog( pr, this );
  if( pr_dialog.exec() ) {
    devTp = 1;
    pr->setFullPage( false );
    pr->newPage();
    QPainter p( pr );
    drawAll( p );
    p.end();
  };
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
  int i, h, w, nh, nw;
  int line_busy;
  int li_src_y, li_dst_y;
  int st_y; /* label on elems start y */
  int sel_x, sel_y /*,sel, mark*/;
  QString src_name;
  TMiso *ob;
  TModel *model = sch->getAncestorT<TModel>();

  Mo2Settings *psett = LaboWin::labowin->getSettings();
  int s_icons = psett->showicons;
  const QFont &strf = LaboWin::labowin->getStructFont();
  p.setFont( strf );
  const QFont &smlf = LaboWin::labowin->getSmallFont();
  h = height(); w = width(); nh = 1 + h / grid_sz; nw = 1 + w / grid_sz;
  el_marg = (grid_sz-obj_sz)/2;
  sel_x = mainview->getSelX();
  sel_y = mainview->getSelY();
  if( nh >= MODEL_MY ) nh = MODEL_MY;
  if( nw >= MODEL_MX ) nh = MODEL_MX;

  if( ! sch ) {
    p.setBrush( Qt::red );
    p.drawRect( 0, 0, w, 8 );
    return;
  };

  // ---------- draw grid
  if( psett->showgrid ) {
    p.setPen( QPen(QColor(200,220,220), 0, Qt::DotLine ) ); // TODO: config
    for( i=0; i<nw; i++ )
      p.drawLine( lm + i*grid_sz, tm, lm+i*grid_sz, h );
    for( i=0; i<nh; i++ )
      p.drawLine( lm, tm+i*grid_sz, w, tm+i*grid_sz );
  };

  p.setPen( Qt::black );
  // --------- draw elems ---------------------
  int cr_diff = el_marg/3;
  QPoint p_crp {  cr_diff, cr_diff }; // difs for cross and arrow
  QPoint p_crm { -cr_diff, cr_diff };

  ElemInfo ei, sei;
  for( auto o : sch->children() ) {
    ob = qobject_cast<TMiso*>( o );
    if( !ob ) {
      continue;
    }

    if( ! fill_elmInfo( ob, ei ) )
      continue;
    line_busy = 0;
    if( ei.vis_x < 0 || ei.vis_y < 0  )
      continue;
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
    if( ei.locked )
      p.drawRect( ei.xs + 4, ei.ys + 8, 20, 8 );
    if( ei.onlyFirst )
      p.drawRect( ei.xs + 4, ei.ys + 14, 6, 6 );
    if( ei.onlyLast )
      p.drawRect( ei.xs + 20, ei.ys +14, 6, 6 );

    // order mark
    if( psett->showord ) {
      if( s_icons && ! ei.noIcon )  {
        p.setPen( Qt::NoPen ); p.setBrush( QColor(240,240,255) );
        p.drawRect( ei.xs, ei.ys, 31,11);
      };
      p.setPen( Qt::black );
      p.drawText( ei.xs + 2, ei.ys + 10, QSN(ei.ord) );
      line_busy++;
    };
    // --------------------- draw element name
    if( psett->shownames || ei.noIcon || !s_icons ) {
      st_y = ei.ys + line_busy*10;
      if( s_icons && ! ei.noIcon )  {
        p.setPen( Qt::NoPen ); p.setBrush( QColor(255,255,225) );
        p.drawRect( ei.xs, st_y, 31,11);
      };
      p.setPen( Qt::black );
      p.drawText( ei.xs + 2, st_y+10, ei.name );
      line_busy++;
    };
    // TODO: vector icon here
    p.setBrush( Qt::NoBrush ); p.setPen( QPen(Qt::black,1) );
    st_y = ei.ys + line_busy*10;




    if( ! psett->showLinks ) {
      continue;
    }

    //  input marks
    int in_sep_sz = obj_sz/(ei.n_inp+1);
    p.setFont( smlf );
    p.setBrush( Qt::NoBrush );

    // ordinary inputs
    for( int i_in=0; i_in < ei.n_inp; ++i_in ) {
      const InputSimple *in = ob->getInput(i_in);
      if( ! in )
        continue;
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
    for( const auto c : ei.pis->children() ) {
      const InputParam* ips = qobject_cast<const InputParam*>( c );
      if( ! ips ) {
        continue;
      }

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
  if( !model ) {
    DBG1( "warn: not found model" );
    return;
  }

  ContOut *outs = model->getElemT<ContOut*>( "outs" );
  if( !outs ) {
    DBG1( "warn: not found 'outs' in model" );
    return;
  }
  int out_nu = -1; // first vill be 0 (after ++)
  for( auto c : outs->children() ) {
    TOutArr *arr = qobject_cast<TOutArr*>( c );
    if( ! arr ) {
      continue;
    }
    ++out_nu;

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

    switch( out_tp ) {
      case 0: p.setBrush( Qt::white ); break;
      case 1: p.setBrush( Qt::green ); break;
      case 2: p.setBrush( Qt::cyan ); break;
      case 3: p.setBrush( Qt::gray ); break;
      default: p.setBrush( Qt::red ); break;
    };
    int omark_x = sei.xs0 + obj_sz - 10 - out_nu*2;
    int omark_y = sei.ys0 +  1;
    p.drawRect( omark_x, omark_y, 10, 10 );
    if( src_name.contains('.') ) { // inner link mark
      p.setBrush( Qt::red );
      p.drawRect( omark_x, omark_y+9, 10, 2 );
    }
    p.drawText( omark_x+2, omark_y+9,  QSN( out_nu ) );

  } // ------------ end output marks


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
  const TMiso *markObj = mainview->getMarkObj();
  if( markObj ) { // red rect around marked element
    if( fill_elmInfo( markObj, ei ) ) {
      p.setBrush( Qt::NoBrush ); p.setPen( Qt::red );
      p.drawRect( ei.xs0, ei.ys0, grid_sz, grid_sz );
    }
  };

}


void StructView::mousePressEvent( QMouseEvent *me )
{
  int h, w, nh, nw, ex, ey, x, y;
  QMenu *menu;
  TMiso *ob = 0;
  QString elmname;
  h = height(); w = width(); nh = h / grid_sz - 1; nw = w / grid_sz - 1;
  x = me->x(); y = me->y();
  ex = ( x - lm ) / grid_sz; ey = ( y - tm ) / grid_sz;
  if( ex >= 0 && ex <= nw && ey >=0 && ey <= nh ) {
    mainview->changeSel( ex, ey, 0 );
    ob = sch->xy2Miso( ex, ey );
    if( ob ) {
      elmname = ob->getFullName();
      // double outval = ob->getDataD( "out0", 0.0 );
      if( elmname.isEmpty() ) {
        elmname = "?unknown?";
      }
    };
    switch( me->button() ) {
      case Qt::LeftButton:  break;
      case Qt::RightButton:
            menu = createPopupMenu( elmname, ob != nullptr );
            menu->exec( mapToGlobal(QPoint( x, y )) );
            delete menu;
            break;
      case Qt::MidButton:   mainview->editElm();  break;
      default: break;// none
    };
  };
}

QMenu* StructView::createPopupMenu( const QString &title, bool has_elem )
{
  QMenu *menu = new QMenu( this );
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
    connect( act, &QAction::triggered, mainview, &LaboView::editElm );
    act = menu->addAction( QIcon( ":icons/delelm.png" ), "&Delete element" );
    connect( act, &QAction::triggered, mainview, &LaboView::delElm );
    menu->addSeparator();
    act = menu->addAction( QIcon::fromTheme("insert-link"), "&Link" );
    connect( act, &QAction::triggered, mainview, &LaboView::qlinkElm );
    act = menu->addAction(  QIcon( ":icons/orderelm.png" ), "&Reorder" );
    connect( act, &QAction::triggered, mainview, &LaboView::ordElm );
    menu->addSeparator();
    act = menu->addAction( QIcon::fromTheme("edit-copy"), "&Copy" );
    connect( act, &QAction::triggered, mainview, &LaboView::copyElm );
    menu->addSeparator();
  } else {
    act = menu->addAction(  QIcon( ":icons/newelm.png" ), "&New element" );
    connect( act, &QAction::triggered, mainview, &LaboView::newElm );
    if( mainview->getMarkObj() ) {
      act = menu->addAction( "&Move to" );
      connect( act, &QAction::triggered, mainview, &LaboView::moveElm );
    }
    act = menu->addAction( QIcon::fromTheme("edit-pase"), "&Paste" );
    connect( act, &QAction::triggered, mainview, &LaboView::pasteElm );
  };
  menu->addSeparator();
  act = menu->addAction( QIcon( ":icons/newout.png" ), "New outp&ut" );
  connect( act, &QAction::triggered, mainview, &LaboView::newOut );
  menu->addSeparator();
  act = menu->addAction( QIcon( ":icons/editmodel.png" ), "Edit model" );
  connect( act, &QAction::triggered, mainview, &LaboView::editModel );
  act = menu->addAction( QIcon::fromTheme("document-print"),"Print model" );
  connect( act, &QAction::triggered, mainview, &LaboView::print );

  return menu;
}

void StructView::mouseDoubleClickEvent( QMouseEvent * /*me*/ )
{
  mainview->editElm();
}


void StructView::keyPressEvent( QKeyEvent *ke )
{
  int k, /*h, w, nh, nw,*/ st, btnShift, /*btnCtrl,*/ xy_delta;
  k = ke->key(); st = ke->modifiers();
  btnShift = ( ( st & Qt::ShiftModifier ) != 0 );
  QMenu* menu;
  QString title;
  TMiso *ob = mainview->getSelObj();
  if( ob ) {
    title = ob->getFullName();
  }
  // btnCtrl = ( ( st & Qt::ControlModifier ) != 0 );
  // h = height();
  // w = width();
  // nh = h / grid_sz - 1;
  // nw = w / grid_sz - 1;
  xy_delta = btnShift ? 5 : 1;
  switch( k ) {
    case Qt::Key_Return: mainview->editElm(); break; // to catch both keys
    case Qt::Key_Home:  emit sig_changeSel( 0, 0, 0 ); break;
    case Qt::Key_Left:  emit sig_changeSel( -xy_delta, 0, 1 );  break;
    case Qt::Key_Right: emit sig_changeSel( xy_delta, 0, 1 );  break;
    case Qt::Key_Up:    emit sig_changeSel( 0, -xy_delta, 1 );  break;
    case Qt::Key_Down:  emit sig_changeSel( 0, xy_delta, 1 );  break;
    case Qt::Key_N:     emit sig_changeSel( 1, 0, 2 ); break;
    case Qt::Key_0:     emit sig_changeLevel(0); break;
    case Qt::Key_1:     emit sig_changeLevel(1); break;
    case Qt::Key_2:     emit sig_changeLevel(2); break;
    case Qt::Key_3:     emit sig_changeLevel(3); break;
    case Qt::Key_4:     emit sig_changeLevel(4); break;
    case Qt::Key_5:     emit sig_changeLevel(5); break;
    case Qt::Key_6:     emit sig_changeLevel(6); break;
    case Qt::Key_7:     emit sig_changeLevel(7); break;
    case Qt::Key_8:     emit sig_changeLevel(8); break;
    case Qt::Key_9:     emit sig_changeLevel(9); break;

    case Qt::Key_F10:
            menu = createPopupMenu( title, ob != nullptr ); // FIXME: use
            menu->exec( mapToGlobal( getSelCoords() )  );
            delete menu;
            break;
    default: ke->ignore();
  };
}

// end of structview.cpp


