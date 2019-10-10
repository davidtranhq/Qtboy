#include "disassemblerwindow.h"

#include <QFont>

DisassemblerWindow::DisassemblerWindow(gameboy::System &s)
    : system {s},
      textArea {new QTextEdit}
{
    QFont font {"Courier New", 12};
    textArea->setFont(font);
    textArea->setReadOnly(true);
    textArea->setPlainText(QString::fromStdString(s.disassemble()));
    textArea->show();
}

