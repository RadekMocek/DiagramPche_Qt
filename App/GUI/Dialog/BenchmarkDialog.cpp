#include <QCloseEvent>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "BenchmarkDialog.hpp"
#include "../../Helper/Color.hpp"

BenchmarkDialog::BenchmarkDialog(QWidget* parent, BenchmarkStatsState& crate) :
    QDialog(parent),
    m_is_benchmark_running(false),
    m_crate(crate)
{
    setWindowTitle("Benchmark");

    const QPointer layout = new QVBoxLayout();

    m_group_init = new QGroupBox();
    const QPointer group_init_layout = new QVBoxLayout();

    QPalette error_text_palette;
    error_text_palette.setColor(QPalette::WindowText, COLOR_ERROR);
    const QPointer warning_label = new QLabel(
        "Save your work before running the benchmark.\nRunning the benchmark will discard the current document.\n"
    );
    warning_label->setPalette(error_text_palette);
    group_init_layout->addWidget(warning_label);

    group_init_layout->addWidget(new QLabel("Syntax highlight may affect performance:"));
    const QPointer button_syntax_highlight = new QPushButton("Syntax highlight on/off");
    connect(button_syntax_highlight, &QPushButton::clicked, this, &BenchmarkDialog::ButtonSwitchSyntaxHighlightClicked);
    group_init_layout->addWidget(button_syntax_highlight);

    group_init_layout->addWidget(new QLabel("\nChoose one of the three benchmarks:"));
    m_combo_benchmark_type = new QComboBox();
    m_combo_benchmark_type->addItems(BENCHMARK_TYPE_NAMES);
    group_init_layout->addWidget(m_combo_benchmark_type);

    m_group_init->setLayout(group_init_layout);
    layout->addWidget(m_group_init);

    // --- --- --- --- --- --- --- --- --- --- --- --- ---

    m_progress_bar = new QProgressBar();
    // All zeroes to make "endless" loading (undetermined progress bar)
    m_progress_bar->setRange(0, 0);
    layout->addWidget(m_progress_bar);

    m_group_stats = new QGroupBox("Stats");
    const QPointer group_stats_layout = new QGridLayout();

    const auto AddRow = [group_stats_layout](const char* name, QPointer<QLabel>& member, const int row_n) {
        group_stats_layout->addWidget(new QLabel(name), row_n, 0, Qt::AlignRight);
        member = new QLabel("?");
        group_stats_layout->addWidget(member, row_n, 1);
    };

    AddRow("Scene framerate: ", m_stats_scene_fps, 0);
    AddRow("Total nodes drawn: ", m_stats_total_nodes, 1);
    AddRow("Working set size: ", m_stats_mem_usage_mib, 2);
    AddRow("System CPU usage: ", m_stats_cpu_usage, 3);

    m_group_stats->setLayout(group_stats_layout);
    layout->addWidget(m_group_stats);

    // --- --- --- --- --- --- --- --- --- --- --- --- ---

    layout->addSpacerItem(new QSpacerItem(0, 8));

    m_button_start = new QPushButton("Start benchmark");
    connect(m_button_start, &QPushButton::clicked, [this] {
        emit ButtonStartClicked(static_cast<BenchmarkType>(m_combo_benchmark_type->currentIndex()));
        ChangeState(true);
    });
    layout->addWidget(m_button_start);

    m_button_stop = new QPushButton("Stop");
    m_button_stop->setVisible(false);
    connect(m_button_stop, &QPushButton::clicked, [this] {
        emit ButtonStopClicked();
        ChangeState(false);
    });
    layout->addWidget(m_button_stop);

    setLayout(layout);
    ChangeState(false);
}

void BenchmarkDialog::ChangeState(const bool is_benchmark_running)
{
    m_is_benchmark_running = is_benchmark_running;

    m_group_init->setVisible(!is_benchmark_running);
    m_button_start->setVisible(!is_benchmark_running);

    m_progress_bar->setVisible(is_benchmark_running);
    m_group_stats->setVisible(is_benchmark_running);
    m_button_stop->setVisible(is_benchmark_running);

    if (is_benchmark_running) {
        // Hide close button
        setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        show();
    }
    else {
        // Restore close button
        setWindowFlags(Qt::Dialog);
        show();
    }
}

void BenchmarkDialog::OnBenchmarkStatsCrateUpdate() const
{
    m_stats_scene_fps->setText(QString("%1 FPS").arg(m_crate.scene_fps));
    m_stats_total_nodes->setText(QString::number(m_crate.total_nodes));
    m_stats_mem_usage_mib->setText(QString("%1 MiB").arg(m_crate.mem_usage_mib, 0, 'f', 1));
    m_stats_cpu_usage->setText(QString("%1 %").arg(m_crate.cpu_usage_system, 0, 'f', 1));
}

void BenchmarkDialog::OnBenchmarkDone()
{
    ChangeState(false);
}

void BenchmarkDialog::closeEvent(QCloseEvent* event)
{
    if (m_is_benchmark_running) {
        event->ignore();
    }
    else {
        event->accept();
    }
}
