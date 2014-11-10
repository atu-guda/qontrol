#include <QtWidgets>

#include "laboview.h"

class SimulView : public QListView {
  public:
   SimulView( HolderData *a_mod, LaboView *par );
  private:
   HolderData *mod;
   LaboView *laboview;
};
