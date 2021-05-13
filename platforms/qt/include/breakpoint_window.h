#ifndef BREAKPOINT_WINDOW_H
#define BREAKPOINT_WINDOW_H
/*

#include <QWidget>

#include <unordered_map>

namespace qtboy
{

class Debugger;

}

class QLabel;
class QLineEdit;
class QListWidget;

class Breakpoint_window : public QWidget
{
    Q_OBJECT

    public:
    Breakpoint_window(qtboy::Debugger *d, QWidget *parent = nullptr);

    private slots:
    void add_breakpoint();
    void delete_breakpoint();

    private:
    uint16_t get_address(bool *success);
    void add_breakpoint_to_list(uint16_t adr);

    private:
    qtboy::Debugger *debugger_;
    QLineEdit *breakpoint_adr_;
    QListWidget *breakpoint_list_;
    std::unordered_map<uint16_t, QLabel *> breakpoints_;
};
*/

#endif // BREAKPOINT_WINDOW_H
