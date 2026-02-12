#include <QGraphicsView>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>

#include "MainWindow.hpp"
#include "../Config.hpp"
#include "../Helper/Print.hpp"
#include "Scene.hpp"
#include "SceneItem/SceneNode.hpp"
#include "Viewer.hpp"

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
    m_scene->Hobluj(m_parser.m_result_nodes_pq);
}

void GUIMainWindow::InitMainMenuBar()
{
    const QPointer main_menu_bar = new QMenuBar(this);
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

    // .: Debug :.
    const QPointer debug_menu = main_menu_bar->addMenu("Debug");
    // .: Render tests :.
    const QPointer render_tests_menu = debug_menu->addMenu("Render tests");
    const QPointer render_test_1_action = render_tests_menu->addAction("Z-axis, out-of-order");

    // .: Help :.
    const QPointer help_menu = main_menu_bar->addMenu("Help");
    // .: Examples :.
    const QPointer examples_menu = help_menu->addMenu("Examples");
    const QPointer example_1_action = examples_menu->addAction("Example 1");
    // . About .
    const QPointer about_action = help_menu->addAction("About");
    connect(about_action, &QAction::triggered, [this] {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("About");
        msgBox.setTextFormat(Qt::RichText); // For <a href> to work
        msgBox.setText(
            "DiagramPche :: Qt<br><a href='https://github.com/RadekMocek/DiagramPche_Qt'>https://github.com/RadekMocek/DiagramPche_Qt</a>");
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

    m_scene = new GUIScene(this);
    //m_scene->setSceneRect(0, 0, SCENE_SIZE, SCENE_SIZE);
    //const auto view = new QGraphicsView(m_scene);
    //view->setDragMode(QGraphicsView::NoDrag);
    const auto viewer = new GUISceneViewer(m_scene);
    main_layout->addWidget(viewer, 0, 1);

    // Same width for both columns
    main_layout->setColumnStretch(0, 1);
    main_layout->setColumnStretch(1, 1);
}
