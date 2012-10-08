#include "rootana/Cut.hxx"
namespace rootana {

struct Cut0: public rootana::Cut { bool operator() () const {
  return gHead.bgo.q[0] < 3000;
} };

struct Cut1: public rootana::Cut { bool operator() () const {
  return gHead.bgo.q[0] > 300;
} };

struct Cut2: public rootana::Cut { bool operator() () const {
  return gHead.bgo.q[1] > 3000;
} };

}
