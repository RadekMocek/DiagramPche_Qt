#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSlider>
#include <QSplitter>
#include <QStackedWidget>
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
constexpr auto MAX_ROWS = 26;
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
constexpr auto BENCHMARK_HEAVY_N_NODES = 13230;

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
    const int textedit_width = window_width * TEXTEDIT_WIDTH_RATIO; // NOLINT(*-narrowing-conversions)
    m_splitter->setSizes({textedit_width, window_width - textedit_width});

    // Reserve string space if type == BENCHMARK_GRADUAL
    // Can't do that with QPlainTextEdit?

    // Maximize the window
    setWindowState(Qt::WindowMaximized);

    // Initialize helper variables
    auto node_counter_total = 0;
    int node_counter_total_pairs = 0;
    int gradual_node_id = 0;
    int x_cor = 0;
    int y_cor = 0;
    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    int zoom_level = 0;
    int scrolling_x = 0;
    int complete_bench_phase_n = 1;

    // Init log vars
    BenchmarkLogResults log_data{};
    CPUMeasureStart(); // This enables 'm_keep_measuring_CPU', set to false when done

    // Sleep for a bit
    co_await QCoro::sleepFor(std::chrono::milliseconds(500));

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
    else if (type == BENCHMARK_COMPLETE) {
        m_source->setPlainText("[node.\"Hang onto yer helmet!\\nThe complete benchmark has started...\"]\n");
    }

    const auto time_start = std::chrono::steady_clock::now();

    qDebug() << "Benchmark started.";

    while (true) {
        if (m_bench_stop_flag) {
            qDebug() << "Benchmark stopped.";
            SetGUIEnabled(true);
            setWindowModified(false); // no dirty
            m_keep_measuring_CPU = false;
            emit BenchmarkDone();
            co_return;
        }
        // Do the next batch only when certain amount of time has passed
        co_await timer;

        // Keep dialog top left (dialog is not visible if benchmark started from the terminal, so the check is necessary
        if (m_benchmark_dialog) {
            m_benchmark_dialog->move(m_benchmark_dialog->parentWidget()->geometry().topLeft());
        }

        // Zoom frenzy
        zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO; // 0,1,2,3,4,5
        m_secondary_canvas_toolbar_slider->setValue(zoom_level); // Callback will handle the zoom

        const auto is_gradual =
            type == BENCHMARK_GRADUAL
            || (type == BENCHMARK_COMPLETE && complete_bench_phase_n == 2);

        if (is_gradual) {
            QString batch;
            batch.reserve(188);
            for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
                const auto z = gradual_node_id % Z_MODULO;
                batch += QString(
                             "[node.\"A%1\"]\nxy=[%2,%3]\nz=%4\ncolor=[%5,%6,%7,128]\n"
                             "[node.\"B%8\"]\nxy=[\"A%9\",\"bottom-right\",10,10]\nz=%10\ntype=\"ellipse\"\n"
                             "[[path]]\nstart=[\"A%11\",\"left\",0,0]\nend=[\"B%12\",\"right\",0,0]\n"
                         )
                         .arg(node_counter_total_pairs).arg(x_cor).arg(y_cor).arg(z)
                         .arg(color_r).arg(color_g).arg(color_b)
                         .arg(node_counter_total_pairs).arg(node_counter_total_pairs).arg(z)
                         .arg(node_counter_total_pairs).arg(node_counter_total_pairs);

                node_counter_total_pairs++;
                gradual_node_id++;
                x_cor += X_COR_ADDITION;
                BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
            }
            m_source->appendPlainText(batch);
            setWindowModified(false); // no dirty
        }
        else {
            for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
                gradual_node_id++;
                x_cor += X_COR_ADDITION;
                BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
            }
        }

        // Auto scrolling
        scrolling_x -= AUTO_SCROLL_STEP_X;
        if (scrolling_x < -AUTO_SCROLL_MODULO_X) {
            scrolling_x = 0;
        }
        m_viewer->ResetCanvasScrolling(scrolling_x, 0);

        // Jump to new row if needed
        if (gradual_node_id > MAX_NODES_ON_ROW) {
            gradual_node_id = 0;
            x_cor = 0;
            y_cor += Y_COR_ADDITION;
        }

        // Stats
        if (is_gradual) {
            node_counter_total = 2 * node_counter_total_pairs;
        }

        if (zoom_level % 3 == 1) {
            constexpr auto MIBI = 1024.0 * 1024.0;
            const auto mem_usage_mib = static_cast<double>(getCurrentRSS()) / MIBI;

            // Report to GUI
            m_state_benchmark_stats.scene_fps = m_scene_fps;
            m_state_benchmark_stats.total_nodes = node_counter_total;
            m_state_benchmark_stats.mem_usage_mib = mem_usage_mib;
            m_state_benchmark_stats.cpu_usage_system = m_CPU_usage;
            emit BenchmarkStatsCrateUpdated();

            // LOG
            log_data.timestamp.push_back(ChronoTrigger(time_start).count());
            log_data.fps.push_back(static_cast<float>(m_scene_fps));
            log_data.n_nodes.push_back(node_counter_total);
            log_data.mem_mib.push_back(mem_usage_mib);
            log_data.cpu_usage.push_back(m_CPU_usage);
        }

        // End the benchmark check
        if (y_cor > MAX_Y_COR) {
            if (type == BENCHMARK_COMPLETE && complete_bench_phase_n < 3) {
                complete_bench_phase_n++;
                x_cor = 0;
                y_cor = 0;
            }
            else {
                qDebug() << "Benchmark done.";

                const auto bench_id = std::format("b{}", static_cast<int>(type));

                auto bench_info = (m_highlighter_light->document() || m_highlighter_dark->document())
                                      ? "shon"
                                      : "shoff";
                if (m_source_wrapper->currentIndex() == 1) {
                    bench_info = "txoff";
                }

                // ReSharper disable once CppTooWideScopeInitStatement
                const auto filename = QString("./bnchres_Qt_%1_%2_%3_%4.csv")
                                      .arg(OS_ID).arg(bench_id).arg(bench_info).arg(GetUNIXTimestamp());

                if (WriteBenchmarkResultsToCSV(filename.toUtf8(), log_data)) {
                    qDebug() << "Benchmark data written to:" << filename;
                }
                else {
                    qDebug() << "Error writing benchmark data to file.";
                }
                break;
            }
        }
    }

    // Enable everything
    SetGUIEnabled(true);
    setWindowModified(false); // no dirty
    m_keep_measuring_CPU = false;
    emit BenchmarkDone();
    // Exit ?
    // ReSharper disable once CppRedundantBooleanExpressionArgument
    if (EXIT_AFTER_BENCHMARK_FROM_TERMINAL && m_is_benchmark_run_from_terminal) {
        close();
    }
}

void GUIMainWindow::OnSyntaxHighlightSwitchRequest()
{
    const auto is_window_modified = isWindowModified();

    if (m_is_color_theme_light) {
        if (m_highlighter_light->document()) {
            m_highlighter_light->setDocument(nullptr);
            m_state_dialog_preferences.is_syntax_highlight_enabled = false;
        }
        else {
            m_highlighter_light->setDocument(m_source->document());
            m_state_dialog_preferences.is_syntax_highlight_enabled = true;
        }
    }
    else {
        if (m_highlighter_dark->document()) {
            m_highlighter_dark->setDocument(nullptr);
            m_state_dialog_preferences.is_syntax_highlight_enabled = false;
        }
        else {
            m_highlighter_dark->setDocument(m_source->document());
            m_state_dialog_preferences.is_syntax_highlight_enabled = true;
        }
    }

    // Changing highlighter will mark document as dirty, so we undo it like this
    if (!is_window_modified) {
        QTimer::singleShot(0, this, [this] {
            setWindowModified(false);
        });
    }
}

void GUIMainWindow::OnTextEditVisibilitySwitchRequest() const
{
    if (m_source_wrapper->currentIndex() == 0) {
        m_source_wrapper->setCurrentIndex(1);
    }
    else {
        m_source_wrapper->setCurrentIndex(0);
    }
}
