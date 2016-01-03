/***************************************************************************
   autoact.h - autoactions classes
                             -------------------
    begin                : 2015.11.28
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <functional>

// helper class to auto decrement values at function exit
class AutoIncDec {
  public:
   explicit AutoIncDec( int &av ) : v(av) { ++v; }
   ~AutoIncDec() { --v; };
  private:
   int &v;
};

// a-la simplefied boost ScopeExit
class AtScopeExit {
  public:
   using ExitFunType = const std::function<void()>;
   explicit AtScopeExit( ExitFunType &a_action ) : action( a_action ) {};
   ~AtScopeExit() {  try {    action(); } catch(...) {}   }
  private:
    ExitFunType action;
  private:
    AtScopeExit( const AtScopeExit &r ) = delete;
    AtScopeExit& operator=( const AtScopeExit &rhs ) = delete;
};
