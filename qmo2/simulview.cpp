#include "dataset.h"
#include "simulview.h"
#include "labowin.h"


SimulView::SimulView( HolderData *a_mod, LaboView *par )
  : QListView( par ), mod( a_mod ), laboview( par )
{
  setModel( mod );

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 255, 200, 200 ) );
  setPalette( s_pal );

  int em = LaboWin::labowin->getEm();
  setFixedWidth( 8*em );
}
