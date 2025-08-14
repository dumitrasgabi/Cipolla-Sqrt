#ifndef APP_H
#define APP_H

#include <QMainWindow>
#include <QString>
#include "BigInt.h"
#include "Cipolla.h"

namespace Ui {
    class AppClass;
}

class App : public QMainWindow {
    Q_OBJECT

public:
    explicit App(QWidget* parent = nullptr);
    ~App();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void calculate();
    void reset();

private:
    bool VerificareInput(const BigInt& p);
    bool EsteProbabilBun(const BigInt& n, int iterations);
    Ui::AppClass* ui;
};

#endif
