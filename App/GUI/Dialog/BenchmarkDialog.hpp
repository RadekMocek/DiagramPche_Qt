#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
QT_END_NAMESPACE

// === === === === === === === === ===

class BenchmarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BenchmarkDialog(QWidget* parent);

private:
    void ChangeState(bool is_benchmark_running) const;

    QPointer<QLabel> m_intro_text;
    QPointer<QPushButton> m_button_start;
    QPointer<QPushButton> m_button_stop;
    QPointer<QGroupBox> m_group_stats;
    QPointer<QLabel> m_stats_total_nodes;
    QPointer<QLabel> m_stats_mem_usage_mib;

signals:
    void ButtonStartClicked();
    void ButtonStopClicked();

public slots:
    void BenchmarkStatsRx(int total_nodes, double mem_usage_mib) const;
};
