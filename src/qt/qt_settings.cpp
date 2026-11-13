// Persisted settings through QSettings. The organization and application names choose where values are stored.
#include "qt_common.hpp"
#include <QByteArray>
#include <QSettings>
#include <QString>

namespace evgx {

namespace {
QString g_org = QStringLiteral("evg");
QString g_app = QStringLiteral("evg-app");
QByteArray g_setting;   // storage behind getsetting's returned pointer
}

void QtApi::setapp(const char* organization, const char* application) {
    g_org = QString::fromUtf8(organization ? organization : "evg");
    g_app = QString::fromUtf8(application ? application : "evg-app");
}

void QtApi::setsetting(const char* key, const char* value) {
    QSettings s(g_org, g_app);
    s.setValue(QString::fromUtf8(key ? key : ""), QString::fromUtf8(value ? value : ""));
    s.sync();
}

const char* QtApi::getsetting(const char* key, const char* fallback) {
    QSettings s(g_org, g_app);
    QString v = s.value(QString::fromUtf8(key ? key : ""), QString::fromUtf8(fallback ? fallback : "")).toString();
    g_setting = v.toUtf8();
    return g_setting.constData();
}

}  // namespace evgx
