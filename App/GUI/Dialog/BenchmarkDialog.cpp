#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "BenchmarkDialog.hpp"

BenchmarkDialog::BenchmarkDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Benchmark");

    const QPointer layout = new QVBoxLayout();

    m_intro_text = new QLabel("TODO");
    layout->addWidget(m_intro_text);

    // --- --- --- --- --- --- --- --- --- --- --- --- ---

    m_group_stats = new QGroupBox("Stats");
    const QPointer group_stats_layout = new QGridLayout();

    group_stats_layout->addWidget(new QLabel("Total nodes drawn: "), 0, 0, Qt::AlignRight);
    m_stats_total_nodes = new QLabel("?");
    group_stats_layout->addWidget(m_stats_total_nodes, 0, 1);
    group_stats_layout->addWidget(new QLabel("Working set size: "), 1, 0, Qt::AlignRight);
    m_stats_mem_usage_mib = new QLabel("?");
    group_stats_layout->addWidget(m_stats_mem_usage_mib, 1, 1);

    m_group_stats->setLayout(group_stats_layout);
    layout->addWidget(m_group_stats);

    // --- --- --- --- --- --- --- --- --- --- --- --- ---

    m_button_start = new QPushButton("Start benchmark");
    connect(m_button_start, &QPushButton::clicked, [this] {
        emit ButtonStartClicked();
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

void BenchmarkDialog::ChangeState(const bool is_benchmark_running) const
{
    m_intro_text->setVisible(!is_benchmark_running);
    m_group_stats->setVisible(is_benchmark_running);
    m_button_start->setVisible(!is_benchmark_running);
    m_button_stop->setVisible(is_benchmark_running);
}

void BenchmarkDialog::BenchmarkStatsRx(const int total_nodes, const double mem_usage_mib) const
{
    m_stats_total_nodes->setText(QString::number(total_nodes));
    m_stats_mem_usage_mib->setText(QString("%1 MiB").arg(mem_usage_mib, 0, 'f', 1));
}
