#include <QCheckBox>
#include <QGroupBox>
#include <QGuiApplication>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyleHints>
#include <QTabWidget>
#include <QVBoxLayout>

#include "PreferencesDialog.hpp"

PreferencesDialog::PreferencesDialog(QWidget* parent, PreferencesDialogState& state) : QDialog(parent), m_state(state)
{
    setWindowTitle("Preferences");

    // Qt automatically sets the width according to width of the widest tab,
    // but does not care about the tabs themselves, so here goes the magic number.
    setMinimumWidth(350);

    InitGUI();
}

void PreferencesDialog::InitGUI()
{
    const QPointer layout = new QVBoxLayout();

    // .: APPEARANCE TAB :.
    // .:================:.
    const QPointer layout_appearance_wrapper = new QWidget();
    const QPointer layout_appearance = new QVBoxLayout(layout_appearance_wrapper);

    // == App color theme ==
    const QPointer group1_1 = new QGroupBox("App color theme");
    const QPointer group1_1_layout = new QHBoxLayout();

    const QPointer appearance_button_light = new QPushButton("Light");
    connect(appearance_button_light, &QPushButton::clicked, [] {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    });
    group1_1_layout->addWidget(appearance_button_light);
    const QPointer appearance_button_dark = new QPushButton("Dark");
    connect(appearance_button_dark, &QPushButton::clicked, [] {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    });
    group1_1_layout->addWidget(appearance_button_dark);

    group1_1_layout->addStretch(1);
    group1_1->setLayout(group1_1_layout);
    layout_appearance->addWidget(group1_1);

    // == Canvas color theme ==
    const QPointer group1_2 = new QGroupBox("Canvas color theme");
    const QPointer group1_2_layout = new QHBoxLayout();

    m_appareance_canvas_light = new QRadioButton("Light");
    m_appareance_canvas_light->setChecked(m_state.is_canvas_light);
    group1_2_layout->addWidget(m_appareance_canvas_light);
    m_appareance_canvas_dark = new QRadioButton("Dark");
    m_appareance_canvas_dark->setChecked(!m_state.is_canvas_light);
    group1_2_layout->addWidget(m_appareance_canvas_dark);

    group1_2_layout->addStretch(1);
    group1_2->setLayout(group1_2_layout);
    layout_appearance->addWidget(group1_2);

    // .: TEXTEDIT TAB :.
    // .:==============:.
    const QPointer layout_textedit_wrapper = new QWidget();
    const QPointer layout_textedit = new QVBoxLayout(layout_textedit_wrapper);

    const QPointer group2_1 = new QGroupBox("Text editor font size");
    const QPointer group2_1_layout = new QHBoxLayout();

    m_textedit_font_size = new QSpinBox();
    m_textedit_font_size->setValue(m_state.source_font_size);
    m_textedit_font_size->setMinimum(FONT_SIZE_SOURCE_MIN);
    m_textedit_font_size->setMaximum(FONT_SIZE_SOURCE_MAX);
    group2_1_layout->addWidget(m_textedit_font_size);

    group2_1->setLayout(group2_1_layout);
    layout_textedit->addWidget(group2_1);

    const QPointer group2_2 = new QGroupBox("Text editor syntax highlight");
    const QPointer group2_2_layout = new QHBoxLayout();

    m_textedit_syntax_highlight = new QCheckBox("Enable syntax highlight");
    m_textedit_syntax_highlight->setChecked(m_state.is_syntax_highlight_enabled);
    group2_2_layout->addWidget(m_textedit_syntax_highlight);

    group2_2->setLayout(group2_2_layout);
    layout_textedit->addWidget(group2_2);

    // .: VIEW TAB :.
    // .:==========:.
    const QPointer layout_view_wrapper = new QWidget();
    const QPointer layout_view = new QVBoxLayout(layout_view_wrapper);

    m_view_primary_toolbar = new QCheckBox("Toolbar");
    m_view_primary_toolbar->setChecked(m_state.do_show_primary_toolbar);
    layout_view->addWidget(m_view_primary_toolbar);

    m_view_canvas_grid = new QCheckBox("Canvas grid");
    m_view_canvas_grid->setChecked(m_state.do_show_canvas_grid);
    layout_view->addWidget(m_view_canvas_grid);

    m_view_secondary_toolbar = new QCheckBox("Secondary canvas toolbar");
    m_view_secondary_toolbar->setChecked(m_state.do_show_secondary_toolbar);
    layout_view->addWidget(m_view_secondary_toolbar);

    // .::.
    const QPointer tabs = new QTabWidget();
    tabs->addTab(layout_appearance_wrapper, "Appearance");
    tabs->addTab(layout_textedit_wrapper, "Text editor");
    tabs->addTab(layout_view_wrapper, "View");

    layout->addWidget(tabs);

    // .: APPLY ALL BUTTON :.
    // .:==================:.
    const QPointer button_apply_all = new QPushButton("Apply all settings");

    layout->addWidget(button_apply_all);
    connect(button_apply_all, &QPushButton::clicked, [this] {
        m_state.is_canvas_light = m_appareance_canvas_light->isChecked();
        m_state.source_font_size = m_textedit_font_size->value();
        m_state.is_syntax_highlight_enabled = m_textedit_syntax_highlight->isChecked();
        m_state.do_show_primary_toolbar = m_view_primary_toolbar->isChecked();
        m_state.do_show_canvas_grid = m_view_canvas_grid->isChecked();
        m_state.do_show_secondary_toolbar = m_view_secondary_toolbar->isChecked();
        emit ButtonApplyClicked();
    });
    setLayout(layout);
}
