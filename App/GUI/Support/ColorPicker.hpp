#pragma once

#include <QPushButton>

class ColorPicker : public QPushButton
{
    Q_OBJECT

public:
    explicit ColorPicker(QWidget* parent);

    const QColor& GetColorRef() const;
    void SetColor(const QColor& new_color);

private:
    QColor m_color;
};
