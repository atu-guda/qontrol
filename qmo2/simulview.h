#include <QtWidgets>

#include "laboview.h"

class SimulView : public QListView {
  Q_OBJECT
  public:
   SimulView( HolderData *a_mod, LaboView *par );
  private:
   HolderData *mod;
   LaboView *laboview;
   QAction *act_new, *act_del, *act_edit, *act_setActive;
  private:
   void init_actions();
};
