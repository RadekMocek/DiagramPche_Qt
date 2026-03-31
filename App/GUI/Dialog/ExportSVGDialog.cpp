#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "ExportSVGDialog.hpp"
#include "../../Helper/Color.hpp"

ExportSVGDialog::ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state) : QDialog(parent), m_state(state)
{
    setWindowTitle("Export to SVG");

    m_palette_normal.setColor(QPalette::WindowText, COLOR_BLACK);
    m_palette_error.setColor(QPalette::WindowText, COLOR_ERROR);

    InitGUI();
    UpdateGUI();
}

void ExportSVGDialog::InitGUI()
{
    const QPointer layout = new QVBoxLayout();

    // .: Group 1 :: Path to SVG :.
    // .:=====================:.
    const QPointer group_path = new QGroupBox("Location");
    const QPointer group_path_layout = new QHBoxLayout();

    m_group_path_input = new QLineEdit();
    m_group_path_input->setText(m_state.path);
    group_path_layout->addWidget(m_group_path_input);

    connect(m_group_path_input, &QLineEdit::textChanged, [this] {
        m_group_guard_checkbox->setChecked(false);
        UpdateGUI();
    });

    const QPointer group1_button = new QPushButton("Browse...");
    group_path_layout->addWidget(group1_button);

    connect(group1_button, &QPushButton::clicked, [this] {
        const auto path =
            QFileDialog::getSaveFileName(this, "Export to SVG", m_group_path_input->text(),
                                         "Scalable Vector Graphics (*.svg)");
        if (!path.isEmpty()) {
            m_group_path_input->setText(path);
            m_group_guard_checkbox->setChecked(false);
            UpdateGUI();
        }
    });

    group_path->setLayout(group_path_layout);
    layout->addWidget(group_path);

    // .: Group 2 :: Overwrite guard :.
    // .:============================:.
    const QPointer group_guard = new QGroupBox("Overwrite guard");
    const QPointer group_guard_layout = new QVBoxLayout();

    m_group_guard_label = new QLabel("Path verification failed.");
    group_guard_layout->addWidget(m_group_guard_label);

    m_group_guard_checkbox = new QCheckBox("Overwrite");
    group_guard_layout->addWidget(m_group_guard_checkbox);

    connect(m_group_guard_checkbox, &QCheckBox::checkStateChanged, this, &ExportSVGDialog::UpdateGUI);

    group_guard->setLayout(group_guard_layout);
    layout->addWidget(group_guard);

    // .: Group 3 :: Document padding :.
    // .:=============================:.
    const QPointer group_padding = new QGroupBox("SVG padding");
    const QPointer group_padding_layout = new QHBoxLayout();

    m_group_padding_spinbox = new QSpinBox();
    m_group_padding_spinbox->setMinimum(0);
    m_group_padding_spinbox->setMaximum(100);
    m_group_padding_spinbox->setValue(m_state.padding);
    group_padding_layout->addWidget(m_group_padding_spinbox);

    group_padding->setLayout(group_padding_layout);
    layout->addWidget(group_padding);

    // .: Group 4 :: Action after export :.
    // .:================================:.
    const QPointer group_action = new QGroupBox("Action after export");
    const QPointer group_action_layout = new QHBoxLayout();

    m_group_action_radio_nothing = new QRadioButton("Nothing");
    m_group_action_radio_explorer = new QRadioButton("Show in explorer");
    m_group_action_radio_open = new QRadioButton("Open");
    m_group_action_radio_nothing->setChecked(m_state.action == ActionAfterExport_DoNothing);
    m_group_action_radio_explorer->setChecked(m_state.action == ActionAfterExport_OpenFolder);
    m_group_action_radio_open->setChecked(m_state.action == ActionAfterExport_OpenFile);
    group_action_layout->addWidget(m_group_action_radio_nothing);
    group_action_layout->addWidget(m_group_action_radio_explorer);
    group_action_layout->addWidget(m_group_action_radio_open);

    group_action->setLayout(group_action_layout);
    layout->addWidget(group_action);

    // .: Export button :.
    // .:===============:.
    m_button_export = new QPushButton("Export");
    layout->addWidget(m_button_export);
    connect(m_button_export, &QPushButton::clicked, [this] {
        m_state.path = m_group_path_input->text();
        m_state.padding = m_group_padding_spinbox->value();
        m_state.action = (m_group_action_radio_nothing->isChecked())
                             ? ActionAfterExport_DoNothing
                             : (m_group_action_radio_explorer->isChecked())
                             ? ActionAfterExport_OpenFolder
                             : ActionAfterExport_OpenFile;
        emit ButtonExportClicked();
        close();
    });

    setLayout(layout);
}

void ExportSVGDialog::UpdateGUI() const
{
    const auto path = m_group_path_input->text().toStdString();

    bool can_export = true;
    bool is_overwrite_needed = false;

    if (!QFile::exists(m_group_path_input->text())) {
        m_group_guard_label->setPalette(m_palette_normal);
        m_group_guard_label->setText("Specified path is unique.");
    }
    else {
        const QFileInfo path_info(m_group_path_input->text());
        can_export = false;
        if (!path_info.isDir()) {
            m_group_guard_label->setPalette(m_palette_error);
            m_group_guard_label->setText("File at the specified path already exists.");
            is_overwrite_needed = true;
            if (m_group_guard_checkbox->isChecked()) {
                can_export = true;
            }
        }
        else {
            m_group_guard_label->setPalette(m_palette_error);
            m_group_guard_label->setText("The specified path is a directory.");
        }
    }

    m_group_guard_checkbox->setEnabled(is_overwrite_needed);
    m_button_export->setEnabled(can_export);
}
