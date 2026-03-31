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
class QSpinBox;
QT_END_NAMESPACE

// === === === === === === === === ===

// What to do after successful SVG export
enum ActionAfterExport
{
    ActionAfterExport_DoNothing, ActionAfterExport_OpenFolder, ActionAfterExport_OpenFile
};

// === === === === === === === === ===

// This state is saved after successful SVG export and restored if user opens this dialog again in the same session
struct ExportSVGDialogState
{
    // SVG filename
    QString path;
    // SVG padding
    int padding;
    // What to do after successful SVG export
    ActionAfterExport action;
};

// === === === === === === === === ===

// Dialog that allow user to export the diagram into a SVG file by specifying file name and action after export.
// This dialog checks the path and after clicking export it sends signal wo the main window, where the actual SVG export happens.
class ExportSVGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state);

private:
    ExportSVGDialogState& m_state;
    void InitGUI();
    void UpdateGUI() const;

    // Path input for SVG filename
    QPointer<QLineEdit> m_group_path_input;
    // This label informs user if the file at path specified in `m_group1_input` already exists or not; the label is red if the file exists
    QPointer<QLabel> m_group_guard_label;
    // Overwrite checkbox, user has to check this to allow SVG export to file that already exists
    QPointer<QCheckBox> m_group_guard_checkbox;
    // Spinbox to change the SVG padding
    QPointer<QSpinBox> m_group_padding_spinbox;
    // Radio to choose `ActionAfterExport_DoNothing`
    QPointer<QRadioButton> m_group_action_radio_nothing;
    // Radio to choose `ActionAfterExport_OpenFolder`
    QPointer<QRadioButton> m_group_action_radio_explorer;
    // Radio to choose `ActionAfterExport_OpenFile`
    QPointer<QRadioButton> m_group_action_radio_open;
    // Button to start the export
    QPointer<QPushButton> m_button_export;

    // Palette used to switch `m_group_guard_label` color to normal color (black)
    QPalette m_palette_normal;
    // Palette used to switch `m_group_guard_label` color to error color (red)
    QPalette m_palette_error;

signals:
    // Signal to tell the main window to start the actual export
    void ButtonExportClicked();
};
