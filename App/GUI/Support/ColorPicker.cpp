#include <QColorDialog>

#include "ColorPicker.hpp"

ColorPicker::ColorPicker(QWidget* parent) : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, [this] {
        if (const auto new_color = QColorDialog::getColor(m_color, parentWidget());
            new_color != m_color) {
            SetColor(new_color);
        }
    });
}

const QColor& ColorPicker::GetColorRef() const
{
    return m_color;
}

void ColorPicker::SetColor(const QColor& new_color)
{
    m_color = new_color;
    setStyleSheet("background-color: " + m_color.name());
}
