#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

// === === === === === === === === ===

struct BenchmarkStatsState
{
    int scene_fps;
    int total_nodes;
    double mem_usage_mib;
};

// === === === === === === === === ===

class BenchmarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BenchmarkDialog(QWidget* parent, bool is_saved, BenchmarkStatsState& crate);

private:
    void ChangeState(bool is_benchmark_running);

    void closeEvent(QCloseEvent* event) override;

    bool m_is_benchmark_running;

    BenchmarkStatsState& m_crate;

    QPointer<QLabel> m_intro_text;
    QPointer<QPushButton> m_button_start;
    QPointer<QPushButton> m_button_stop;
    QPointer<QProgressBar> m_progress_bar;
    QPointer<QGroupBox> m_group_stats;
    QPointer<QLabel> m_stats_scene_fps;
    QPointer<QLabel> m_stats_total_nodes;
    QPointer<QLabel> m_stats_mem_usage_mib;
    QPointer<QLabel> m_stats_cpu_usage;

signals:
    void ButtonStartClicked();
    void ButtonStopClicked();

public slots:
    void OnBenchmarkStatsCrateUpdate() const;
};
