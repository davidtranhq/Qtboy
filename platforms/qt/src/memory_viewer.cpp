#include "memory_viewer.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QMessageBox>
#include <QTextBlock>

#include <sstream>
#include <iomanip>

Memory_viewer::Memory_viewer(const QString &mem, QWidget *parent)
    : QPlainTextEdit(mem, parent)
{
    setReadOnly(true);
    setWordWrapMode(QTextOption::NoWrap);
    QFont f {"Courier New"};
    setFont(f);
}

Memory_viewer::Memory_viewer(QWidget *parent)
    : Memory_viewer("", parent)
{

}

void Memory_viewer::update(const QString &txt)
{
    // save vertical scroll position and cursor
    int scroll = verticalScrollBar()->value();
    QTextCursor saved_cursor = textCursor();
    // locate visible text block
    const QTextBlock &visible = firstVisibleBlock();
    int start = visible.position();
    int len = visible.length();
    QTextCursor update_cursor = textCursor();
    update_cursor.setPosition(start);
    setTextCursor(update_cursor);
    setOverwriteMode(true);
    // get relevant block from input text
    const QString *block = txt.midRef(start, len).string();
    // replace visible text block with relevant input block
    if (block)
        insertPlainText(*block);
    // restore scrollbar and cursor position
    setTextCursor(saved_cursor);
    verticalScrollBar()->setValue(scroll);
}

// highlight the line containing adr
void Memory_viewer::highlight_lines()
{
    setExtraSelections(selections_);
}

void Memory_viewer::make_selection(uint16_t adr, const QColor &color)
{
    // convert address to hex string
    std::ostringstream hex_str;
    hex_str << std::hex << std::setfill('0') << std::setw(4) << adr;
    auto qhex_str = QString::fromStdString(hex_str.str());
    // save current cursor
    QTextCursor cursor = textCursor();
    QTextCursor saved_cursor = cursor;
    // find the hex string in the viewer
    if (!find(qhex_str))
    {
        // found nothing, restart at the beginning
        cursor.movePosition(QTextCursor::Start);
        setTextCursor(cursor);
        // if still not found, there is no match
        if (!find(qhex_str))
        {
            QMessageBox msg;
            msg.setText(QString(tr("Unable to locate address %1 in memory viewer"))
                        .arg(adr, 4, 16));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            // word not found : set the cursor back to its initial position
            setTextCursor(saved_cursor);
        }
    }
    // highlight the line where the cursor is located
    QTextCursor current = textCursor();
    current.select(QTextCursor::LineUnderCursor);
    QTextCharFormat fmt;
    fmt.setBackground(color);
    QTextEdit::ExtraSelection selection {current, fmt};
    selections_.push_back(selection);
    current.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    setTextCursor(current);
}

void Memory_viewer::clear_selections()
{
    selections_.clear();
}

