#include <QClipboard>
#include <QCursor>
#include <QGuiApplication>
#include <QHotkey>
#include <QPainter>
#include <QPixmap>
#include <sys/stat.h>

#include "utils.h"

QScreen *getActiveScreen() {
    QPoint globalCursorPos = QCursor::pos();
    QScreen *activeScreen = QGuiApplication::screenAt(globalCursorPos);
    return activeScreen;
}

void remove_spaces(char *s) {
    const char *d = s;
    int space_flag = 0;
    int newline_flag = 0;
    do {
      switch (*d){
      case ' ':
      case '\t':
	if (space_flag){
	  ++d;
	  continue;
	}
	space_flag = 1;
	break;
      case '\n':
	if (++newline_flag > 2){
	  // >2 as \n for continuation line and \n\n for new lines.
	  ++d;
	  continue;
	}
	break;
      default:
	space_flag = 0;
	newline_flag = 0;
      }
    } while (*s++ = *d++);
}

void setRegistered(std::map<std::string, QHotkey *> hotkeys, bool registered) {
    for (auto const &x : hotkeys) {
        (x.second)->setRegistered(registered);
    }
}

void copyToClipboard(char *text, QClipboard *cb) { cb->setText(text); }

QPixmap grabScreenshot() {
    QScreen *activeScreen = getActiveScreen();
    QPixmap desktopPixmap = QPixmap(activeScreen->geometry().size());
    QPainter p(&desktopPixmap);

    p.drawPixmap(*(new QPoint(0, 0)), activeScreen->grabWindow(0));

    return desktopPixmap;
}


bool pathExist(const char* s)
{
  struct stat buffer;
  return (stat (s, &buffer) == 0);
}
