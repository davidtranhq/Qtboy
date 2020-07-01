#include "breakpoint_window.h"
#include "debugger.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>

Breakpoint_window::Breakpoint_window(gameboy::Debugger *d,
                                     QWidget *parent)
    : QWidget(parent),
      debugger_ {d},
      breakpoint_adr_(new QLineEdit),
      breakpoint_list_(new QListWidget)
{
    // add all breakpoints to list
    const std::unordered_map<uint16_t, bool> &breaks = debugger_->breakpoints();
    // first = key (address), second = boolean indicating if the address is a
    // breakpoint or not
    for (const auto &b : breaks)
        if (b.second)
            add_breakpoint_to_list(b.first);

    // prevent addresses larger than ffff
    breakpoint_adr_->setMaxLength(4);

    // create layout
    auto *input_layout = new QHBoxLayout;

    auto *add_btn = new QPushButton(tr("Add"));
    auto *delete_btn = new QPushButton(tr("Delete"));
    connect(add_btn, SIGNAL(clicked()), this, SLOT(add_breakpoint()));
    connect(delete_btn, SIGNAL(clicked()), this, SLOT(delete_breakpoint()));

    input_layout->addWidget(new QLabel("PC:"));
    input_layout->addWidget(breakpoint_adr_);
    input_layout->addWidget(add_btn);
    input_layout->addWidget(delete_btn);

    auto *layout = new QVBoxLayout;
    layout->addLayout(input_layout);
    layout->addWidget(breakpoint_list_);
    setLayout(layout);
    setWindowTitle("Edit Breakpoints");
}

void Breakpoint_window::add_breakpoint()
{
    bool ok = false;
    uint16_t adr = get_address(&ok);
    if (!ok)
        return;
    debugger_->add_breakpoint(adr);
    add_breakpoint_to_list(adr);
}

void Breakpoint_window::delete_breakpoint()
{
    auto selected_bps = breakpoint_list_->selectedItems();
    bool ok = false;
    for (auto bp : selected_bps)
    {
        // get address held in text
        uint16_t adr = static_cast<uint16_t>(
                    bp->text().toUInt(&ok, 16));
        // remove breakpoint from debugger
        debugger_->delete_breakpoint(adr);
        // remove breakpoint from list
        delete bp;
    }
}

uint16_t Breakpoint_window::get_address(bool *success)
{
    bool ok = false;
    // convert text to hexadecimal
    uint16_t adr = static_cast<uint16_t>(
                breakpoint_adr_->text().toUInt(&ok, 16));
    if (!ok)
    {
        // error handling
        QMessageBox msg;
        msg.setText("Error parsing address value. Use hex values inrange from"
                    " 0000 to ffff.");
        msg.exec();
        *success = false;
    }
    *success = true;
    return adr;
}

void Breakpoint_window::add_breakpoint_to_list(uint16_t adr)
{
    // insert label at row adr with the label adr
    auto str = QStringLiteral("%1").arg(adr, 4, 16, QChar('0'));
    QListWidgetItem *new_bp = new QListWidgetItem(str);
    breakpoint_list_->insertItem(adr, new_bp);
}
