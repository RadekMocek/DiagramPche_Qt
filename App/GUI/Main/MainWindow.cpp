#include <filesystem>

#include <QDesktopServices>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSvgGenerator>

#include "MainWindow.hpp"
#include "../../Config.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "Scene.hpp"
#include "Viewer.hpp"

GUIMainWindow::GUIMainWindow()
{
    setWindowTitle("Untitled – DiagramPche :: Qt");
    resize(1366, 768);

    InitMainMenuBar();
    InitCentralWidget();
    InitState();

    ParseAndRedraw();
}

// == Logic ==

void GUIMainWindow::ParseAndRedraw()
{
    m_parser.Parse(m_source->toPlainText().toStdString());
    m_scene->Redraw(m_parser.m_result_nodes_pq, m_parser.m_result_paths);

    if (m_parser.m_is_error) {
        auto err_str = QString::fromStdString(m_parser.m_error_description);
        err_str.replace("\n", " ");
        m_error_label->setText(err_str);
    }
    else {
        m_error_label->setText("");
    }
}

void GUIMainWindow::ExportToSvg() const
{
    constexpr auto SVG_PADDING = 25.0f;
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

// == GUI init ==

void GUIMainWindow::InitMainMenuBar()
{
    const QPointer main_menu_bar = new QMenuBar(this);

    QFont main_menu_font = main_menu_bar->font();
    main_menu_font.setPixelSize(FONT_SIZE_MAIN_MENU_BAR);
    main_menu_bar->setFont(main_menu_font);

    setMenuBar(main_menu_bar);

    // .: File :.
    const QPointer file_menu = main_menu_bar->addMenu("File");
    // . Export to SVG .
    const QPointer export_svg_action = file_menu->addAction("Export to SVG");
    connect(export_svg_action, &QAction::triggered, [this]() {
        ExportSVGDialog dialog(this, m_state_dialog_export);
        connect(&dialog, &ExportSVGDialog::ButtonExportClicked, this, &GUIMainWindow::ExportToSvg);
        dialog.exec();
    });
    // . Exit .
    const QPointer exit_action = file_menu->addAction("Exit");
    connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

    // .: View :.
    const QPointer view_menu = main_menu_bar->addMenu("View");
    // . Grid .
    const QPointer view_grid_action = view_menu->addAction("Grid");
    view_grid_action->setCheckable(true);
    view_grid_action->setChecked(DO_SHOW_GRID_INIT);
    connect(view_grid_action, &QAction::toggled, [this](const bool is_checked) {
        m_viewer->m_do_show_grid = is_checked;
        m_scene->update();
    });

    // .: Debug :.
    const QPointer debug_menu = main_menu_bar->addMenu("Debug");
    // .: Render tests :.
    const QPointer render_tests_menu = debug_menu->addMenu("Render tests");
    const QPointer render_test_1_action = render_tests_menu->addAction("Z-axis, out-of-order");
    connect(render_test_1_action, &QAction::triggered, [this] {
        LoadSourceFromFile("./Resource/Example/Debug/Z-axis.toml");
    });

    // .: Help :.
    const QPointer help_menu = main_menu_bar->addMenu("Help");
    // .: Examples :.
    const QPointer examples_menu = help_menu->addMenu("Examples");
    const QPointer example_1_action = examples_menu->addAction("Example 1");
    connect(example_1_action, &QAction::triggered, [this] { LoadSourceFromFile("./Resource/Example/Example1.toml"); });
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

    const QPointer main_layout = new QGridLayout();
    central_widget->setLayout(main_layout);

    m_source = new QPlainTextEdit();
    m_source->setPlainText(WELCOME_TOML);
    main_layout->addWidget(m_source, 0, 0);

    connect(m_source, &QPlainTextEdit::textChanged, this, &GUIMainWindow::ParseAndRedraw);

    QFont scene_font;
    scene_font.setFamily(FONT_FAMILY_DEFAULT);
    scene_font.setPixelSize(SCENE_FONT_SIZE_BASE);
    m_scene = new GUIScene(scene_font, this);

    m_viewer = new GUISceneViewer(m_scene);
    main_layout->addWidget(m_viewer, 0, 1);

    QPalette error_text_palette;
    error_text_palette.setColor(QPalette::WindowText, COLOR_ERROR);

    m_error_label = new QLabel();
    m_error_label->setPalette(error_text_palette);
    main_layout->addWidget(m_error_label, 1, 0, 1, 2);

    // Same width for both columns
    main_layout->setColumnStretch(0, 1);
    main_layout->setColumnStretch(1, 1);
}

void GUIMainWindow::InitState()
{
    m_state_dialog_export.path = QDir::current().filePath("diagram.svg");
    m_state_dialog_export.action = ActionAfterExport_DoNothing;
}
