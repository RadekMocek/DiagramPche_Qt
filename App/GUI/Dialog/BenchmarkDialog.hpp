#pragma once

// Qt imports
#include <QDialog>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QComboBox;
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
    float cpu_usage_system;
};

// === === === === === === === === ===

enum BenchmarkType
{
    BENCHMARK_LIGHT, BENCHMARK_HEAVY, BENCHMARK_GRADUAL, BENCHMARK_COMPLETE
};

const QStringList BENCHMARK_TYPE_NAMES = {"Light", "Heavy", "Gradual", "Complete 3in1"};

// === === === === === === === === ===

class BenchmarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BenchmarkDialog(QWidget* parent, BenchmarkStatsState& crate);

private:
    void ChangeState(bool is_benchmark_running);

    void closeEvent(QCloseEvent* event) override;

    bool m_is_benchmark_running;

    BenchmarkStatsState& m_crate;

    QPointer<QGroupBox> m_group_init;
    QPointer<QPushButton> m_button_start;
    QPointer<QComboBox> m_combo_benchmark_type;

    QPointer<QProgressBar> m_progress_bar;
    QPointer<QGroupBox> m_group_stats;
    QPointer<QLabel> m_stats_scene_fps;
    QPointer<QLabel> m_stats_total_nodes;
    QPointer<QLabel> m_stats_mem_usage_mib;
    QPointer<QLabel> m_stats_cpu_usage;
    QPointer<QPushButton> m_button_stop;

signals:
    void ButtonStartClicked(BenchmarkType type);
    void ButtonStopClicked();
    void ButtonSwitchSyntaxHighlightClicked();
    void ButtonSwitchTextEditVisibilityClicked();

public slots:
    void OnBenchmarkStatsCrateUpdate() const;
    void OnBenchmarkDone();
};
