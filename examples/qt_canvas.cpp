// qt_canvas: a Qt 6 window whose contents are presented by evg through Vulkan on the GPU. Runs ~6 seconds.
#include <QApplication>
#include <QCoreApplication>
#include <QTimer>
#include <stdio.h>
#include <evg/qt/evg_qt.hpp>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    evgx::qt::Canvas canvas;
    canvas.setWindowTitle("evg + Qt6 Vulkan");
    canvas.setClearColor(0.9f, 0.1f, 0.1f);
    canvas.resize(640, 480);
    canvas.show();
    printf("hwnd=%p\n", (void*)canvas.winId()); fflush(stdout);
    QTimer::singleShot(6000, &app, &QCoreApplication::quit);
    int rc = app.exec();
    printf("gpu: %s\n", evg.gpu());
    printf("qt window presented at %.1f fps through evg/Vulkan\n", canvas.fps());
    return rc;
}
