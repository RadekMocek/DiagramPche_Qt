#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>

#include "MainWindow.hpp"
#include "../Helper/Print.hpp"
#include "Scene.hpp"
#include "Viewer.hpp"
#include "../Config.hpp"

GUIMainWindow::GUIMainWindow()
{
    setWindowTitle("Untitled – DiagramPche :: Qt");
    resize(1366, 768);

    InitMainMenuBar();
    InitCentralWidget();

    ParseAndUpdate();
}

void GUIMainWindow::ParseAndUpdate()
{
    m_parser.Parse(m_source->toPlainText().toStdString());
    //Print(m_parser.m_result_nodes_pq.size());
    m_scene->Redraw(m_parser.m_result_nodes_pq);
    m_error_label->setText((m_parser.m_is_error) ? QString::fromStdString(m_parser.m_error_description) : "");
}

void GUIMainWindow::InitMainMenuBar()
{
    const QPointer main_menu_bar = new QMenuBar(this);

    QFont menu_font = main_menu_bar->font();
    menu_font.setPixelSize(FONT_SIZE_MAIN_MENU_BAR);
    main_menu_bar->setFont(menu_font);

    setMenuBar(main_menu_bar);

    // .: File :.
    const QPointer file_menu = main_menu_bar->addMenu("File");
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
}

void GUIMainWindow::InitCentralWidget()
{
    const QPointer central_widget = new QWidget();
    setCentralWidget(central_widget);

    const QPointer main_layout = new QGridLayout();
    central_widget->setLayout(main_layout);

    m_source = new QPlainTextEdit();
    m_source->setPlainText(SOURCE_INIT);
    main_layout->addWidget(m_source, 0, 0);

    connect(m_source, &QPlainTextEdit::textChanged, this, &GUIMainWindow::ParseAndUpdate);

    QFont scene_font;
    scene_font.setFamily("Inconsolata Medium");
    scene_font.setPixelSize(SCENE_FONT_SIZE_BASE);
    m_scene = new GUIScene(scene_font, this);
    //m_scene->setSceneRect(0, 0, SCENE_SIZE, SCENE_SIZE);

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
