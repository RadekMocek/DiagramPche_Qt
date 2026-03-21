#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "BenchmarkDialog.hpp"

BenchmarkDialog::BenchmarkDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Benchmark");

    const QPointer layout = new QVBoxLayout();

    const QPointer dummy1 = new QLabel("TODO");
    layout->addWidget(dummy1);

    const QPointer button = new QPushButton("Start benchmark");
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, [this] {
        emit ButtonStartClicked();
    });

    setLayout(layout);
}
