#pragma once

#include <QMainWindow>
#include <QPointer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
private:
    void InitMainMenuBar();
};
