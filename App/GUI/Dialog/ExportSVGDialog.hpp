#pragma once

#include <QDialog>

struct ExportSVGDialogState
{
    QString path;
};

class ExportSVGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state);

private:
    ExportSVGDialogState& m_state;
};
