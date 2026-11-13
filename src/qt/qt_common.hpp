// Shared Qt bridge helpers: handle casts and placement. Internal to src/qt.
#pragma once
#include <evg/evg.hpp>
#include <QWidget>

namespace evgx { namespace qtb {

// evg widget handles are QWidget pointers in disguise
inline QWidget* W(QtWidget* p) { return reinterpret_cast<QWidget*>(p); }
inline QtWidget* H(QWidget* w) { return reinterpret_cast<QtWidget*>(w); }

// puts a widget under `parent` (or at top level) at the given geometry. Shown only when it has a parent.
QtWidget* place(QWidget* w, QtWidget* parent, int x, int y, int ww, int hh);

}}  // namespace evgx::qtb
