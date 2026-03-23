#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSlider>
#include <QSplitter>
#include <QToolBar>

#include "../../Dependency/RSS.hpp"

#include "../GUI/Main/MainWindow.hpp"
#include "../GUI/Main/Viewer.hpp"
#include "../Helper/Color.hpp"

// (In this benchmark, nodes are being added to the canvas (they are added as pairs connected by arrow))
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
// (While benchmarking, we also scroll and zoom, so we have some movement)
// Amount of scrolling right after each node batch added
constexpr auto AUTO_SCROLL_STEP_X = 10;
// When to wrap to the beggining with the scrolling
constexpr auto AUTO_SCROLL_MODULO_X = 600;
// How many zoom levels we iterate, this corresponds to the slider and MW behavior
constexpr auto ZOOM_LEVEL_MODULO = 6;

void GUIMainWindow::SetGUIEnabled(const bool value) const
{
    menuBar()->setEnabled(value);

    m_toolbar_source_font_size->setEnabled(value);
    m_toolbar_source_cursor_position->setEnabled(value);
    // These can be always false (for the benchmark purposes)
    SetNodeToolbarsEnabled(false);

    centralWidget()->setEnabled(value);
}

QCoro::Task<> GUIMainWindow::BenchmarkStart()
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

    // Reserve string space
    // Can't do that with QPlainTextEdit?

    qDebug() << "Benchmark started.";
    // --- --- --- --- --- --- --- ---

    // Initialize helper variables
    int node_counter_total_pairs = 0;
    int node_counter_row_pairs = 0;
    int x_cor = 0;
    int y_cor = 0;
    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    int zoom_level = 0;
    int scrolling_x = 0;

    // Set up timer
    QTimer timer;
    timer.setInterval(TIME_INTERVAL);
    timer.start();

    // The benchmark
    while (true) {
        if (m_bench_stop_flag) {
            qDebug() << "Benchmark stopped.";
            SetGUIEnabled(true);
            setWindowModified(false);
            co_return;
        }
        // Do the next batch only when certain amount of time has passed
        co_await timer;

        // Zoom frenzy
        zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO; // 0,1,2,3,4,5
        m_secondary_canvas_toolbar_slider->setValue(zoom_level); // Callback will handle the zoom

        // Add a new batch of nodes
        for (int i = 0; i < N_NODES_IN_INTERVAL; i++) {
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
            // Update values for next iteration
            node_counter_total_pairs++;
            node_counter_row_pairs++;
            x_cor += X_COR_ADDITION;
            // Modify the color for next node; accepts number from 0 to 5 as last param to shuffle things up, we can use zoom level
            BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
        }

        // Auto scrolling
        scrolling_x += AUTO_SCROLL_STEP_X;
        if (scrolling_x > AUTO_SCROLL_MODULO_X) {
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
        const auto node_counter_total = 2 * node_counter_total_pairs;
        constexpr auto MIBI = 1024.0 * 1024.0;
        const auto mem_usage_mib = static_cast<double>(getCurrentRSS()) / MIBI;

        // Report to GUI
        m_state_benchmark_stats.scene_fps = m_scene_fps;
        m_state_benchmark_stats.total_nodes = node_counter_total;
        m_state_benchmark_stats.mem_usage_mib = mem_usage_mib;
        emit BenchmarkStatsCrateUpdated();

        // TODO LOG

        // End the benchmark check
        if (y_cor > MAX_Y_COR) {
            qDebug() << "Benchmark done.";
            break;
        }
    }

    // Enable everything
    SetGUIEnabled(true);
    setWindowModified(false);
}
