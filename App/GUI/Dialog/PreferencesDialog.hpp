#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent);
};
