#ifndef MEMORYVIEWER_H
#define MEMORYVIEWER_H
/*
#include <QWidget>
#include <QPlainTextEdit>

class MemoryViewer : public QPlainTextEdit
{
    Q_OBJECT

    public:
    explicit MemoryViewer(const QString &memory,
                           QWidget *parent = nullptr);
    explicit MemoryViewer(QWidget *parent = nullptr);

    void highlight_lines();
    void update(const QString &memory);
    void make_selection(uint16_t adr, const QColor &);
    void clear_selections();

    private:
    QPlainTextEdit *viewer_;
    QList<QTextEdit::ExtraSelection> selections_;
};
*/
#endif // MEMORYVIEWER_H
