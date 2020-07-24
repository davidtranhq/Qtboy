#ifndef MEMORY_VIEWER_H
#define MEMORY_VIEWER_H

#include <QPlainTextEdit>

class Memory_viewer : public QPlainTextEdit
{
    Q_OBJECT

    public:
    explicit Memory_viewer(const QString &memory,
                           QWidget *parent = nullptr);
    explicit Memory_viewer(QWidget *parent = nullptr);

    void highlight_lines();
    void update(const QString &memory);
    void make_selection(uint16_t adr, const QColor &);
    void clear_selections();

    private:
    QPlainTextEdit *viewer_;
    QList<QTextEdit::ExtraSelection> selections_;
};

#endif // MEMORY_VIEWER_H
