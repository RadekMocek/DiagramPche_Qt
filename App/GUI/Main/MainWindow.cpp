#include <filesystem>

#include <QDesktopServices>
#include <QComboBox>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QSvgGenerator>
#include <QToolBar>

#include "MainWindow.hpp"
#include "../../Helper/Color.hpp"
#include "../../Welcome.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "../Support/ColorPicker.hpp"
#include "../Support/DragButton.hpp"
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

    if (m_is_some_node_selected) {
        // If selected node is removed from the TOML source, unselect it
        if (!m_parser.m_result_nodes.contains(m_selected_node_info.id)) {
            // Clicking on empty space in canvas is basically unselecting the node
            OnEmptySpaceClick();
        }
        // If selected node was not removed, it's data might have changed in some way
        else {
            const auto& node = m_parser.m_result_nodes[m_selected_node_info.id];
            ToolbarInfoSet(node);
            m_selected_node_info.Update(node);
        }
    }
}

std::optional<std::reference_wrapper<const Node>> GUIMainWindow::GetNodeRefFromId(const std::string& id) const
{
    const auto& nodes = m_parser.m_result_nodes;
    if (const auto it = nodes.find(id); it != nodes.end()) {
        return std::cref(it->second);
    }
    return std::nullopt;
}

void GUIMainWindow::OnEmptySpaceClick()
{
    m_is_some_node_selected = false;
    SetNodeToolbarsEnabled(false);
    ToolbarInfoReset();
}

void GUIMainWindow::OnNodeClick(const std::string& id)
{
    if (const auto& node = GetNodeRefFromId(id); node.has_value()) {
        m_is_some_node_selected = true;
        m_selected_node_info.Update(node.value());
        SetNodeToolbarsEnabled(true);
        ToolbarInfoSet(node.value());
    }
}

void GUIMainWindow::OnNodeCtrlClick(const std::string& id) const
{
    if (const auto& node = GetNodeRefFromId(id); node.has_value()) {
        const QTextCursor cursor(m_source->document()->findBlockByLineNumber(
            static_cast<int>(node.value().get().node_source.begin.line - 1)
        ));
        m_source->moveCursor(QTextCursor::End);
        // Move to end first so when we jump the [node.id] is at top of the text edit
        m_source->setTextCursor(cursor);
        m_source->setFocus();
    }
}

void GUIMainWindow::OnNodeHoverEnter(const std::string& id) const
{
    if (m_is_some_node_selected) {
        return;
    }
    if (const auto& node = GetNodeRefFromId(id); node.has_value()) {
        ToolbarInfoSet(node.value());
    }
}

void GUIMainWindow::OnNodeHoverLeave() const
{
    if (m_is_some_node_selected) {
        return;
    }
    ToolbarInfoReset();
}

void GUIMainWindow::OnGhostNodePlace(const NodeType type) const
{
    qDebug() << "#1";
    m_source->insertPlainText("hello?");
    /*
    m_source->insertPlainText(QString("\n[node_%1]\ntype = %2\n")
                              .arg(m_parser.m_result_nodes.size())
                              .arg(GetQuotedStringFromNodeType(type))
    );
    */
}

void GUIMainWindow::ToolbarInfoSet(const Node& node) const
{
    m_tbd_color_picker->SetColor(GetQColorFromTuple(node.color), false);
    m_tbd_id_label->setText(QString::fromStdString(node.id));
    m_tbd_type_combo->setCurrentIndex(node.type); // Implicit enum to int
}

void GUIMainWindow::ToolbarInfoReset() const
{
    m_tbd_color_picker->SetColor(QColor::fromRgb(240, 240, 240), false);
    m_tbd_id_label->setText("(No node hovered)");
    m_tbd_type_combo->setCurrentIndex(0);
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

int GUIMainWindow::GetSourceFontSize() const
{
    return m_source->font().pixelSize();
}

void GUIMainWindow::SetSourceFontSize(const int new_size) const
{
    auto font = m_source->font();
    font.setPixelSize(new_size);
    m_source->setFont(font);
}

void GUIMainWindow::SetAllToolbarsVisible(const bool value) const
{
    m_toolbar_source_font_size->setVisible(value);
    m_toolbar_source_cursor_position->setVisible(value);
    m_toolbar_node_color->setVisible(value);
    m_toolbar_node_type->setVisible(value);
    m_toolbar_node_id->setVisible(value);
}

void GUIMainWindow::SetNodeToolbarsEnabled(const bool value) const
{
    m_toolbar_node_color->setEnabled(value);
    m_toolbar_node_type->setEnabled(value);
    m_toolbar_node_id->setEnabled(value);
}

void GUIMainWindow::UpdateCursorPositionInfo() const
{
    const auto& cursor = m_source->textCursor();
    m_tbd_cursor_position_label->setText(QString("%1,%2")
                                         .arg(cursor.blockNumber())
                                         .arg(cursor.columnNumber()));
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
    view_canvas_toolbar_action->setCheckable(true);
    view_canvas_toolbar_action->setChecked(DO_SHOW_PRIMARY_TOOLBAR_INIT);
    connect(view_canvas_toolbar_action, &QAction::toggled, [this](const bool is_checked) {
        SetAllToolbarsVisible(is_checked);
    });
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
    view_secondary_toolbar_action->setCheckable(true);
    view_secondary_toolbar_action->setChecked(DO_SHOW_SECONDARY_TOOLBAR_INIT);
    connect(view_secondary_toolbar_action, &QAction::toggled, [this](const bool is_checked) {
        m_secondary_canvas_toolbar_wrapper->setVisible(is_checked);
    });

    // .: Debug :.
    const QPointer debug_menu = main_menu_bar->addMenu("Debug");
    // .: Render tests :.
    const QPointer debug_render_tests_menu = debug_menu->addMenu("Render tests");
    const QPointer debug_render_test_1_action = debug_render_tests_menu->addAction("Z-axis, out-of-order");
    connect(debug_render_test_1_action, &QAction::triggered, [this] {
        LoadSourceFromFile("./Resource/Example/Debug/Z-axis.toml");
    });
    const QPointer debug_render_test_2_action = debug_render_tests_menu->addAction("Path label background");
    connect(debug_render_test_2_action, &QAction::triggered, [this] {
        LoadSourceFromFile("./Resource/Example/Debug/PathLabel.toml");
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
    connect(m_source, &QPlainTextEdit::cursorPositionChanged, this, &GUIMainWindow::UpdateCursorPositionInfo);

    // Canvas
    QFont scene_font;
    scene_font.setFamily(FONT_FAMILY_DEFAULT);
    scene_font.setPixelSize(SCENE_FONT_SIZE_BASE);
    m_scene = new GUIScene(scene_font, this);

    connect(m_scene, &GUIScene::EmptySpaceClicked, this, &GUIMainWindow::OnEmptySpaceClick);
    connect(m_scene, &GUIScene::NodeClicked, this, &GUIMainWindow::OnNodeClick);
    connect(m_scene, &GUIScene::NodeCtrlClicked, this, &GUIMainWindow::OnNodeCtrlClick);
    connect(m_scene, &GUIScene::NodeHoverEntered, this, &GUIMainWindow::OnNodeHoverEnter);
    connect(m_scene, &GUIScene::NodeHoverLeft, this, &GUIMainWindow::OnNodeHoverLeave);
    connect(m_scene, &GUIScene::GhostNodePlaced, this, &GUIMainWindow::OnGhostNodePlace);

    m_viewer = new GUISceneViewer(m_scene);

    // Canvas + secondary canvas toolbar container?
    const QPointer canvas_container_wrapper = new QWidget(); // Wrapper so it can be added to the splitter
    const QPointer canvas_container = new QVBoxLayout(canvas_container_wrapper);
    canvas_container->setContentsMargins(0, 0, 0, 0);
    canvas_container->addWidget(m_viewer);

    m_secondary_canvas_toolbar_wrapper = new QWidget(); // Wrapper so it can be hided with View→SecondaryCanvasToolbar
    const QPointer secondary_canvas_toolbar = new QHBoxLayout(m_secondary_canvas_toolbar_wrapper);
    secondary_canvas_toolbar->setContentsMargins(0, 0, 0, 0);

    // secondary_canvas_toolbar == "sct"
    // drag'n'drop = "dnd"
    for (int i = 0; i < N_NTYPES; i++) {
        const QPointer sct_dnd_button = new DragButton(static_cast<NodeType>(i));
        sct_dnd_button->setToolTip(
            QString("Drag and drop me onto the canvas to add a '%1' node.")
            .arg(NODE_TYPE_NAMES[i])
        );
        connect(sct_dnd_button, &DragButton::DragStateChanged, m_scene, &GUIScene::OnDragStateChange);
        secondary_canvas_toolbar->addWidget(sct_dnd_button);
    }

    // Buttons on left, slider on right
    secondary_canvas_toolbar->addStretch(1);

    const QPointer sct_slider = new QSlider(Qt::Horizontal);
    //sct_slider->setMaximumWidth(50);
    secondary_canvas_toolbar->addWidget(sct_slider);

    canvas_container->addWidget(m_secondary_canvas_toolbar_wrapper);
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
    constexpr auto FONT_SIZE_SOURCE_MIN = 8;
    constexpr auto FONT_SIZE_SOURCE_MAX = 40;

    // These numbers don't really work as expected, otherwise I would give it a proper padding/margin.
    //constexpr QMargins TOOLBAR_MARGINS(0, 0, 2, 0);

    // Currently it seems that the best way to add some right padding to the toolbar is by adding a literal space
    const auto AddSpace = [](QToolBar* toolbar) {
        toolbar->addWidget(new QLabel(" "));
    };

    // .: Toolbar :: Text edit font size :.
    m_toolbar_source_font_size = addToolBar("Text edit font size");
    const QPointer label_font_size = new QLabel(" Font size: ");
    m_toolbar_source_font_size->addWidget(label_font_size);

    const QPointer widget_font_size = new QSpinBox();
    widget_font_size->setValue(GetSourceFontSize());
    widget_font_size->setMinimum(FONT_SIZE_SOURCE_MIN);
    widget_font_size->setMaximum(FONT_SIZE_SOURCE_MAX);
    m_toolbar_source_font_size->addWidget(widget_font_size);
    connect(widget_font_size, &QSpinBox::valueChanged, [this](const int value) {
        SetSourceFontSize(value);
    });

    AddSpace(m_toolbar_source_font_size);
    m_toolbar_source_font_size->setMovable(false);
    m_toolbar_source_font_size->toggleViewAction()->setEnabled(false);

    // .: Toolbar :: Text edit cursor position :.
    m_toolbar_source_cursor_position = addToolBar("Text edit cursor position");
    const QPointer label_cursor_pos = new QLabel(" Cursor pos: ");
    m_toolbar_source_cursor_position->addWidget(label_cursor_pos);

    m_tbd_cursor_position_label = new QLabel("0,0");
    m_toolbar_source_cursor_position->addWidget(m_tbd_cursor_position_label);

    AddSpace(m_toolbar_source_cursor_position);
    m_toolbar_source_cursor_position->setMovable(false);
    m_toolbar_source_cursor_position->toggleViewAction()->setEnabled(false);

    // .: Toolbar :: Selected node color :.
    m_toolbar_node_color = addToolBar("Selected node color");
    const QPointer label_node_color = new QLabel(" Node color: ");
    m_toolbar_node_color->addWidget(label_node_color);

    m_tbd_color_picker = new ColorPicker(this);
    connect(m_tbd_color_picker, &ColorPicker::ColorChanged, [this](const QColor& new_color) {
        if (m_is_some_node_selected) {
            const auto color_quoted_str = GetQuotedRGBAHexFromQColor(new_color);
            if (m_selected_node_info.color_source.has_value()) {
                ReplaceInMSource(m_selected_node_info.color_source.value(), color_quoted_str);
            }
            else {
                InsertNodeParameterInMSource(m_selected_node_info.node_source.end, "\ncolor = " + color_quoted_str);
            }
        }
    });
    m_toolbar_node_color->addWidget(m_tbd_color_picker);

    AddSpace(m_toolbar_node_color);
    m_toolbar_node_color->setMovable(false);
    m_toolbar_node_color->toggleViewAction()->setEnabled(false);

    // .: Toolbar :: Selected node type :.
    m_toolbar_node_type = addToolBar("Selected node type");
    const QPointer label_node_type = new QLabel(" Node type: ");
    m_toolbar_node_type->addWidget(label_node_type);

    m_tbd_type_combo = new QComboBox();
    m_tbd_type_combo->addItems(NODE_TYPE_NAMES);
    // using `activated` instead of `currentIndexChanged`, because it does not trigger when the index is changed from code
    connect(m_tbd_type_combo, &QComboBox::activated, [this](const int new_idx) {
        if (m_is_some_node_selected) {
            const auto type_quoted_str = GetQuotedStringFromNodeType(static_cast<NodeType>(new_idx));
            if (m_selected_node_info.type_source.has_value()) {
                ReplaceInMSource(m_selected_node_info.type_source.value(), type_quoted_str);
            }
            else {
                InsertNodeParameterInMSource(m_selected_node_info.node_source.end,
                                             QString("\ntype = %1").arg(type_quoted_str));
            }
        }
    });
    m_toolbar_node_type->addWidget(m_tbd_type_combo);

    AddSpace(m_toolbar_node_type);
    m_toolbar_node_type->setMovable(false);
    m_toolbar_node_type->toggleViewAction()->setEnabled(false);

    // .: Toolbar :: Selected node ID :.
    m_toolbar_node_id = addToolBar("Selected node ID");
    const QPointer label_node_id = new QLabel(" Node ID: ");
    m_toolbar_node_id->addWidget(label_node_id);

    m_tbd_id_label = new QLabel("(No node hovered)");
    m_toolbar_node_id->addWidget(m_tbd_id_label);

    AddSpace(m_toolbar_node_id);
    m_toolbar_node_id->setMovable(false);
    m_toolbar_node_id->toggleViewAction()->setEnabled(false);

    // No node is selected at the start
    SetNodeToolbarsEnabled(false);
}

void GUIMainWindow::InitState()
{
    m_state_dialog_export.path = QDir::current().filePath("diagram.svg");
    m_state_dialog_export.action = ActionAfterExport_DoNothing;
}
