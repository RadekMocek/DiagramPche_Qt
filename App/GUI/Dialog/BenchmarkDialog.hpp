#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

class BenchmarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BenchmarkDialog(QWidget* parent);

signals:
    void ButtonStartClicked();
};
