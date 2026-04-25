#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QRadioButton;
class QSpinBox;
QT_END_NAMESPACE

// === === === === === === === === === === ===

struct PreferencesDialogState
{
    bool is_canvas_light;
    int source_font_size;
    bool is_syntax_highlight_enabled;
    bool do_show_primary_toolbar;
    bool do_show_canvas_grid;
    bool do_show_secondary_toolbar;
};

// === === === === === === === === === === ===

// === Config ==== === === === === === === ===
constexpr auto FONT_SIZE_SOURCE_MIN = 8;
constexpr auto FONT_SIZE_SOURCE_MAX = 40;
constexpr auto FONT_SIZE_SOURCE_STEP = 2;
// === === === === === === === === === === ===

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent, PreferencesDialogState& state);

private:
    PreferencesDialogState& m_state;
    void InitGUI();

    QPointer<QRadioButton> m_appareance_canvas_light;
    QPointer<QRadioButton> m_appareance_canvas_dark;
    QPointer<QSpinBox> m_textedit_font_size;
    QPointer<QCheckBox> m_textedit_syntax_highlight;
    QPointer<QCheckBox> m_view_primary_toolbar;
    QPointer<QCheckBox> m_view_canvas_grid;
    QPointer<QCheckBox> m_view_secondary_toolbar;

signals:
    void ButtonApplyClicked();
    void ButtonColorThemeClicked(bool is_light);
};
