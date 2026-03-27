#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

class WidgetbenchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WidgetbenchDialog(QWidget* parent, bool is_opened_from_mainmenubar, int n_batches);
};
