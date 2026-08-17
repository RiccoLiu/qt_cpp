
#include <QApplication>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QFile>
#include <QDir>
#include <QDebug>

#include "mainwindow.h"
#include "database.h"
#include "logger2.h"

// 加载自定义样式表（如深色主题）
void loadStyleSheet(QApplication &app)
{
    QString stylePath = ":/styles/styles.qss"; // 使用 Qt 资源系统
    QFile file(stylePath);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning() << "Failed to load stylesheet:" << stylePath;
    }
}

void setupHighDpi()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}


int main(int argc, char *argv[]) {
    Logger2::Init(Logger2::Level::Info, "mysql.log");
    LOGI("%s:%d: App starting...", __func__, __LINE__);

    QApplication app(argc, argv);

    // 设置应用元信息
    app.setApplicationName("MyMVS");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("YourCompany");

    // 使用 Fusion 风格确保跨平台一致性（尤其 Windows 上更现代）
    if (QStyleFactory::keys().contains("Fusion")) {
        app.setStyle(QStyleFactory::create("Fusion"));
    }

    // 加载自定义字体（如 Roboto、Noto Sans 等）
    int fontId = QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        app.setFont(QFont(fontFamily, 9));
    }

    // 加载样式表（深色主题等）
    loadStyleSheet(app);

    MainWindow w;
    w.show();
    return app.exec();
}

