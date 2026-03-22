#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// === === === === === === === === ===

struct PreferencesDialogState
{
    bool do_keep_canvas_light;
};

// === === === === === === === === ===

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent, PreferencesDialogState& state);

private:
    PreferencesDialogState& m_state;
    void InitGUI();

    QPointer<QCheckBox> m_appareance_keep_canvas_light;

signals:
    void ButtonApplyClicked();
};
