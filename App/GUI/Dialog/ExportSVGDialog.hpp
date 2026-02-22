#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
QT_END_NAMESPACE

// ===================
enum ActionAfterExport
{
    ActionAfterExport_DoNothing, ActionAfterExport_OpenFolder, ActionAfterExport_OpenFile
};

// ========================
struct ExportSVGDialogState
{
    QString path;
    ActionAfterExport action;
};

// ===================================
class ExportSVGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state);

private:
    ExportSVGDialogState& m_state;
    void InitGUI();
    void UpdateGUI() const;

    QPointer<QLineEdit> m_group1_input;
    QPointer<QLabel> m_group2_label;
    QPointer<QCheckBox> m_group2_checkbox;
    QPointer<QRadioButton> m_group3_radio_nothing;
    QPointer<QRadioButton> m_group3_radio_explorer;
    QPointer<QRadioButton> m_group3_radio_open;
    QPointer<QPushButton> m_button_export;

    QPalette m_palette_normal;
    QPalette m_palette_error;

signals:
    void ButtonExportClicked();
};
