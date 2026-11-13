// Modal dialogs: message boxes (info, warning, yes/no) and file open/save pickers. Qt's own dialogs, never native ones.
#include "qt_common.hpp"
#include <QByteArray>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

namespace evgx {
using namespace qtb;

namespace {
QByteArray g_openPath, g_savePath;   // storage behind the returned pointers, valid until the next call of each
}

// kind 0 information, 1 warning, 2 yes/no (returns 1 for yes, 0 for no). Returns after the user answers.
int QtApi::messagebox(QtWidget* parent, const char* title, const char* text, int kind) {
    if (!init()) return -1;
    QWidget* p = parent ? W(parent) : nullptr;
    QString t = QString::fromUtf8(title ? title : "");
    QString s = QString::fromUtf8(text ? text : "");
    if (kind == 2) return QMessageBox::question(p, t, s, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes ? 1 : 0;
    if (kind == 1) QMessageBox::warning(p, t, s);
    else QMessageBox::information(p, t, s);
    return 1;
}

// "" when the user cancels; the Qt dialog is used even where a native one exists
const char* QtApi::openfile(QtWidget* parent, const char* title, const char* filter) {
    if (!init()) return "";
    QString f = QFileDialog::getOpenFileName(parent ? W(parent) : nullptr, QString::fromUtf8(title ? title : ""), QString(),
                                             QString::fromUtf8(filter ? filter : ""), nullptr, QFileDialog::DontUseNativeDialog);
    g_openPath = f.toUtf8();
    return g_openPath.constData();
}

const char* QtApi::savefile(QtWidget* parent, const char* title, const char* filter) {
    if (!init()) return "";
    QString f = QFileDialog::getSaveFileName(parent ? W(parent) : nullptr, QString::fromUtf8(title ? title : ""), QString(),
                                             QString::fromUtf8(filter ? filter : ""), nullptr, QFileDialog::DontUseNativeDialog);
    g_savePath = f.toUtf8();
    return g_savePath.constData();
}

}  // namespace evgx
