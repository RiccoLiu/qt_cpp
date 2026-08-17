#include "mainwindow.h"

#include <QApplication>

#include <QFile>
#include <QString>
#include <QStyleFactory>
#include <QFontDatabase>

// 设置默认风格，影响所有未风格的窗口和控件
int LoadStyleSheet(QApplication& app) {
    /**
     *  QT Style Sheet 风格, fusion保证各个平台的显示风格一致
     * Windows: "windows", "windowsvista", "fusion"
     * macOS: "macintosh", "fusion"
     * Linux (KDE): "breeze", "fusion", "gtk2" 等
     */
    if (QStyleFactory::keys().contains("fusion")) {
        app.setStyle(QStyleFactory::create("fusion"));
    }

    QString style_sheet = ":/styles/styles.qss";
    QFile file(style_sheet);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning() << "Failed to load stylesheet: " << style_sheet;
    }
    return 0;
}

// 设置默认字体，影响所有未设置字体的窗口和控件
int LoadFont(QApplication& app) {
    QString font_file = ":/fonts/Roboto-Regular.ttf";
    // QString font_file = ":/fonts/Font_Abbey_Dawn_by_loveinhoollywood.ttf";

    int fontId = QFontDatabase::addApplicationFont(font_file);
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        app.setFont(QFont(fontFamily, 9)); // 默认9号字体
    } else {
        qWarning() << "Failed to load font: " << font_file;
    }

    return 0;
}

void applyDarkTheme(QApplication *app)
{
    QPalette pal;

    // ✅ 基础表面色
    pal.setColor(QPalette::Window,          QColor("#1E1E2E"));
    pal.setColor(QPalette::Base,            QColor("#282840"));
    pal.setColor(QPalette::AlternateBase,   QColor("#313150"));

    // ✅ 文字层级（关键！好看不好看全看这里）
    pal.setColor(QPalette::Text,            QColor("#E0E0E8"));  // 主文字
    pal.setColor(QPalette::ButtonText,      QColor("#E0E0E8"));
    pal.setColor(QPalette::PlaceholderText, QColor("#8888A0"));  // 次要/提示

    // ✅ 交互反馈色
    pal.setColor(QPalette::Highlight,       QColor("#7C6BF0"));  // 品牌强调色
    pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    pal.setColor(QPalette::Link,            QColor("#9D8FFF"));

    // ✅ 边框与分隔
    pal.setColor(QPalette::Mid,             QColor("#3A3A55"));
    pal.setColor(QPalette::Dark,            QColor("#151520"));

    app->setPalette(pal);

    // QSS 只补充 Palette 无法表达的样式（圆角、阴影、渐变）
    // app->setStyleSheet(loadQss(":/themes/dark.qss"));
}

void SetupHighDpi()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用元信息
    app.setApplicationName("MVS");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("YourCompany");
    app.setOrganizationDomain("yourCompany.com");

    SetupHighDpi();
    // LoadStyleSheet(app); // 设置颜色表
    // applyDarkTheme(&app);
    LoadFont(app);

    MainWindow w;
    w.show();

    return app.exec();
}
