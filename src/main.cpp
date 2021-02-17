#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QHotkey>
#include <QObject>
#include <QPixmap>
#include <map>
#include <vector>

#include "config.h"
#include "configwindow.h"
#include "selectorwidget.h"
#include "state.h"
#include "utils.h"

State state;
OCR *ocr;
QClipboard *clipboard;
QString imagePath = "/tmp/tempImg.png";

QHotkey *setupOCRHotkey(QString sequence, void callback(ORIENTATION orn),
        ORIENTATION orn = NONE) {
    QHotkey *hotkey = new QHotkey(QKeySequence(sequence), true, qApp);
    QObject::connect(hotkey, &QHotkey::activated, qApp, [=]() { callback(orn); });
    return hotkey;
}

void runRegOCR(ORIENTATION orn) {

    if (!state.getCurrentlySelecting()) {
        state.setCurrentlySelecting(true);
        SelectorWidget sw;
        sw.exec();
        state.setCurrentlySelecting(false);

        char *result = ocr->ocrImage(imagePath, orn);
        clipboard->setText(result);

        LastOCRInfo info = {orn, sw.lastSelectedRect};
        state.setLastOCRInfo(info);

#ifdef DEBUG
        qDebug(result);
#endif
    }
}

void runPrevOCR(ORIENTATION _) {
    QPixmap desktopPixmap = grabScreenshot();
    QPixmap selectedPixmap =
        desktopPixmap.copy(state.getLastOCRInfo().rect.normalized());
    selectedPixmap.toImage().save(imagePath);

    ORIENTATION orn = state.getLastOCRInfo().orn;
    if (orn != NONE) {
        char *result = ocr->ocrImage(imagePath, orn);
        clipboard->setText(result);
#ifdef DEBUG
        qDebug(result);
#endif
    }
}

int main(int argc, char **argv) {
    ocr = new OCR();

    if (argc > 1){
      if (! std::strcmp(argv[1], "help") || ! std::strcmp(argv[1], "--help")){
	qInfo("OCR for Japanese Texts.");
	qInfo("Usage:");
	qInfo("1) %s", argv[0]);
	qInfo("\t%s", "Run the main application.");
	qInfo("2) %s %s", argv[0],"help/ --help");
	qInfo("\t%s", "Display this message.");
	qInfo("3) %s %s", argv[0], "ORIENTATION{-h; -v; horizontal; vertical} IMAGEFILE");
	qInfo("\tRun the OCR on the given IMAGEFILE with given ORIENTATION.");
	exit(0);
      }
      if (argc > 2){
        char* orientation = argv[1];
        char* img = argv[2];

        ORIENTATION orn;

        if (!pathExist(img)){
            qCritical("Invalid image path");
            exit(1);
        }

        if (! std::strcmp(orientation, "vertical") || ! std::strcmp(orientation, "-v")){
            orn = VERTICAL;
        }
        else if (! std::strcmp(orientation, "horizontal") || ! std::strcmp(orientation, "-h")){
            orn = HORIZONTAL;
        }
        else {
            qCritical("Invalid orientation");
            exit(1);
        }

        char *result = ocr->ocrImage(img, orn);
        qInfo("%s", result);
      } else {
	qCritical("Invalid Arguments, please use %s --help to see the usage", argv[0]);
      }
    }
    else {
        QApplication app(argc, argv);
        clipboard = QApplication::clipboard();
        QSettings settings("gazou", "gazou");

        QString verticalHotkey =
            settings.value("Hotkeys/verticalOCR", "Alt+A").toString();
        QString horizontalHotkey =
            settings.value("Hotkeys/horizontalOCR", "Alt+D").toString();
        QString prevOCRHotkey =
            settings.value("Hotkeys/repeatOCR", "Alt+S").toString();

        QHotkey *vKey = setupOCRHotkey(verticalHotkey, runRegOCR, VERTICAL);
        QHotkey *hKey = setupOCRHotkey(horizontalHotkey, runRegOCR, HORIZONTAL);
        QHotkey *prevKey = setupOCRHotkey(prevOCRHotkey, runPrevOCR);

        std::map<std::string, QHotkey *> hotkeys = {
            {"verticalOCR", vKey}, {"horizontalOCR", hKey}, {"repeatOCR", prevKey}};

        ConfigWindow *cw = new ConfigWindow(hotkeys);

        app.setQuitOnLastWindowClosed(false);
        int ret = app.exec();
        setRegistered(hotkeys, false);
        return ret;
    }
}
