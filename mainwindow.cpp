#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cmath>
#include <QIntValidator>
#include <QDoubleValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->groupPos->setValidator(new QIntValidator(1, 100));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    bool hasError = false;
    bool ok = true;

    int gPos;
    double lb, rb, tol;

    clear();
    gPos = ui->groupPos->text().toInt(&ok);
    if(!ok || gPos < 1 || gPos > 100) {
        hasError = true;
        print("Error: Group position must be a number and be in range from 1 to 100");
    }

    lb = ui->leftBound->text().toDouble(&ok);
    if(!ok) {
        hasError = true;
        print("Error: Left Bound must be a valid number");
    }

    rb = ui->rightBound->text().toDouble(&ok);
    if(!ok) {
        hasError = true;
        print("Error: Right Bound must be a valid number");
    }

    tol = ui->tolerance->text().toDouble(&ok);
    if(!ok || tol < 0) {
        hasError = true;
        print("Error: Precision must be a valid number and be greater than 0");
    }


    if(rb <= lb) {
        print("Warning: Right Bound smaller or equals than Left Bound");
    }

    double minTol = 1e-200;

    if(tol < minTol) tol = minTol;

    if(hasError) return;

    findX(gPos, lb, rb, tol);

}

double MainWindow::F(int groupPos, double x)
{
    bool isOdd = (groupPos & 1) == 1;

    double a = static_cast<double>(groupPos);

    double b = pow(2.71828182845904523536028747135266249775724709369995,x);

    double c = (isOdd ? static_cast<double>(groupPos / 2) + 1 : static_cast<double>(groupPos / 2)) * (isOdd ? (x * x) : x);

    return isOdd ? a - b - c : a + b + c;
}

QString MainWindow::FString(int groupPos)
{
    bool isOdd = (groupPos & 1) == 1;
    int halfPos = isOdd ? (groupPos / 2) + 1 : (groupPos / 2);

    QString result; // 3 - e^x - 3x^2 = 0

    result += QString::number(groupPos) + " ";
    result += isOdd ? "- " : "+ ";
    result += "e^x ";
    result += isOdd ? "- " : "+ ";
    result += QString::number(halfPos);
    result += isOdd ? "x^2" : "x";

    return result;
}

double MainWindow::findX(int groupPos, double a, double b, double e)
{
    const double K = 0.1;
    const double T = 0;

    double d;

    if(e < 0 || std::isnan(e) || std::isinf(e) || std::isnan(a) || std::isinf(a) || std::isnan(b) || std::isinf(b)) return std::numeric_limits<double>::quiet_NaN();

    if(a > b) {
        double tmp = a;
        a = b;
        b = tmp;
    }

    int i = 0;

    clear();

    print("Expression: F(x) = 0. F is " + FString(groupPos));

    do {
        print(QString("[%1] Seg[a, b]: [%2, %3] (d = %4 | e = %5)").arg(QStringLiteral("%1").arg(i++, 2, 10, QLatin1Char('0')), QString::number(a, 'f', 4), QString::number(b, 'f', 4), QString::number(d, 'f', 4), QString::number(e, 'f', 4)));

        d = b - a;
        double c = (a + b) / 2.0;
        double step = K * (d / 2.0);
        double lX = c - step;
        double rX = c + step;
        double lY = F(groupPos, lX);
        double rY = F(groupPos, rX);
        double lAbs = std::abs(lY - T);
        double rAbs = std::abs(rY - T);

        if(lAbs <= rAbs) b = rX;
        if(lAbs >= rAbs) a = lX;
    } while (d > e);

    print("Result: " + QString::number(a, 'f', 10));

    return a;
}

void MainWindow::print(const QString str)
{
    ui->output->appendPlainText(str);
}

void MainWindow::clear()
{
    ui->output->clear();
}

