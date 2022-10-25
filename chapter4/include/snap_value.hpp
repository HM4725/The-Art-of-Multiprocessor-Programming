#pragma once

#include "array.hpp"


namespace stamp {
  // primitive value
  template <typename T>
  class SnapValue {
    private:
      long label;
      T value;
      Array<T> *snap;

    public:
      SnapValue() : label(0), value(0), snap(nullptr) {}
      SnapValue(long label, T init, Array<T> *snap) : \
          label(label), value(init), \
          snap(new Array<T>(*snap)) {}
      SnapValue(const SnapValue<T> *psv) :
          label(psv->label), value(psv->value), \
          snap(psv->snap == nullptr ? nullptr : new Array<T>(*(psv->snap))) {}
      ~SnapValue() {
        if(snap != nullptr) {
          delete snap;
        }
      }

      long getLabel() const {
        return label;
      }
      T getValue() const {
        return value;
      }
      Array<T>* stealSnap() {
        return new Array<T>(snap);
      }

      bool operator==(const SnapValue<T>& ss) const {
        return label==ss.label;
      }
      bool operator!=(const SnapValue<T>& ss) const {
        return label!=ss.label;
      }
  };
}
