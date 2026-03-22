#include <QCheckBox>
#include <QGroupBox>
#include <QGuiApplication>
#include <QPushButton>
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

    const QPointer group1_2 = new QGroupBox("Canvas color theme");
    const QPointer group1_2_layout = new QHBoxLayout();

    const QPointer group1_2_checkbox = new QCheckBox("Keep canvas light");
    group1_2_checkbox->setChecked(true);
    group1_2_layout->addWidget(group1_2_checkbox);

    group1_2->setEnabled(false);
    group1_2->setLayout(group1_2_layout);
    layout_appearance->addWidget(group1_2);

    // .: TEXTEDIT TAB :.
    // .:==============:.
    const QPointer layout_textedit_wrapper = new QWidget();
    const QPointer layout_textedit = new QVBoxLayout(layout_textedit_wrapper);

    const QPointer group2_1 = new QGroupBox("Text editor font size");
    const QPointer group2_1_layout = new QHBoxLayout();

    const QPointer group2_1_int_input = new QSpinBox();
    group2_1_layout->addWidget(group2_1_int_input);

    group2_1->setLayout(group2_1_layout);
    layout_textedit->addWidget(group2_1);

    const QPointer group2_2 = new QGroupBox("Text editor syntax highlight");
    const QPointer group2_2_layout = new QHBoxLayout();

    const QPointer group2_2_checkbox = new QCheckBox("Enable syntax highlight");
    group2_2_layout->addWidget(group2_2_checkbox);

    group2_2->setLayout(group2_2_layout);
    layout_textedit->addWidget(group2_2);

    // .: VIEW TAB :.
    // .:==========:.
    const QPointer layout_view_wrapper = new QWidget();
    const QPointer layout_view = new QVBoxLayout(layout_view_wrapper);
    const QPointer view_toolbar_checkbox = new QCheckBox("Toolbar");
    layout_view->addWidget(view_toolbar_checkbox);
    const QPointer view_grid_checkbox = new QCheckBox("Canvas grid");
    layout_view->addWidget(view_grid_checkbox);
    const QPointer view_secondary_toolbar_checkbox = new QCheckBox("Secondary canvas toolbar");
    layout_view->addWidget(view_secondary_toolbar_checkbox);

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
        emit ButtonApplyClicked();
    });
    setLayout(layout);
}
