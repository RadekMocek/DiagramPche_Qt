#include <QColorDialog>

#include "ColorPicker.hpp"

ColorPicker::ColorPicker(QWidget* parent) : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, [this] {
        if (const auto new_color = QColorDialog::getColor(m_color, parentWidget());
            new_color.isValid() && new_color != m_color) {
            SetColor(new_color, true);
        }
    });
}

void ColorPicker::SetColor(const QColor& new_color, const bool do_emit_change)
{
    m_color = new_color;
    setStyleSheet("background-color: " + m_color.name());
    if (do_emit_change) {
        emit ColorChanged(new_color);
    }
}
