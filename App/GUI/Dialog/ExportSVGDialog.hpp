#pragma once

#include <QDialog>

enum ActionAfterExport
{
    ActionAfterExport_DoNothing, ActionAfterExport_OpenFolder, ActionAfterExport_OpenFile
};

struct ExportSVGDialogState
{
    QString path;
    ActionAfterExport action;
};

class ExportSVGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state);

private:
    ExportSVGDialogState& m_state;

signals:
    void ButtonExportClicked();
};
