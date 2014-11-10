#include "dataset.h"
#include "simulview.h"
#include "labowin.h"


SimulView::SimulView( HolderData *a_mod, LaboView *par )
  : QListView( par ), mod( a_mod ), laboview( par )
{

  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 255, 200, 200 ) );
  setPalette( s_pal );

  int em = LaboWin::labowin->getEm();
  setFixedWidth( 8*em );

  setContextMenuPolicy( Qt::ActionsContextMenu );

  setModel( mod );
}

void SimulView::init_actions()
{
  act_new = new QAction( QIcon::fromTheme("list-add"), "New", this );
  addAction( act_new );
  connect( act_new, SIGNAL(triggered()), laboview, SLOT(newSimul()) );

  act_del = new QAction( QIcon::fromTheme("list-remove"), "Delete", this );
  addAction( act_del );
  connect( act_del, SIGNAL(triggered()), laboview, SLOT(delSimul()) );

  act_edit = new QAction( QIcon::fromTheme("document-properties"), "Edit", this );
  addAction( act_edit );
  connect( act_edit, SIGNAL(triggered()), laboview, SLOT(editSimul()) );

  act_setActive = new QAction( QIcon::fromTheme("checkmark"), "set Active", this );
  addAction( act_setActive );
  connect( act_setActive, SIGNAL(triggered()), laboview, SLOT(setActiveSimul()) );
}
