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
#include <QStackedWidget>
#include <QStyleHints>
#include <QSvgGenerator>
#include <QToolBar>

#include "MainWindow.hpp"
#include "../../Helper/Color.hpp"
#include "../../Helper/CPU.hpp"
#include "../../Welcome.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "../Dialog/Widgetbench.hpp"
#include "../Support/ColorPicker.hpp"
#include "../Support/DragButton.hpp"
#include "Scene.hpp"
#include "Viewer.hpp"

constexpr auto ICON_SCALE_DEFAULT = 0.9;
constexpr auto CANVAS_FONT_SIZE_BASE = 18;
constexpr auto CANVAS_FONT_SIZE_STEP = 4;
constexpr auto CANVAS_FONT_SIZE_MIN = 6;
constexpr auto CANVAS_FONT_SIZE_MAX = 30;
constexpr auto SLIDER_INIT = 3;
constexpr auto SLIDER_MIN = 0;
constexpr auto SLIDER_MAX = (CANVAS_FONT_SIZE_MAX - CANVAS_FONT_SIZE_MIN) / CANVAS_FONT_SIZE_STEP;

GUIMainWindow::GUIMainWindow()
{
    setWindowTitle("[*] Untitled – DiagramPche :: Qt");
    resize(1280, 800);
    setMinimumSize(666, 416);

    // Icons
    m_awesome = new fa::QtAwesome(this);
    m_awesome->initFontAwesome();
    m_awesome->setDefaultOption("scale-factor", ICON_SCALE_DEFAULT);

    InitMainMenuBar();
    InitCentralWidget();
    InitToolbar();
    InitState();

    ParseAndRedraw();

    // Fill with initial value
    m_CPU_usage = CPUStats::GetCurrentValue();

    // cmd args
    const auto& args = QCoreApplication::arguments();
    auto args_do_benchmark_nodes = false;
    auto args_benchmark_type_num = 0;
    auto args_do_benchmark_widgets = false;
    auto is_benchmark_run_from_terminal = false;
    if (args.size() == 4 && args[1] == "b") {
        constexpr auto benchmark_type_err_msg =
            "Second parameter (benchmark type) must be a number:\n\n\t0 - light\n\t1 - heavy\n\t2 - gradual\n\t3 - complete 3in1\n\n";

        args_do_benchmark_nodes = true;
        is_benchmark_run_from_terminal = true;

        bool is_parse_ok;
        args_benchmark_type_num = args[2].toInt(&is_parse_ok);
        if (!is_parse_ok) {
            qDebug() << benchmark_type_err_msg;
            args_do_benchmark_nodes = false;
        }
        if (args_benchmark_type_num < 0 || args_benchmark_type_num > 3) {
            qDebug() << benchmark_type_err_msg;
            args_do_benchmark_nodes = false;
        }

        if (args[3] == "0") {
            m_highlighter_light->setDocument(nullptr);
        }
        else if (args[3] == "2") {
            m_source_wrapper->setCurrentIndex(1);
        }
    }
    else if (args.size() == 2 && args[1] == "w") {
        args_do_benchmark_widgets = true;
        is_benchmark_run_from_terminal = true;
    }

    m_is_benchmark_run_from_terminal = is_benchmark_run_from_terminal;

    if (args_do_benchmark_nodes) {
        BenchmarkStart(static_cast<BenchmarkType>(args_benchmark_type_num));
    }
    else if (args_do_benchmark_widgets) {
        WidgetbenchStart();
    }

    QTimer::singleShot(0, this, [this] {
        // After this CTOR, `m_source` sends signal about text being changed.
        // We have to use a timer to mark the document as non-dirty after that happens.
        // I am not sure what exactly happens under the hood but calling just `m_is_source_dirty = false;` without timer won't work.
        // It has something to do with the syntax highlighter. This issue stops when I turn off syntax highlighting.
        setWindowModified(false);

        // If this line is called outside this timer, canvas scrolling won't be set properly.
        // So there is also some stuff happening under the hood with the scene viewer.
        ResetCanvasScrollingAndZoom();
    });
}

// == Logic ==

void GUIMainWindow::OnSourceChange()
{
    //qDebug() << "OnSourceChange";
    setWindowModified(true);
    ParseAndRedraw();
}

void GUIMainWindow::ParseAndRedraw()
{
    m_parser.Parse(m_source->toPlainText().toStdString());
    m_scene->Redraw(m_parser.m_result_nodes_pq, m_parser.m_result_nodes, m_parser.m_result_paths);

    if (m_parser.m_is_error) {
        auto err_str = QString::fromStdString(m_parser.m_error_description);
        err_str.replace("\n", " ");
        m_error_label->setText(err_str);
        m_error_label->setToolTip(err_str);
        ErrorHighlight(m_parser.m_error_source_region);
    }
    else {
        m_error_label->setText("");
        m_error_label->setToolTip("");
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
        QTextCursor cursor(m_source->document()->findBlockByLineNumber(
            static_cast<int>(node.value().get().node_source.begin.line - 1)
        ));
        m_source->moveCursor(QTextCursor::End);
        // Move to end first so when we jump the [node.id] is at top of the text edit
        cursor.movePosition(QTextCursor::EndOfLine);
        m_source->setTextCursor(cursor);
        m_source->setFocus();
    }
}

void GUIMainWindow::OnNodeHoverEnter(const std::string& id) const
{
    if (m_is_some_node_selected || m_viewer->IsUserDraggingCanvas()) {
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

void GUIMainWindow::OnGhostNodePlace(const NodeType type, const QPoint position)
{
    // Prepare new node info
    const auto new_node_id = std::format("node_{}", m_parser.m_result_nodes.size());

    // Add new node definition to textedit
    m_source->appendPlainText(
        QString("\n[node.%1]\ntype = %2\nxy = [%3, %4]")
        .arg(new_node_id)
        .arg(GetQuotedStringFromNodeType(type))
        .arg(position.x())
        .arg(position.y())
    );

    // Select newly added node
    OnNodeClick(new_node_id);

    // Move cursor up
    OnNodeCtrlClick(new_node_id);
}

void GUIMainWindow::OnZoomChangeRequest(const bool is_plus) const
{
    const auto current_value = m_secondary_canvas_toolbar_slider->value();
    const auto slider_step = m_secondary_canvas_toolbar_slider->singleStep();
    const auto current_step = (is_plus) ? slider_step : -slider_step;
    m_secondary_canvas_toolbar_slider->setValue(current_value + current_step);
    // Callback that triggers after slider value change will handle the actual zoom level change
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
    // Qt has the ability to save content of QGraphicsScene into a SVG file
    QSvgGenerator svg_generator;

    // We need to calculate the SVG canvas size, so the diagram occupies 100% of the canvas (excluding `svg_padding`)
    const auto svg_padding = m_state_dialog_export.padding;

    const auto scene_aabr = m_scene->GetSceneAABR().toRect();
    const auto scene_aabr_size = scene_aabr.size();
    const QRect viewbox(scene_aabr.left() - svg_padding,
                        scene_aabr.top() - svg_padding,
                        scene_aabr_size.width() + 2 * svg_padding,
                        scene_aabr.height() + 2 * svg_padding);

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

void GUIMainWindow::ApplyPreferences()
{
    const auto is_window_modified = isWindowModified();

    // (Not ideal, I am updating everything everytime. More ideal would be to check only for the changes)
    // Keep canvas light?
    m_viewer->m_is_background_light = m_state_dialog_preferences.is_canvas_light;
    // Redraw so the color change is visible immediatelly and not only after first interaction
    m_viewer->viewport()->update();
    // Source font size
    m_source_font_size_spinbox->setValue(m_state_dialog_preferences.source_font_size);
    // Syntax highlight
    if (m_state_dialog_preferences.is_syntax_highlight_enabled) {
        if (m_is_color_theme_light) {
            m_highlighter_light->setDocument(m_source->document());
        }
        else {
            m_highlighter_dark->setDocument(m_source->document());
        }
    }
    else {
        m_highlighter_light->setDocument(nullptr);
        m_highlighter_dark->setDocument(nullptr);
    }
    // View
    m_view_primary_toolbar_checkable_action->setChecked(m_state_dialog_preferences.do_show_primary_toolbar);
    m_view_canvas_grid_checkable_action->setChecked(m_state_dialog_preferences.do_show_canvas_grid);
    m_view_secondary_toolbar_checkable_action->setChecked(m_state_dialog_preferences.do_show_secondary_toolbar);

    // Changing highlighter will mark document as dirty, so we undo it like this
    if (!is_window_modified) {
        QTimer::singleShot(0, this, [this] {
            setWindowModified(false);
        });
    }
}

void GUIMainWindow::ApplyColorTheme(const bool is_light)
{
    const auto is_window_modified = isWindowModified();

    m_is_color_theme_light = is_light;

    if (is_light) {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
        if (m_state_dialog_preferences.is_syntax_highlight_enabled) {
            m_highlighter_light->setDocument(m_source->document());
            m_highlighter_dark->setDocument(nullptr);
        }
    }
    else {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
        if (m_state_dialog_preferences.is_syntax_highlight_enabled) {
            m_highlighter_light->setDocument(nullptr);
            m_highlighter_dark->setDocument(m_source->document());
        }
    }

    // Changing highlighter will mark document as dirty, so we undo it like this
    if (!is_window_modified) {
        QTimer::singleShot(0, this, [this] {
            setWindowModified(false);
        });
    }
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

void GUIMainWindow::ResetCanvasScrollingAndZoom() const
{
    m_secondary_canvas_toolbar_slider->setValue(SLIDER_INIT);
    m_viewer->ResetCanvasScrolling();
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
    // .:======:.
    const QPointer file_menu = main_menu_bar->addMenu("File");

    // Helper function to DRY
    const auto HandlePossibleUnsaved = [this](const std::function<void()>& Func) {
        if (!isWindowModified()) {
            Func();
        }
        else {
            switch (UnsavedWarningDialog()) {
            case QMessageBox::Save:
                if (HandleRegularSave()) {
                    Func();
                }
                break;
            case QMessageBox::Discard:
                Func();
                break;
            case QMessageBox::Cancel: // NOLINT(*-branch-clone)
                break;
            default:
                break;
            }
        }
    };

    // . New .
    const QPointer file_new_action = file_menu->addAction(Icon(fa::fa_file_circle_plus), "New");
    connect(file_new_action, &QAction::triggered, [this, HandlePossibleUnsaved] {
        HandlePossibleUnsaved([this] { HandleRegularNew(); });
    });
    // . Open .
    const QPointer file_open_action = file_menu->addAction(Icon(fa::fa_folder_open), "Open");
    connect(file_open_action, &QAction::triggered, [this, HandlePossibleUnsaved] {
        HandlePossibleUnsaved([this] { HandleRegularOpen(); });
    });
    // . Save .
    const QPointer file_save_action = file_menu->addAction(Icon(fa::fa_floppy_disk, false), "Save");
    connect(file_save_action, &QAction::triggered, [this] {
        HandleRegularSave();
    });
    // . Save as .
    const QPointer file_saveas_action = file_menu->addAction(Icon(fa::fa_floppy_disk), "Save as");
    connect(file_saveas_action, &QAction::triggered, [this] {
        SaveSourceToFileFromDialog();
    });
    // . Export to SVG .
    const QPointer file_export_svg_action = file_menu->addAction(Icon(fa::fa_share_from_square), "Export to SVG");
    connect(file_export_svg_action, &QAction::triggered, [this] {
        ExportSVGDialog dialog(this, m_state_dialog_export);
        connect(&dialog, &ExportSVGDialog::ButtonExportClicked, this, &GUIMainWindow::ExportToSvg);
        dialog.exec();
    });
    file_menu->addSeparator();
    // . Preferences .
    const QPointer file_preferences_action = file_menu->addAction(Icon(fa::fa_wrench), "Preferences");
    connect(file_preferences_action, &QAction::triggered, [this] {
        if (m_preferences_dialog) {
            m_preferences_dialog->raise();
            m_preferences_dialog->activateWindow();
            return;
        }
        // These can be set from elsewhere so need to update them in dialog state before showing it
        m_state_dialog_preferences.source_font_size = GetSourceFontSize();
        m_state_dialog_preferences.do_show_primary_toolbar = m_view_primary_toolbar_checkable_action->isChecked();
        m_state_dialog_preferences.do_show_canvas_grid = m_view_canvas_grid_checkable_action->isChecked();
        m_state_dialog_preferences.do_show_secondary_toolbar = m_view_secondary_toolbar_checkable_action->isChecked();
        // Show dialog
        m_preferences_dialog = new PreferencesDialog(this, m_state_dialog_preferences);

        connect(m_preferences_dialog, &PreferencesDialog::ButtonApplyClicked,
                this, &GUIMainWindow::ApplyPreferences);

        connect(m_preferences_dialog, &PreferencesDialog::ButtonColorThemeClicked,
                this, &GUIMainWindow::ApplyColorTheme);

        m_preferences_dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_preferences_dialog->show();
    });
    file_menu->addSeparator();
    // . Exit .
    const QPointer file_exit_action = file_menu->addAction(Icon(fa::fa_person_through_window), "Exit");
    connect(file_exit_action, SIGNAL(triggered()), this, SLOT(close()));

    // .: View :.
    // .:======:.
    const QPointer view_menu = main_menu_bar->addMenu("View");
    // . Toolbar .
    m_view_primary_toolbar_checkable_action = view_menu->addAction("Toolbar");
    m_view_primary_toolbar_checkable_action->setCheckable(true);
    m_view_primary_toolbar_checkable_action->setChecked(DO_SHOW_PRIMARY_TOOLBAR_INIT);
    connect(m_view_primary_toolbar_checkable_action, &QAction::toggled, [this](const bool is_checked) {
        SetAllToolbarsVisible(is_checked);
    });
    view_menu->addSeparator();
    // . Canvas grid .
    m_view_canvas_grid_checkable_action = view_menu->addAction("Canvas grid");
    m_view_canvas_grid_checkable_action->setCheckable(true);
    m_view_canvas_grid_checkable_action->setChecked(DO_SHOW_GRID_INIT);
    connect(m_view_canvas_grid_checkable_action, &QAction::toggled, [this](const bool is_checked) {
        m_viewer->m_do_show_grid = is_checked;
        m_scene->update();
    });
    // . Secondary canvas toolbar .
    m_view_secondary_toolbar_checkable_action = view_menu->addAction("Secondary canvas toolbar");
    m_view_secondary_toolbar_checkable_action->setCheckable(true);
    m_view_secondary_toolbar_checkable_action->setChecked(DO_SHOW_SECONDARY_TOOLBAR_INIT);
    connect(m_view_secondary_toolbar_checkable_action, &QAction::toggled, [this](const bool is_checked) {
        m_secondary_canvas_toolbar_wrapper->setVisible(is_checked);
    });
    view_menu->addSeparator();
    // . Jump to canvas origin .
    m_view_jump_to_origin_action = view_menu->addAction("Jump to canvas origin");
    connect(m_view_jump_to_origin_action, &QAction::triggered, this, &GUIMainWindow::ResetCanvasScrollingAndZoom);

    // .: Debug :.
    // .:=======:.
    const QPointer debug_menu = main_menu_bar->addMenu("Debug");
    // .: Render tests :.
    const QPointer debug_render_tests_menu = debug_menu->addMenu("Render tests");
    const QPointer debug_render_test_1_action = debug_render_tests_menu->addAction("Z-axis, out-of-order");
    connect(debug_render_test_1_action, &QAction::triggered, [this] {
        HandleOpenExample("./Resource/Example/Debug/Z-axis.toml");
    });
    const QPointer debug_render_test_2_action = debug_render_tests_menu->addAction("Path label background");
    connect(debug_render_test_2_action, &QAction::triggered, [this] {
        HandleOpenExample("./Resource/Example/Debug/PathLabel.toml");
    });
    const QPointer debug_render_test_3_action = debug_render_tests_menu->addAction("Benchmark light");
    connect(debug_render_test_3_action, &QAction::triggered, [this] { HandleOpenExample(BENCHMARK_LIGHT_PATH); });
    const QPointer debug_render_test_4_action = debug_render_tests_menu->addAction("Benchmark heavy");
    connect(debug_render_test_4_action, &QAction::triggered, [this] { HandleOpenExample(BENCHMARK_HEAVY_PATH); });
    // . Benchmark .
    const QPointer debug_benchmark_action = debug_menu->addAction("Benchmark nodes");
    connect(debug_benchmark_action, &QAction::triggered, [this] {
        if (m_benchmark_dialog) {
            m_benchmark_dialog->raise();
            m_benchmark_dialog->activateWindow();
            return;
        }
        m_benchmark_dialog = new BenchmarkDialog(this, m_state_benchmark_stats);
        m_benchmark_dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_benchmark_dialog, &BenchmarkDialog::ButtonStartClicked, this, &GUIMainWindow::BenchmarkStart);
        connect(m_benchmark_dialog, &BenchmarkDialog::ButtonStopClicked, this, &GUIMainWindow::BenchmarkStopForce);

        connect(m_benchmark_dialog, &BenchmarkDialog::ButtonSwitchSyntaxHighlightClicked,
                this, &GUIMainWindow::OnSyntaxHighlightSwitchRequest);

        connect(m_benchmark_dialog, &BenchmarkDialog::ButtonSwitchTextEditVisibilityClicked,
                this, &GUIMainWindow::OnTextEditVisibilitySwitchRequest);

        connect(this, &GUIMainWindow::BenchmarkStatsCrateUpdated,
                m_benchmark_dialog, &BenchmarkDialog::OnBenchmarkStatsCrateUpdate);

        connect(this, &GUIMainWindow::BenchmarkDone, m_benchmark_dialog, &BenchmarkDialog::OnBenchmarkDone);

        m_benchmark_dialog->show();
    });
    const QPointer debug_widgetbench_action = debug_menu->addAction("Benchmark widgets");
    connect(debug_widgetbench_action, &QAction::triggered, [this] {
        WidgetbenchDialog dialog(this, true, 16);
        dialog.exec();
    });

    // .: Help :.
    // .:======:.
    const QPointer help_menu = main_menu_bar->addMenu("Help");
    // .: Examples :.
    const QPointer examples_menu = help_menu->addMenu("Examples");
    const QPointer example_1_action = examples_menu->addAction("Example 1: CPU block diagram");
    connect(example_1_action, &QAction::triggered, [this] { HandleOpenExample("./Resource/Example/Example1.toml"); });
    const QPointer example_2_action = examples_menu->addAction("Example 2: BPMN");
    connect(example_2_action, &QAction::triggered, [this] { HandleOpenExample("./Resource/Example/Example2.toml"); });
    const QPointer example_3_action = examples_menu->addAction("Example 3: Simple bubble chart");
    connect(example_3_action, &QAction::triggered, [this] { HandleOpenExample("./Resource/Example/Example3.toml"); });
    // .: Thesis images :.
    const QPointer thesis_images_menu = examples_menu->addMenu("Thesis images");

    constexpr std::array<std::pair<const char*, const char*>, 3> items = {
        {
            {"Wireframe", "Wireframe"},
            {"Error highlight", "ErrorHighlight"},
            {"Node position", "BeforeDrawingNodes"},
        }
    };

    for (const auto& [guiname, filename] : items) {
        const QPointer action = thesis_images_menu->addAction(guiname);
        connect(action, &QAction::triggered, [this, filename] {
            HandleOpenExample(QString("./Resource/Example/Thesis/%1.toml").arg(filename));
        });
    }

    // . About .
    const QPointer about_action = help_menu->addAction("About");
    connect(about_action, &QAction::triggered, [this] {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("About");
        msgBox.setTextFormat(Qt::RichText); // For `<a href="">...</a>` to work
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
    m_splitter = new QSplitter();
    m_splitter->setChildrenCollapsible(false);

    // Text editor
    // (Stacked widget is only used so we can hide the text editor in benchmark, otherwise we could use `QPlainTextEdit` directly)
    m_source_wrapper = new QStackedWidget();

    m_source = new QPlainTextEdit();
    m_source->setPlainText(WELCOME_TOML);
    m_source->setLineWrapMode(QPlainTextEdit::NoWrap);

    m_highlighter_light = new Highlighter(m_source->document());
    m_highlighter_dark = new HighlighterDark();

    m_source_wrapper->addWidget(m_source); // Index 0
    m_source_wrapper->addWidget(new QWidget()); // Index 1 (for hiding in benchmark and keeping empty space)
    m_source_wrapper->setCurrentIndex(0);

    m_splitter->addWidget(m_source_wrapper);

    connect(m_source, &QPlainTextEdit::textChanged, this, &GUIMainWindow::OnSourceChange);
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

    connect(m_scene, &GUIScene::FPSInfoTx, [this](const int fps) {
        m_scene_fps = fps;
    });

    m_viewer = new GUISceneViewer(m_scene);
    connect(m_viewer, &GUISceneViewer::ZoomChangeRequested, this, &GUIMainWindow::OnZoomChangeRequest);

    // Canvas + secondary canvas toolbar container
    const QPointer canvas_container_wrapper = new QWidget(); // Wrapper so it can be added to the splitter
    const QPointer canvas_container = new QVBoxLayout(canvas_container_wrapper);
    canvas_container->setContentsMargins(0, 0, 0, 0);
    canvas_container->addWidget(m_viewer);
    InitSecondaryCanvasToolbar(canvas_container);
    m_splitter->addWidget(canvas_container_wrapper);

    // Splitter ratio starts at 50/50
    const int half = width() / 2;
    m_splitter->setSizes({half, half});

    main_layout->addWidget(m_splitter, 1);

    // Error label
    QPalette error_text_palette;
    error_text_palette.setColor(QPalette::WindowText, COLOR_ERROR);

    m_error_label = new QLabel();
    m_error_label->setPalette(error_text_palette);
    m_error_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    main_layout->addWidget(m_error_label, 0);
}

void GUIMainWindow::InitSecondaryCanvasToolbar(const QPointer<QVBoxLayout>& canvas_container)
{
    m_secondary_canvas_toolbar_wrapper = new QWidget(); // Wrapper so it can be hided with View→SecondaryCanvasToolbar
    const QPointer secondary_canvas_toolbar = new QHBoxLayout(m_secondary_canvas_toolbar_wrapper);
    secondary_canvas_toolbar->setContentsMargins(0, 0, 0, 0);

    // secondary_canvas_toolbar == "sct"
    // drag'n'drop == "dnd"
    for (int i = 0; i < N_NTYPES; i++) {
        const QPointer sct_dnd_button = new DragButton(static_cast<NodeType>(i), Icon(NODE_TYPE_ICONS[i]));
        sct_dnd_button->setToolTip(
            QString("Drag and drop me onto the canvas to add a '%1' node.")
            .arg(NODE_TYPE_NAMES[i])
        );
        connect(sct_dnd_button, &DragButton::DragStateChanged, m_scene, &GUIScene::OnDragStateChange);
        secondary_canvas_toolbar->addWidget(sct_dnd_button);
    }

    // Buttons on left, slider on right
    secondary_canvas_toolbar->addStretch(1);

    // Slider label
    m_secondary_canvas_toolbar_slider_label = new QLabel("Zoom level: 1.00");
    secondary_canvas_toolbar->addWidget(m_secondary_canvas_toolbar_slider_label);

    // Zoom level slider
    m_secondary_canvas_toolbar_slider = new QSlider(Qt::Horizontal);
    m_secondary_canvas_toolbar_slider->setFixedWidth(180);
    secondary_canvas_toolbar->addWidget(m_secondary_canvas_toolbar_slider);

    m_secondary_canvas_toolbar_slider->setRange(SLIDER_MIN, SLIDER_MAX);
    m_secondary_canvas_toolbar_slider->setValue(SLIDER_INIT);
    m_secondary_canvas_toolbar_slider->setSingleStep(1);
    m_secondary_canvas_toolbar_slider->setPageStep(1);
    connect(m_secondary_canvas_toolbar_slider, &QSlider::valueChanged, [this](const int slider_value) {
        const auto new_scale =
            static_cast<float>(CANVAS_FONT_SIZE_MIN + CANVAS_FONT_SIZE_STEP * slider_value)
            / static_cast<float>(CANVAS_FONT_SIZE_BASE);

        m_viewer->resetTransform();
        m_viewer->scale(new_scale, new_scale);

        m_secondary_canvas_toolbar_slider_label->setText(QString("Zoom level: %1").arg(new_scale, 0, 'f', 2));
    });

    canvas_container->addWidget(m_secondary_canvas_toolbar_wrapper);
}

void GUIMainWindow::InitToolbar()
{
    const auto SetupToolbar = [](QToolBar* toolbar, const bool is_last = false) {
        // Pad
        const QPointer spacer = new QWidget;
        spacer->setFixedWidth(6);
        toolbar->addWidget(spacer);
        // Forbid drag'n'droping the toolbar
        toolbar->setMovable(false);
        // Forbid hiding the toolbar via context menu
        toolbar->toggleViewAction()->setEnabled(false);
        // Separator
        if (!is_last) {
            toolbar->addSeparator();
        }
    };

    // .: Toolbar :: Text edit font size :.
    m_toolbar_source_font_size = addToolBar("Text edit font size");
    m_toolbar_source_font_size->addWidget(new QLabel(" Font size: "));
    m_source_font_size_spinbox = new QSpinBox();
    m_source_font_size_spinbox->setMinimum(FONT_SIZE_SOURCE_MIN);
    m_source_font_size_spinbox->setMaximum(FONT_SIZE_SOURCE_MAX);
    m_source_font_size_spinbox->setSingleStep(FONT_SIZE_SOURCE_STEP);
    m_source_font_size_spinbox->setValue(GetSourceFontSize());
    m_toolbar_source_font_size->addWidget(m_source_font_size_spinbox);
    connect(m_source_font_size_spinbox, &QSpinBox::valueChanged, [this](const int value) {
        SetSourceFontSize(value);
    });
    SetupToolbar(m_toolbar_source_font_size);

    // .: Toolbar :: Text edit cursor position :.
    m_toolbar_source_cursor_position = addToolBar("Text edit cursor position");
    m_toolbar_source_cursor_position->addWidget(new QLabel("Cursor pos: "));
    m_tbd_cursor_position_label = new QLabel("0,0");
    m_toolbar_source_cursor_position->addWidget(m_tbd_cursor_position_label);
    SetupToolbar(m_toolbar_source_cursor_position);

    // .: Toolbar :: Selected node color :.
    m_toolbar_node_color = addToolBar("Selected node color");
    m_toolbar_node_color->addWidget(new QLabel("Node color: "));
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
    SetupToolbar(m_toolbar_node_color);

    // .: Toolbar :: Selected node type :.
    m_toolbar_node_type = addToolBar("Selected node type");
    m_toolbar_node_type->addWidget(new QLabel("Node type: "));
    m_tbd_type_combo = new QComboBox();
    for (int i = 0; i < N_NTYPES; i++) {
        m_tbd_type_combo->addItem(Icon(NODE_TYPE_ICONS[i]), NODE_TYPE_NAMES[i]);
    }
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
    SetupToolbar(m_toolbar_node_type);

    // .: Toolbar :: Selected node ID :.
    m_toolbar_node_id = addToolBar("Selected node ID");
    m_toolbar_node_id->addWidget(new QLabel("Node ID: "));
    m_tbd_id_label = new QLabel("(No node hovered)");
    m_toolbar_node_id->addWidget(m_tbd_id_label);
    SetupToolbar(m_toolbar_node_id, true);

    // No node is selected at the start
    SetNodeToolbarsEnabled(false);
}

void GUIMainWindow::InitState()
{
    m_state_dialog_export.path = QDir::current().filePath("diagram.svg");
    m_state_dialog_export.padding = 25;
    m_state_dialog_export.action = ActionAfterExport_DoNothing;

    constexpr auto IS_CANVAS_BG_LIGHT = true;
    m_viewer->m_is_background_light = IS_CANVAS_BG_LIGHT;
    m_state_dialog_preferences.is_canvas_light = IS_CANVAS_BG_LIGHT;
    m_state_dialog_preferences.source_font_size = GetSourceFontSize();
    m_state_dialog_preferences.is_syntax_highlight_enabled = true;
    m_state_dialog_preferences.do_show_primary_toolbar = DO_SHOW_PRIMARY_TOOLBAR_INIT;
    m_state_dialog_preferences.do_show_canvas_grid = DO_SHOW_GRID_INIT;
    m_state_dialog_preferences.do_show_secondary_toolbar = DO_SHOW_SECONDARY_TOOLBAR_INIT;
}

std::optional<QString> GUIMainWindow::GetMSourceFilename()
{
    return m_source_filename;
}

void GUIMainWindow::SetMSourceFilename(const std::optional<QString>& filename)
{
    m_source_filename = filename;
    if (m_source_filename.has_value()) {
        setWindowTitle(QString("[*] %1 – DiagramPche :: Qt").arg(m_source_filename.value()));
    }
    else {
        setWindowTitle("[*] Untitled – DiagramPche :: Qt");
    }
}

void GUIMainWindow::closeEvent(QCloseEvent* event)
{
    if (isWindowModified()) {
        switch (UnsavedWarningDialog()) {
        case QMessageBox::Save:
            if (HandleRegularSave()) {
                event->accept();
            }
            else {
                event->ignore();
            }
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            break;
        }
    }
    else {
        event->accept();
    }
}
