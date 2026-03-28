#include <QPlainTextEdit>

#include "../../Dependency/RSS.hpp"

#include "../GUI/Main/MainWindow.hpp"
#include "../GUI/Dialog/Widgetbench.hpp"
#include "../Helper/BenchmarkCSV.hpp"
#include "../Helper/CPU.hpp"

QCoro::Task<> GUIMainWindow::WidgetbenchStart()
{
    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr auto DURATION_THRESHOLD_MS = 15000;

    // Maximize the window
    setWindowState(Qt::WindowMaximized);
    // Start measuring CPU usage
    CPUMeasureStart(); // This enables 'm_keep_measuring_CPU', set to false when done
    // Sleep for a bit
    co_await QCoro::sleepFor(std::chrono::milliseconds(16));

    // Vars
    // ReSharper disable once CppJoinDeclarationAndAssignment
    std::chrono::time_point<std::chrono::steady_clock> timestamp_window_queued;
    WidgetbenchLogResults log_data = {};
    int n_batches = 1;
    int batch_iter = 1;

    while (true) {
        // Sleep for a bit
        co_await QCoro::sleepFor(std::chrono::milliseconds(16));
        // Start time measure
        timestamp_window_queued = std::chrono::steady_clock::now();
        // Show the window
        WidgetbenchDialog dialog(this, false, n_batches);
        dialog.exec(); // Closing logic is in the dialog's ctor
        // --- --- --- --- --- --- --- --- --- --- --- ---
        // LOG N "ROWS"
        log_data.n_batches.push_back(n_batches);
        log_data.batch_iter.push_back(batch_iter);
        // LOG DURATION
        const auto duration_ms = ChronoTrigger(timestamp_window_queued).count();
        log_data.duration.push_back(duration_ms);
        // LOG RAM
        constexpr auto MIBI = 1024.0 * 1024.0;
        log_data.mem_mib.push_back(static_cast<double>(getCurrentRSS()) / MIBI);
        // LOG CPU
        log_data.cpu_usage.push_back(m_CPU_usage);
        // --- --- --- --- --- --- --- --- --- --- --- ---
        // Report progress
        m_source->setPlainText(QString("[node.\"%1 %2\"]").arg(n_batches).arg(batch_iter));
        // Prepare batch for the next iter
        batch_iter++;
        if (batch_iter > 9) {
            batch_iter = 0;
            n_batches *= 2;
        }
        // Widgetbench stop condition
        if (duration_ms > DURATION_THRESHOLD_MS) {
            // Save
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto filename = QString("./widgetbechres_Qt_%1_%2.csv")
                                  .arg(OS_ID).arg(GetUNIXTimestamp());

            if (WriteWidgetbenchResultsToCSV(filename.toUtf8(), log_data)) {
                qDebug() << "Benchmark data written to:" << filename;
            }
            else {
                qDebug() << "Error writing benchmark data to file.";
            }
            // Let know
            m_source->setPlainText("[node.\"Widget benchmark done\"]");
            setWindowModified(false); // no dirty
            // End
            m_keep_measuring_CPU = false;
            break;
        }
    }

    // Exit ?
    // ReSharper disable once CppRedundantBooleanExpressionArgument
    if (EXIT_AFTER_BENCHMARK_FROM_TERMINAL && m_is_benchmark_run_from_terminal) {
        close();
    }
}
