#include <QString>
#include <QCoreApplication>
#include <iostream>
#include <random>
#include <stdexcept>
#include <limits>
#include <QKeyEvent>

#include "ui_App.h"
#include "App.h"
#include "Cipolla.h"
#include "BigInt.h"

using namespace std;

App::App(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::AppClass)
{
    ui->setupUi(this);
    
    connect(ui->btnEnter, &QPushButton::clicked, this, &App::calculate);
    connect(ui->btnReset, &QPushButton::clicked, this, &App::reset);
    setWindowFlags(Qt::Window);
    setMinimumSize(640, 480);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setWindowTitle("Radacini Patrate in Zp");
    setWindowIcon(QIcon("images/icon.png"));

    ui->tbA->setPlaceholderText("Introduceti a...");
    ui->tbP->setPlaceholderText("Introduceti p...");

    ui->tbX->setWordWrapMode(QTextOption::WordWrap);
    ui->tbX->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void App::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        ui->btnEnter->click();
        break;

    case Qt::Key_Down:
        if (ui->tbA->hasFocus()) {
            ui->tbP->setFocus();
        }
        else if (ui->tbP->hasFocus()) {
            ui->tbA->setFocus();
        }
        break;

    case Qt::Key_Up:
        if (ui->tbP->hasFocus()) {
            ui->tbA->setFocus();
        }
        else if (ui->tbA->hasFocus()) {
            ui->tbP->setFocus();
        }
        break;

    default:
        QMainWindow::keyPressEvent(event);
    }
}

void App::reset() {
    ui->tbA->clear();
    ui->tbP->clear();
    ui->tbX->clear();
}

App::~App() {
    delete ui;
}

void App::calculate() {

    bool ok1, ok2;
    BigInt a, p;
    try {
        a = BigInt(ui->tbA->text().toStdString());
    }
    catch (...) {
        ui->tbX->setText("Input invalid: a trebuie sa fie un numar intreg valid");
        return;
    }

    try {
        p = BigInt(ui->tbP->text().toStdString());
    }
    catch (...) {
        ui->tbX->setText("Input invalid: p trebuie sa fie un numar intreg valid");
        return;
    }

    if (!VerificareInput(p)) {
        ui->tbX->setText("Input invalid: p trebuie sa fie prim si mai mare decat 2");
        return;
    }

    try {
        ui->tbX->setText("Se calculeaza...");
        QCoreApplication::processEvents();
        vector<BigInt> solutions = Cipolla::Solve(a, p);

        if (solutions.empty()) {
            ui->tbX->setText("Nu exista solutii");
        }
        else {
            QString solutionsText;
            for (const auto& sol : solutions) {
                solutionsText += QString("x \u2261 %1 mod %2\n")
                    .arg(QString::fromStdString(sol.toString()))
                    .arg(QString::fromStdString(p.toString()));
            }
            ui->tbX->setText(solutionsText);
        }
    }
    catch (const exception& e) {
        ui->tbX->setText(QString("Error: %1").arg(e.what()));
    }
}

bool App::EsteProbabilBun(const BigInt& n, int iterations) {
    BigInt d = n - BigInt(1);
    int s = 0;
    while (d % BigInt(2) == BigInt(0)) {
        d = d / BigInt(2);
        s++;
    }

    for (int i = 0; i < iterations; i++) {
        BigInt a = BigInt::random(n - BigInt(2)) + BigInt(1);
        BigInt x = Cipolla::ModPow(a, d, n);
        if (x == BigInt(1) || x == n - BigInt(1)) continue;

        bool probablePrime = false;
        for (int r = 1; r < s; ++r) {
            x = Cipolla::ModPow(x, BigInt(2), n);
            if (x == n - BigInt(1)) {
                probablePrime = true;
                break;
            }
            if (x == BigInt(1)) {
                return false;
            }
        }

        if (!probablePrime) {
            return false;
        }
    }
    return true;
}

bool App::VerificareInput(const BigInt& p) {
    if (p <= BigInt(2)) {
        return false;
    }
    if (p % BigInt(2) == BigInt(0)) {
        return false;
    }

    if (p < BigInt(1000)) {
        BigInt i = BigInt(3);
        BigInt max = p / BigInt(2);
        while (i <= max) {
            if (p % i == BigInt(0)) {
                return false;
            }
            i = i + BigInt(2);
        }
        return true;
    }

    return EsteProbabilBun(p, 10);
}
