#include <filesystem>

#include <QDesktopServices>
#include <QComboBox>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QSvgGenerator>
#include <QToolBar>

#include "MainWindow.hpp"
#include "../../Helper/Color.hpp"
#include "../../Welcome.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "Scene.hpp"
#include "Viewer.hpp"

GUIMainWindow::GUIMainWindow()
{
    setWindowTitle("Untitled – DiagramPche :: Qt");
    resize(1280, 800);

    InitMainMenuBar();
    InitCentralWidget();
    InitToolbar();
    InitState();

    ParseAndRedraw();
}

// == Logic ==

void GUIMainWindow::ParseAndRedraw()
{
    m_parser.Parse(m_source->toPlainText().toStdString());
    m_scene->Redraw(m_parser.m_result_nodes_pq, m_parser.m_result_nodes, m_parser.m_result_paths);

    if (m_parser.m_is_error) {
        auto err_str = QString::fromStdString(m_parser.m_error_description);
        err_str.replace("\n", " ");
        m_error_label->setText(err_str);
        ErrorHighlight(m_parser.m_error_source_region);
    }
    else {
        m_error_label->setText("");
        m_source->setExtraSelections(QList<QTextEdit::ExtraSelection>()); // Disable error highlight if any
    }
}

void GUIMainWindow::ExportToSvg() const
{
    constexpr auto SVG_PADDING = 25;
    // Qt has the ability to save content of QGraphicsScene into a SVG file
    QSvgGenerator svg_generator;

    // We need to calculate the SVG canvas size, so the diagram occupies 100% of the canvas (excluding SVG_PADDING)
    const auto scene_aabr = m_scene->GetSceneAABR().toRect();
    const auto scene_aabr_size = scene_aabr.size();
    const QRect viewbox(scene_aabr.left() - SVG_PADDING,
                        scene_aabr.top() - SVG_PADDING,
                        scene_aabr_size.width() + 2 * SVG_PADDING,
                        scene_aabr.height() + 2 * SVG_PADDING);

    svg_generator.setSize(scene_aabr_size);
    svg_generator.setViewBox(viewbox);
    svg_generator.setFileName(m_state_dialog_export.path);

    // Associate painter with SvgGenerator and tell the scene to render itself using that painter
    QPainter painter(&svg_generator);

    // If path is invalid, we can react to that here; show error and stop the export
    if (!painter.isActive()) {
        QMessageBox::critical(nullptr, "Export to SVG failed",
                              "SVG file could not be created:\n" + m_state_dialog_export.path);
        return;
    }

    m_scene->render(&painter, QRectF(viewbox), QRectF(viewbox), Qt::IgnoreAspectRatio);
    painter.end();

    // Handle action after export
    if (m_state_dialog_export.action == ActionAfterExport_OpenFolder) {
        const QFileInfo file_info(m_state_dialog_export.path);
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.absolutePath()));
    }
    else if (m_state_dialog_export.action == ActionAfterExport_OpenFile) {
        QDesktopServices::openUrl(QUrl("file:///" + m_state_dialog_export.path, QUrl::TolerantMode));
    }
}

void GUIMainWindow::ErrorHighlight(const toml::source_region& EH_region) const
{
    const auto error_y = static_cast<int>(EH_region.begin.line - 1);
    const auto error_x_start = static_cast<int>(EH_region.begin.column - 1);
    const auto error_x_length = static_cast<int>(EH_region.end.column - error_x_start - 1);

    QTextCursor cursor = m_source->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, error_y);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, error_x_start);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, error_x_length);

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(COLOR_ERROR_HIGHLIGHT);
    // With this, error length of 0 will highlight whole line
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;

    m_source->setExtraSelections(QList({selection}));
}

// == GUI init ==

void GUIMainWindow::InitMainMenuBar()
{
    // --- Main menu bar config -- --- --- --- ---
    constexpr auto FONT_SIZE_MAIN_MENU_BAR = 18;
    // --- --- --- --- --- --- --- --- --- --- ---

    const QPointer main_menu_bar = new QMenuBar(this);

    QFont main_menu_font = main_menu_bar->font();
    main_menu_font.setPixelSize(FONT_SIZE_MAIN_MENU_BAR);
    main_menu_bar->setFont(main_menu_font);

    setMenuBar(main_menu_bar);

    // .: File :.
    const QPointer file_menu = main_menu_bar->addMenu("File");
    // . New .
    const QPointer file_new_action = file_menu->addAction("New");
    //todo
    // . Open .
    const QPointer file_open_action = file_menu->addAction("Open");
    //todo
    // . Save .
    const QPointer file_save_action = file_menu->addAction("Save");
    //todo
    // . Save as .
    const QPointer file_saveas_action = file_menu->addAction("Save as");
    //todo
    // . Export to SVG .
    const QPointer file_export_svg_action = file_menu->addAction("Export to SVG");
    connect(file_export_svg_action, &QAction::triggered, [this] {
        ExportSVGDialog dialog(this, m_state_dialog_export);
        connect(&dialog, &ExportSVGDialog::ButtonExportClicked, this, &GUIMainWindow::ExportToSvg);
        dialog.exec();
    });
    file_menu->addSeparator();

    // . Preferences .
    const QPointer file_preferences_action = file_menu->addAction("Preferences");
    connect(file_preferences_action, &QAction::triggered, [this] {
        if (m_preferences_dialog) {
            m_preferences_dialog->raise();
            m_preferences_dialog->activateWindow();
            return;
        }
        m_preferences_dialog = new PreferencesDialog(this);
        m_preferences_dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_preferences_dialog->show();
    });
    file_menu->addSeparator();
    // . Exit .
    const QPointer file_exit_action = file_menu->addAction("Exit");
    connect(file_exit_action, SIGNAL(triggered()), this, SLOT(close()));

    // .: View :.
    const QPointer view_menu = main_menu_bar->addMenu("View");
    // . Toolbar .
    const QPointer view_canvas_toolbar_action = view_menu->addAction("Toolbar");
    //todo
    view_menu->addSeparator();
    // . Canvas grid .
    const QPointer view_canvas_grid_action = view_menu->addAction("Canvas grid");
    view_canvas_grid_action->setCheckable(true);
    view_canvas_grid_action->setChecked(DO_SHOW_GRID_INIT);
    connect(view_canvas_grid_action, &QAction::toggled, [this](const bool is_checked) {
        m_viewer->m_do_show_grid = is_checked;
        m_scene->update();
    });
    // . Secondary canvas toolbar .
    const QPointer view_secondary_toolbar_action = view_menu->addAction("Secondary canvas toolbar");
    //todo

    // .: Debug :.
    const QPointer debug_menu = main_menu_bar->addMenu("Debug");
    // .: Render tests :.
    const QPointer debug_render_tests_menu = debug_menu->addMenu("Render tests");
    const QPointer debug_render_test_1_action = debug_render_tests_menu->addAction("Z-axis, out-of-order");
    connect(debug_render_test_1_action, &QAction::triggered, [this] {
        LoadSourceFromFile("./Resource/Example/Debug/Z-axis.toml");
    });
    // . Benchmark .
    const QPointer debug_benchmark_action = debug_menu->addAction("Benchmark");
    connect(debug_benchmark_action, &QAction::triggered, [this] {
        if (m_benchmark_dialog) {
            m_benchmark_dialog->raise();
            m_benchmark_dialog->activateWindow();
            return;
        }
        m_benchmark_dialog = new BenchmarkDialog(this);
        m_benchmark_dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_benchmark_dialog->show();
    });

    // .: Help :.
    const QPointer help_menu = main_menu_bar->addMenu("Help");
    // .: Examples :.
    const QPointer examples_menu = help_menu->addMenu("Examples");
    const QPointer example_1_action = examples_menu->addAction("Example 1: CPU block diagram");
    connect(example_1_action, &QAction::triggered, [this] { LoadSourceFromFile("./Resource/Example/Example1.toml"); });
    const QPointer example_2_action = examples_menu->addAction("Example 2: BPMN");
    connect(example_2_action, &QAction::triggered, [this] { LoadSourceFromFile("./Resource/Example/Example2.toml"); });
    // . About .
    const QPointer about_action = help_menu->addAction("About");
    connect(about_action, &QAction::triggered, [this] {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("About");
        msgBox.setTextFormat(Qt::RichText); // For <a href> to work
        msgBox.setText(
            "DiagramPche :: Qt<br><a href='https://github.com/RadekMocek/DiagramPche_Qt'>https://github.com/RadekMocek/DiagramPche_Qt</a>"
        );
        msgBox.exec();
    });

    // Recursively decrease font size on menu (sub)items
    for (const QAction* action : main_menu_bar->actions()) {
        if (QMenu* menu = action->menu()) {
            menu->setFont(main_menu_font);
            ApplyFontMenu(menu, main_menu_font);
        }
    }
}

// Helper function to recursively decrease font size on menu (sub)items
void GUIMainWindow::ApplyFontMenu(const QMenu* menu, const QFont& font)
{
    for (const QAction* action : menu->actions()) {
        if (QMenu* inner_menu = action->menu()) {
            inner_menu->setFont(font);
            ApplyFontMenu(inner_menu, font);
        }
    }
}

void GUIMainWindow::InitCentralWidget()
{
    const QPointer central_widget = new QWidget();
    setCentralWidget(central_widget);
    const QPointer main_layout = new QVBoxLayout();
    central_widget->setLayout(main_layout);

    // Splitter between Text editor & Canvas
    const QPointer splitter = new QSplitter();
    splitter->setChildrenCollapsible(false);

    // Text editor
    m_source = new QPlainTextEdit();
    m_source->setPlainText(WELCOME_TOML);
    m_source->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter = new Highlighter(m_source->document());

    splitter->addWidget(m_source);

    connect(m_source, &QPlainTextEdit::textChanged, this, &GUIMainWindow::ParseAndRedraw);

    // Canvas
    QFont scene_font;
    scene_font.setFamily(FONT_FAMILY_DEFAULT);
    scene_font.setPixelSize(SCENE_FONT_SIZE_BASE);
    m_scene = new GUIScene(scene_font, this);

    m_viewer = new GUISceneViewer(m_scene);

    // Canvas + secondary canvas toolbar container?
    const QPointer canvas_container_wrapper = new QWidget();
    const QPointer canvas_container = new QVBoxLayout(canvas_container_wrapper);
    canvas_container->setContentsMargins(0, 0, 0, 0);
    canvas_container->addWidget(m_viewer);

    const QPointer secondary_canvas_toolbar = new QHBoxLayout(); // secondary_canvas_toolbar == "sct"

    //TODO loop me and add logic, maybe move this init to a separate function
    const QPointer sct_dnd_btn_rectangle = new QPushButton("A");
    //sct_dnd_btn_rectangle->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_dnd_btn_rectangle);

    const QPointer sct_dnd_btn_ellipse = new QPushButton("B");
    //sct_dnd_btn_ellipse->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_dnd_btn_ellipse);

    const QPointer sct_dnd_btn_diamond = new QPushButton("C");
    //sct_dnd_btn_diamond->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_dnd_btn_diamond);

    const QPointer sct_dnd_btn_text = new QPushButton("D");
    //sct_dnd_btn_text->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_dnd_btn_text);

    secondary_canvas_toolbar->addStretch(1); // Buttons on left, slider on right

    const QPointer sct_slider = new QSlider(Qt::Horizontal);
    //sct_slider->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_slider);

    canvas_container->addLayout(secondary_canvas_toolbar);
    splitter->addWidget(canvas_container_wrapper);

    // Splitter ratio starts at 50/50
    //splitter->setStretchFactor(0, 1); // Index, factor
    //splitter->setStretchFactor(1, 1); // Index, factor
    const int half = width() / 2;
    splitter->setSizes({half, half});

    main_layout->addWidget(splitter, 1);

    // Error label
    QPalette error_text_palette;
    error_text_palette.setColor(QPalette::WindowText, COLOR_ERROR);

    m_error_label = new QLabel();
    m_error_label->setPalette(error_text_palette);
    main_layout->addWidget(m_error_label, 0);
}

void GUIMainWindow::InitToolbar()
{
    // Gives a right-padding to the toolbars.
    // These numbers don't really work as expected, otherwise I would give it a bigger padding/margin. This works for now.
    constexpr QMargins TOOLBAR_MARGINS(0, 0, 2, 0);

    // .: Toolbar :: Text edit font size :.
    const QPointer toolbar_font_size = addToolBar("Text edit font size");
    toolbar_font_size->setContentsMargins(TOOLBAR_MARGINS);
    const QPointer label_font_size = new QLabel("Font size: ");
    toolbar_font_size->addWidget(label_font_size);

    const QPointer widget_font_size = new QSpinBox();
    toolbar_font_size->addWidget(widget_font_size);

    // .: Toolbar :: Text edit cursor position :.
    const QPointer toolbar_cursor_pos = addToolBar("Text edit cursor position");
    toolbar_cursor_pos->setContentsMargins(TOOLBAR_MARGINS);
    const QPointer label_cursor_pos = new QLabel("Cursor pos: ");
    toolbar_cursor_pos->addWidget(label_cursor_pos);

    const QPointer widget_cursor_pos = new QLabel("0,0");
    toolbar_cursor_pos->addWidget(widget_cursor_pos);

    // .: Toolbar :: Selected node color :.
    const QPointer toolbar_node_color = addToolBar("Selected node color");
    toolbar_node_color->setContentsMargins(TOOLBAR_MARGINS);
    const QPointer label_node_color = new QLabel("Node color: ");
    toolbar_node_color->addWidget(label_node_color);

    const QPointer widget_node_color = new QPushButton("Change color");
    toolbar_node_color->addWidget(widget_node_color);

    // .: Toolbar :: Selected node type :.
    const QPointer toolbar_node_type = addToolBar("Selected node type");
    toolbar_node_type->setContentsMargins(TOOLBAR_MARGINS);
    const QPointer label_node_type = new QLabel("Node type: ");
    toolbar_node_type->addWidget(label_node_type);

    const QPointer widget_node_type = new QComboBox();
    toolbar_node_type->addWidget(widget_node_type);

    // .: Toolbar :: Selected node ID :.
    const QPointer toolbar_node_id = addToolBar("Selected node ID");
    toolbar_node_id->setContentsMargins(TOOLBAR_MARGINS);
    const QPointer label_node_id = new QLabel("Node ID: ");
    toolbar_node_id->addWidget(label_node_id);

    const QPointer widget_node_id = new QLabel("(no node hovered)");
    toolbar_node_id->addWidget(widget_node_id);
}

void GUIMainWindow::InitState()
{
    m_state_dialog_export.path = QDir::current().filePath("diagram.svg");
    m_state_dialog_export.action = ActionAfterExport_DoNothing;
}
