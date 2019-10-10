#ifndef DISPLAY_H
#define DISPLAY_H

#include <QImage>

class Display : public QImage
{
    Q_OBJECT

public:

    explicit Display(QWidget *parent = nullptr);

protected:
};

#endif // DISPLAY_H
