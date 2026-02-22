#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#include "ExportSVGDialog.hpp"
#include "../../Config.hpp"

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

    // .: Group 1 :: Location :.
    // .:=====================:.
    const QPointer group1 = new QGroupBox("Location");
    const QPointer group1_layout = new QHBoxLayout();

    m_group1_input = new QLineEdit();
    m_group1_input->setText(m_state.path);
    group1_layout->addWidget(m_group1_input);

    connect(m_group1_input, &QLineEdit::textChanged, [this] {
        m_group2_checkbox->setChecked(false);
        UpdateGUI();
    });

    const QPointer group1_button = new QPushButton("Browse...");
    group1_layout->addWidget(group1_button);

    connect(group1_button, &QPushButton::clicked, [this] {
        const auto path =
            QFileDialog::getSaveFileName(this, "Export to SVG", m_group1_input->text(),
                                         "Scalable Vector Graphics (*.svg)");
        if (!path.isEmpty()) {
            m_group1_input->setText(path);
            m_group2_checkbox->setChecked(false);
            UpdateGUI();
        }
    });

    group1->setLayout(group1_layout);
    layout->addWidget(group1);

    // .: Group 2 :: Overwrite guard :.
    // .:============================:.
    const QPointer group2 = new QGroupBox("Overwrite guard");
    const QPointer group2_layout = new QVBoxLayout();

    m_group2_label = new QLabel("Path verification failed.");
    group2_layout->addWidget(m_group2_label);

    m_group2_checkbox = new QCheckBox("Overwrite");
    group2_layout->addWidget(m_group2_checkbox);

    connect(m_group2_checkbox, &QCheckBox::checkStateChanged, this, &ExportSVGDialog::UpdateGUI);

    group2->setLayout(group2_layout);
    layout->addWidget(group2);

    // .: Group 3 :: Action after export :.
    // .:================================:.
    const QPointer group3 = new QGroupBox("Action after export");
    const QPointer group3_layout = new QHBoxLayout();

    m_group3_radio_nothing = new QRadioButton("Nothing");
    m_group3_radio_explorer = new QRadioButton("Show in explorer");
    m_group3_radio_open = new QRadioButton("Open");
    m_group3_radio_nothing->setChecked(m_state.action == ActionAfterExport_DoNothing);
    m_group3_radio_explorer->setChecked(m_state.action == ActionAfterExport_OpenFolder);
    m_group3_radio_open->setChecked(m_state.action == ActionAfterExport_OpenFile);
    group3_layout->addWidget(m_group3_radio_nothing);
    group3_layout->addWidget(m_group3_radio_explorer);
    group3_layout->addWidget(m_group3_radio_open);

    group3->setLayout(group3_layout);
    layout->addWidget(group3);

    // .: Export button :.
    // .:===============:.
    m_button_export = new QPushButton("Export");
    layout->addWidget(m_button_export);
    connect(m_button_export, &QPushButton::clicked, [this] {
        m_state.path = m_group1_input->text();
        m_state.action = (m_group3_radio_nothing->isChecked())
                             ? ActionAfterExport_DoNothing
                             : (m_group3_radio_explorer->isChecked())
                             ? ActionAfterExport_OpenFolder
                             : ActionAfterExport_OpenFile;
        emit ButtonExportClicked();
        close();
    });

    setLayout(layout);
}

void ExportSVGDialog::UpdateGUI() const
{
    const auto path = m_group1_input->text().toStdString();

    bool can_export = true;
    bool is_overwrite_needed = false;

    if (!QFile::exists(m_group1_input->text())) {
        m_group2_label->setPalette(m_palette_normal);
        m_group2_label->setText("Specified path is unique.");
    }
    else {
        const QFileInfo path_info(m_group1_input->text());
        can_export = false;
        if (!path_info.isDir()) {
            m_group2_label->setPalette(m_palette_error);
            m_group2_label->setText("File at the specified path already exists.");
            is_overwrite_needed = true;
            if (m_group2_checkbox->isChecked()) {
                can_export = true;
            }
        }
        else {
            m_group2_label->setPalette(m_palette_error);
            m_group2_label->setText("The specified path is a directory.");
        }
    }

    m_group2_checkbox->setEnabled(is_overwrite_needed);
    m_button_export->setEnabled(can_export);
}
