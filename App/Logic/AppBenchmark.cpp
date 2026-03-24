#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSlider>
#include <QSplitter>
#include <QToolBar>

#include "../../Dependency/RSS.hpp"

#include "../GUI/Main/MainWindow.hpp"
#include "../GUI/Main/Viewer.hpp"
#include "../Helper/BenchmarkCSV.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/CPU.hpp"

// (In benchmark type GRADUAL, nodes are being added to the canvas (they are added as pairs connected by arrow))
// (While benchmarking, we also scroll and zoom, so we have some movement)
// (Benchmarks type LIGHT and HEAVY have prepared TOML and just do scroll and zoom above them)
// What percentage of the window's width will the text editor occupy during the benchmark
constexpr auto TEXTEDIT_WIDTH_RATIO = 0.28f;
// After this passes, add new batch of nodes
constexpr auto TIME_INTERVAL = 300; // ms
// How many nodes to add in a batch?
constexpr auto N_NODES_IN_INTERVAL = 35;
// Add this to each node x coordinate
constexpr auto X_COR_ADDITION = 12;
// How many Z layers we use? Each node has Z one greater than the previous, moduled by this
constexpr auto Z_MODULO = N_DL_USER_CHANNELS;
// How many nodes on a row we want?
constexpr auto MAX_NODES_ON_ROW = 220;
// When we reach `MAX_NODES_ON_ROW`, we go on a new row, this is the offset of the new row
constexpr auto Y_COR_ADDITION = 100;
// How many rows do we want? When we have this much of rows, benchmark ends
constexpr auto MAX_ROWS = 21;
// Used for the ending condition
constexpr auto MAX_Y_COR = Y_COR_ADDITION * MAX_ROWS;
// Amount of scrolling right after each node batch added
constexpr auto AUTO_SCROLL_STEP_X = 10;
// When to wrap to the beggining with the scrolling
constexpr auto AUTO_SCROLL_MODULO_X = 600;
// How many zoom levels we iterate, this corresponds to the slider and MW behavior
constexpr auto ZOOM_LEVEL_MODULO = 6;
// Precalculated
constexpr auto BENCHMARK_LIGHT_N_NODES = 12;
constexpr auto BENCHMARK_HEAVY_N_NODES = 10780;

void GUIMainWindow::SetGUIEnabled(const bool value) const
{
    menuBar()->setEnabled(value);

    m_toolbar_source_font_size->setEnabled(value);
    m_toolbar_source_cursor_position->setEnabled(value);
    // These can be always false (for the benchmark purposes)
    SetNodeToolbarsEnabled(false);

    centralWidget()->setEnabled(value);
}

QCoro::Task<> GUIMainWindow::BenchmarkStart(const BenchmarkType type)
{
    // Update state
    m_bench_stop_flag = false;

    // Clear the source and reset the view
    HandleRegularNew();
    m_viewer->ResetCanvasScrolling(0, 0);

    // Disable everything
    SetGUIEnabled(false);

    // Change the ratio between textedit and canvas to make canvas bigger (more things to see)
    const auto window_width = width();
    const int textedit_width = window_width * TEXTEDIT_WIDTH_RATIO;
    m_splitter->setSizes({textedit_width, window_width - textedit_width});

    // Reserve string space if type == BENCHMARK_GRADUAL
    // Can't do that with QPlainTextEdit?

    // Maximize the window
    setWindowState(Qt::WindowMaximized);

    // Initialize helper variables
    auto node_counter_total = 0;
    int node_counter_total_pairs = 0;
    int node_counter_row_pairs = 0;
    int x_cor = 0;
    int y_cor = 0;
    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    int zoom_level = 0;
    int scrolling_x = 0;

    // Init log vars
    int log_index = 0;
    BenchmarkLogResults log_data{};

    // Set up timer
    QTimer timer;
    timer.setInterval(TIME_INTERVAL);
    timer.start();

    // --- --- --- --- --- --- --- ---
    // The benchmark
    if (type == BENCHMARK_LIGHT) {
        HandleOpenExample(BENCHMARK_LIGHT_PATH);
        node_counter_total = BENCHMARK_LIGHT_N_NODES;
    }
    else if (type == BENCHMARK_HEAVY) {
        HandleOpenExample(BENCHMARK_HEAVY_PATH);
        node_counter_total = BENCHMARK_HEAVY_N_NODES;
    }

    const auto time_start = std::chrono::steady_clock::now();

    qDebug() << "Benchmark started.";

    while (true) {
        if (m_bench_stop_flag) {
            qDebug() << "Benchmark stopped.";
            SetGUIEnabled(true);
            setWindowModified(false);
            emit BenchmarkDone();
            co_return;
        }
        // Do the next batch only when certain amount of time has passed
        co_await timer;

        // Keep dialog top left
        m_benchmark_dialog->move(m_benchmark_dialog->parentWidget()->geometry().topLeft());

        // Zoom frenzy
        zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO; // 0,1,2,3,4,5
        m_secondary_canvas_toolbar_slider->setValue(zoom_level); // Callback will handle the zoom

        // Add a new batch of nodes
        for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
            if (type == BENCHMARK_GRADUAL) {
                const auto z = node_counter_row_pairs % Z_MODULO;
                m_source->appendPlainText(
                    QString(
                        "[node.\"A%1\"]\nxy=[%2,%3]\nz=%4\ncolor=[%5,%6,%7,128]\n"
                        "[node.\"B%8\"]\nxy=[\"A%9\",\"bottom-right\",10,10]\nz=%10\ntype=\"ellipse\"\n"
                        "[[path]]\nstart=[\"A%11\",\"left\",0,0]\nend=[\"B%12\",\"right\",0,0]\n"
                    )
                    .arg(node_counter_total_pairs).arg(x_cor).arg(y_cor).arg(z).arg(color_r).arg(color_g).arg(color_b)
                    .arg(node_counter_total_pairs).arg(node_counter_total_pairs).arg(z)
                    .arg(node_counter_total_pairs).arg(node_counter_total_pairs)
                );
            }
            // Update values for next iteration
            node_counter_total_pairs++;
            node_counter_row_pairs++;
            x_cor += X_COR_ADDITION;
            // Modify the color for next node; accepts number from 0 to 5 as last param to shuffle things up, we can use zoom level
            BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
        }

        // Auto scrolling
        scrolling_x -= AUTO_SCROLL_STEP_X;
        if (scrolling_x < -AUTO_SCROLL_MODULO_X) {
            scrolling_x = 0;
        }
        m_viewer->ResetCanvasScrolling(scrolling_x, 0);

        // Jump to new row if needed
        if (node_counter_row_pairs > MAX_NODES_ON_ROW) {
            node_counter_row_pairs = 0;
            x_cor = 0;
            y_cor += Y_COR_ADDITION;
        }

        // Stats
        if (type == BENCHMARK_GRADUAL) {
            node_counter_total = 2 * node_counter_total_pairs;
        }

        if (zoom_level % 3 == 1) {
            constexpr auto MIBI = 1024.0 * 1024.0;
            const auto mem_usage_mib = static_cast<double>(getCurrentRSS()) / MIBI;
            const auto cpu_usage = CPUStats::GetCurrentValue();

            // Report to GUI
            m_state_benchmark_stats.scene_fps = m_scene_fps;
            m_state_benchmark_stats.total_nodes = node_counter_total;
            m_state_benchmark_stats.mem_usage_mib = mem_usage_mib;
            m_state_benchmark_stats.cpu_usage_system = cpu_usage;
            emit BenchmarkStatsCrateUpdated();

            // LOG
            log_data.timestamp[log_index] = ChronoTrigger(time_start).count();
            log_data.fps[log_index] = m_scene_fps;
            log_data.n_nodes[log_index] = node_counter_total;
            log_data.mem_mib[log_index] = mem_usage_mib;
            log_data.cpu_usage[log_index] = cpu_usage;
            log_index++;
        }

        // End the benchmark check
        if (y_cor > MAX_Y_COR) {
            qDebug() << "Benchmark done.";
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto filename = QString("./BenchStats_Qt_%1_%2.csv").arg(type).arg(GetUNIXTimestamp());

            if (WriteBenchmarkResultsToCSV(filename.toUtf8(), log_data)) {
                qDebug() << "Benchmark data written to:" << filename;
            }
            else {
                qDebug() << "Error writing benchmark data to file.";
            }
            break;
        }
    }

    // Enable everything
    SetGUIEnabled(true);
    setWindowModified(false);
    emit BenchmarkDone();
}

void GUIMainWindow::OnSyntaxHighlightSwitchRequest() const
{
    if (m_highlighter->document()) {
        m_highlighter->setDocument(nullptr);
    }
    else {
        m_highlighter->setDocument(m_source->document());
    }
}
