#include "disassemblerwindow.h"
#include "disassembler.hpp"

#include <QFont>
#include <sstream>

DisassemblerWindow::DisassemblerWindow(QWidget *parent, gameboy::System *s)
    : QWidget {parent, Qt::Window},
      debugger {s},
      mainLayout {new QVBoxLayout},
      goToLayout {new QHBoxLayout},
      goToPrompt {new QLineEdit},
      goToButton {new QPushButton(tr("Go to"))},
      textArea {new QTextEdit}
{
    resize(400, 450);
    goToButton->setFont({"Calibri", 12});

    QFont font {"Courier New", 12};
    textArea->setFont(font);
    textArea->setReadOnly(true);
    textArea->setWordWrapMode(QTextOption::NoWrap);
    textArea->setPlainText(QString::fromStdString(dsmblr.pretty_disassemble(debugger.dump_rom())));

    goToLayout->addWidget(goToPrompt);
    goToLayout->addWidget(goToButton);
    mainLayout->addLayout(goToLayout);
    mainLayout->addWidget(textArea);
    setLayout(mainLayout);

    connect(goToButton, SIGNAL(clicked()), this, SLOT(goToAddress()));
}

void DisassemblerWindow::goToAddress()
{
    textArea->find(goToPrompt->text());
}

