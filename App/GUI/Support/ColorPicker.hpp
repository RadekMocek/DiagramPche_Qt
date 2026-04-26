#pragma once

#include <QPushButton>

class ColorPicker : public QPushButton
{
    Q_OBJECT

public:
    explicit ColorPicker(QWidget* parent);

    // const QColor& GetColorRef() const { return m_color; }
    void SetColor(const QColor& new_color, bool do_emit_change);

private:
    QColor m_color;

signals:
    void ColorChanged(const QColor& new_color);
};
