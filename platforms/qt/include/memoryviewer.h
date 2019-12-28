#ifndef MEMORYVIEWER_H
#define MEMORYVIEWER_H

#include <QWidget>
#include <QPlainTextEdit>

class Memory_viewer : public QPlainTextEdit
{
    Q_OBJECT

    virtual void reload() = 0;
    virtual void highlight() = 0;
};

class Rom_viewer : public Memory_viewer
{
    public:
    explicit Rom_viewer();

    private:

};

#endif // MEMORYVIEWER_H
