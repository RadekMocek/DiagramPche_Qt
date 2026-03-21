#include <QPlainTextEdit>
#include <QSlider>

#include "../GUI/Main/MainWindow.hpp"
#include "../Helper/Color.hpp"

// (In this benchmark, nodes are being added to the canvas (they are added as pairs connected by arrow))
// After this passes, add new batch of nodes
constexpr auto TIME_INTERVAL = 0.3f;
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

QCoro::Task<> GUIMainWindow::BenchmarkStart()
{
    qDebug() << "bench started";

    // Disable everything
    setEnabled(false);

    // Clear the source
    HandleRegularNew();

    QTimer timer;
    timer.setInterval(300);
    timer.start();

    // The benchmark
    for (int i = 0; i < 100000; i++) {
        co_await timer;
        m_source->appendPlainText(QString("[node.\"%1\"]\nxy=[%1,0]").arg(i));
    }

    // Enable everything
    setEnabled(true);

    qDebug() << "bench ended";
}

/*
void GUIMainWindow::BenchmarkStart()
{
    qDebug() << "bench started";

    // Disable everything
    setEnabled(false);

    // Clear the source
    HandleRegularNew();

    // Initialize helper variables
    int node_counter_total_pairs = 0;
    int node_counter_row_pairs = 0;
    int x_cor = 0;
    int y_cor = 0;
    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    int zoom_level = 0;

    // The benchmark
    while (true) {
        // Zoom frenzy
        zoom_level = (zoom_level + 1) % ZOOM_LEVEL_MODULO;
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

            node_counter_total_pairs++;
            node_counter_row_pairs++;
            x_cor += X_COR_ADDITION;
            // Modify the color for next node; accepts number from 0 to 5 as last param to shuffle things up, we can use zoom level
            BenchmarkChangeColor(color_r, color_g, color_b, zoom_level);
        }

        // Scrolling
        //todo

        // Jump to new row if needed
        if (node_counter_row_pairs > MAX_NODES_ON_ROW) {
            node_counter_row_pairs = 0;
            x_cor = 0;
            y_cor += Y_COR_ADDITION;
        }

        // End the benchmark check
        if (y_cor > MAX_Y_COR) {
            qDebug() << "bench ended";
            break;
        }
    }

    // Enable everything
    setEnabled(true);
}
*/
