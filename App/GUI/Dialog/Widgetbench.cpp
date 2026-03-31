#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QTimer>

#include "Widgetbench.hpp"
#include "../../Helper/Color.hpp"

constexpr auto N_BATCH_LABELS = 20;
constexpr auto N_BATCH_TEXTEDITS = 20;
constexpr auto N_BATCH_BUTTONS = 20;
constexpr auto N_BATCH_CHECKBOXES = 48;
constexpr auto N_BATCH_RADIOS = 48;
constexpr auto N_BATCH_SLIDERS = 16;
//
constexpr auto N_BATCH_WIDGETS
    = N_BATCH_LABELS
    + N_BATCH_TEXTEDITS
    + N_BATCH_BUTTONS
    + N_BATCH_CHECKBOXES
    + N_BATCH_RADIOS
    + N_BATCH_SLIDERS;
//


WidgetbenchDialog::WidgetbenchDialog(QWidget* parent, const bool is_opened_from_mainmenubar, const int n_batches) :
    QDialog(parent)
{
    setWindowTitle(QString("Widget spree %1 x %2").arg(n_batches).arg(N_BATCH_WIDGETS));

    move(parentWidget()->geometry().topLeft());

    const QPointer layout = new QGridLayout();

    int row = 0;

    if (is_opened_from_mainmenubar) {
        QPalette error_text_palette;
        error_text_palette.setColor(QPalette::WindowText, COLOR_ERROR);
        const QPointer info = new QLabel(
            "This benchmark is run from terminal: '.\\DiagramPche_Qt.exe w'\n"
            "[!] epilepsy warning: this window will flicker during the benchmark"
        );
        info->setPalette(error_text_palette);
        layout->addWidget(info, row++, 0, 1, 24);

        const QPointer close = new QPushButton("Close");
        connect(close, SIGNAL(clicked()), this, SLOT(close()));
        layout->addWidget(close, row++, 0, 1, 3);
    }
    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    for (int b = 0; b < n_batches; b++) {
        constexpr int COLSPAN = 3;
        int col = 0;
        for (int i = 0; i < N_BATCH_LABELS; i++) {
            layout->addWidget(new QLabel("ABCČDĎEF"), row, col, 1, COLSPAN);
            col += COLSPAN;
        }
        row++;
        col = 0;
        for (int i = 0; i < N_BATCH_TEXTEDITS; i++) {
            layout->addWidget(new QLineEdit("GHCHIJK"), row, col, 1, COLSPAN);
            col += COLSPAN;
        }
        row++;
        col = 0;
        for (int i = 0; i < N_BATCH_BUTTONS; i++) {
            layout->addWidget(new QPushButton("LMNŇOPQ"), row, col, 1, COLSPAN);
            col += COLSPAN;
        }
        row++;
        for (int i = 0; i < N_BATCH_CHECKBOXES; i++) {
            const QPointer checkbox = new QCheckBox("R");
            checkbox->setChecked(true);
            layout->addWidget(checkbox, row, i, 1, 2);
        }
        row++;
        for (int i = 0; i < N_BATCH_RADIOS; i++) {
            const QPointer radio = new QRadioButton("Ř");
            radio->setChecked(true);
            layout->addWidget(radio, row, i, 1, 2);
        }
        row++;
        col = 0;
        for (int i = 0; i < N_BATCH_SLIDERS; i++) {
            const QPointer slider = new QSlider(Qt::Horizontal);
            slider->setMinimum(0);
            slider->setMaximum(9);
            slider->setValue(5);
            layout->addWidget(slider, row, col, 1, COLSPAN);
            col += COLSPAN;
        }
        row++;
    }
    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    setLayout(layout);

    if (!is_opened_from_mainmenubar) {
        QTimer::singleShot(0, this, SLOT(close()));
    }
}
