#include "ExportSVGDialog.hpp"

#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

ExportSVGDialog::ExportSVGDialog(QWidget* parent, ExportSVGDialogState& state) : QDialog(parent), m_state(state)
{
    setWindowTitle("Export to SVG");

    const QPointer layout = new QVBoxLayout();

    // .: Group 1 :: Location :.
    // .:=====================:.
    const QPointer group1 = new QGroupBox("Location");
    const QPointer group1_layout = new QHBoxLayout();

    const QPointer group1_input = new QLineEdit();
    group1_input->setText(m_state.path);
    group1_layout->addWidget(group1_input);

    const QPointer group1_button = new QPushButton("Browse...");
    group1_layout->addWidget(group1_button);

    connect(group1_button, &QPushButton::clicked, [this, group1_input]() {
        const auto path =
            QFileDialog::getSaveFileName(this, "Export to SVG", group1_input->text(),
                                         "Scalable Vector Graphics (*.svg)");
        if (!path.isEmpty()) {
            group1_input->setText(path);
        }
    });

    group1->setLayout(group1_layout);
    layout->addWidget(group1);

    // .: Group 2 :: Overwrite guard :.
    // .:============================:.
    const QPointer group2 = new QGroupBox("Overwrite guard");
    const QPointer group2_layout = new QVBoxLayout();

    const QPointer group2_label = new QLabel("Path verification failed.");
    group2_layout->addWidget(group2_label);

    const QPointer group2_checkbox = new QCheckBox("Overwrite");
    group2_layout->addWidget(group2_checkbox);

    group2->setLayout(group2_layout);
    layout->addWidget(group2);

    // .: Group 3 :: Action after export :.
    // .:================================:.
    const QPointer group3 = new QGroupBox("Action after export");
    const QPointer group3_layout = new QHBoxLayout();

    const QPointer group3_radio_nothing = new QRadioButton("Nothing");
    const QPointer group3_radio_explorer = new QRadioButton("Show in explorer");
    const QPointer group3_radio_open = new QRadioButton("Open");
    group3_layout->addWidget(group3_radio_nothing);
    group3_layout->addWidget(group3_radio_explorer);
    group3_layout->addWidget(group3_radio_open);

    group3->setLayout(group3_layout);
    layout->addWidget(group3);

    // .: Export button :.
    // .:===============:.
    const QPointer button_export = new QPushButton("Export");
    layout->addWidget(button_export);
    connect(button_export, &QPushButton::clicked, [this, group1_input]() {
        m_state.path = group1_input->text();
        close();
    });

    setLayout(layout);
}
