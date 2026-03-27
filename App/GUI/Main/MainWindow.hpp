#pragma once

// Qt imports
#include <QMainWindow>
#include <QCoroTimer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPlainTextEdit;
class QSlider;
class QSplitter;
class QStackedWidget;
class QVBoxLayout;
QT_END_NAMESPACE

// Non-Qt deps
#include "QtAwesome.h"

// App imports
#include "../../Logic/TOML/Parser.hpp"
#include "../Dialog/BenchmarkDialog.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "../Dialog/PreferencesDialog.hpp"
#include "../SceneItem/SceneNode.hpp"
#include "../Support/Highlighter.hpp"

// App fwd declrs
class GUIScene;
class GUISceneViewer;
class ColorPicker;

// Foe benchmark filenames
#ifdef _WIN32
constexpr auto OS_ID = "win";
#else
constexpr auto OS_ID = "lin";
#endif

constexpr auto BENCHMARK_LIGHT_PATH = "./Resource/Example/Debug/BenchmarkLight.toml";
constexpr auto BENCHMARK_HEAVY_PATH = "./Resource/Example/Debug/BenchmarkHeavy.toml";

// === App config  === === === === === === === === === === ===
constexpr auto FONT_FAMILY_DEFAULT = "Inconsolata";
constexpr auto DO_SHOW_PRIMARY_TOOLBAR_INIT = true;
constexpr auto DO_SHOW_SECONDARY_TOOLBAR_INIT = true;
// === === === === === === === === === === === === === === ===

class GUIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GUIMainWindow();
    ~GUIMainWindow() override = default;

private:
    // = Helper structs =
    // To store info about selected node, that might be used if user changes something via the toolbar
    struct NodeInfo
    {
        std::string id{};
        toml::source_region node_source{};
        std::optional<toml::source_region> color_source = std::nullopt;
        std::optional<toml::source_region> type_source = std::nullopt;

        void Update(const Node& node)
        {
            id = node.id;
            node_source = node.node_source;
            color_source = node.color_source;
            type_source = node.type_source;
        }
    };

    // = Methods =
    void ParseAndRedraw();

    [[nodiscard]] std::optional<std::reference_wrapper<const Node>> GetNodeRefFromId(const std::string& id) const;

    // Slots:
    void OnSourceChange();
    void OnEmptySpaceClick();
    void OnNodeClick(const std::string& id);
    void OnNodeCtrlClick(const std::string& id) const;
    void OnNodeHoverEnter(const std::string& id) const;
    void OnNodeHoverLeave() const;
    void OnGhostNodePlace(NodeType type, QPoint position);
    void OnZoomChangeRequest(bool is_plus) const;
    // ---

    void ToolbarInfoSet(const Node& node) const;
    void ToolbarInfoReset() const;

    void ExportToSvg() const;
    void ErrorHighlight(const toml::source_region& EH_region) const;

    void ApplyPreferences() const;

    [[nodiscard]] int GetSourceFontSize() const;
    void SetSourceFontSize(int new_size) const;

    void SetAllToolbarsVisible(bool value) const;
    void SetNodeToolbarsEnabled(bool value) const;

    void UpdateCursorPositionInfo() const;

    void ResetCanvasScrollingAndZoom() const;

    // AppFile.cpp
    void HandleRegularNew();
    void HandleRegularOpen();
    bool HandleRegularSave();
    void HandleOpenExample(const QString& filename);
    //
    void LoadSourceFromFile(const QString& filename, bool is_example);
    bool SaveSourceToFile(const QString& filename) const;
    bool SaveSourceToFileFromDialog();

    // AppDialog.cpp
    QString SaveTOMLDialog();
    QString OpenTOMLDialog();
    int UnsavedWarningDialog();

    // AppSource.cpp
    void ReplaceInMSource(const toml::source_region& source, const QString& new_str) const;
    void InsertNodeParameterInMSource(const toml::source_position& end, const QString& new_str) const;

    // Init GUI
    void InitMainMenuBar();
    static void ApplyFontMenu(const QMenu* menu, const QFont& font);
    void InitCentralWidget();
    void InitSecondaryCanvasToolbar(const QPointer<QVBoxLayout>& canvas_container);
    void InitToolbar();
    void InitState();

    // Benchmark
    QCoro::Task<> CPUMeasureStart();
    // - Benchmark nodes
    QCoro::Task<> BenchmarkStart(BenchmarkType type);
    void BenchmarkStopForce() { m_bench_stop_flag = true; }
    void SetGUIEnabled(bool value) const;
    void OnSyntaxHighlightSwitchRequest() const;
    // - Benchmark widgets
    QCoro::Task<> WidgetbenchStart();

    // Icons
    [[nodiscard]] QIcon Icon(const int character, const bool is_solid = true) const
    {
        return m_awesome->icon((is_solid) ? fa::fa_solid : fa::fa_regular, character);
    }

    // Override
    void closeEvent(QCloseEvent* event) override;

    // = Members=
    // Splitter between Text editor & Canvas
    QPointer<QSplitter> m_splitter;

    // Text editor with the TOML describing the diagram
    QPointer<QStackedWidget> m_source_wrapper;
    QPointer<QPlainTextEdit> m_source;
    QPointer<Highlighter> m_highlighter; // Syntax highlight
    std::optional<QString> m_source_filename = std::nullopt;
    std::optional<QString> GetMSourceFilename();
    void SetMSourceFilename(const std::optional<QString>& filename);

    // This is where diagram will be rendered
    QPointer<GUIScene> m_scene;
    int m_scene_fps{};

    // Parent for the scene
    QPointer<GUISceneViewer> m_viewer;

    // TOML parser
    Parser m_parser;
    QPointer<QLabel> m_error_label; // Widget showing TOML parsing error, if there is some

    // MainMenuBar
    QPointer<QAction> m_view_jump_to_origin_action;

    // These are in some way synced with preferences
    QPointer<QSpinBox> m_source_font_size_spinbox;
    QPointer<QAction> m_view_primary_toolbar_checkable_action;
    QPointer<QAction> m_view_canvas_grid_checkable_action;
    QPointer<QAction> m_view_secondary_toolbar_checkable_action;

    // Toolbar
    QPointer<QToolBar> m_toolbar_source_font_size;
    QPointer<QToolBar> m_toolbar_source_cursor_position;
    QPointer<QToolBar> m_toolbar_node_color;
    QPointer<QToolBar> m_toolbar_node_type;
    QPointer<QToolBar> m_toolbar_node_id;
    // - toolbar dynamics == "tdb"
    bool m_is_some_node_selected = false;
    NodeInfo m_selected_node_info{};
    QPointer<QLabel> m_tbd_cursor_position_label;
    QPointer<ColorPicker> m_tbd_color_picker;
    QPointer<QComboBox> m_tbd_type_combo;
    QPointer<QLabel> m_tbd_id_label;

    // Secondary canvas toolbar
    QPointer<QWidget> m_secondary_canvas_toolbar_wrapper;
    QPointer<QSlider> m_secondary_canvas_toolbar_slider;

    // Modeless windows
    QPointer<BenchmarkDialog> m_benchmark_dialog;
    QPointer<PreferencesDialog> m_preferences_dialog;

    // State
    ExportSVGDialogState m_state_dialog_export{};
    PreferencesDialogState m_state_dialog_preferences{};
    BenchmarkStatsState m_state_benchmark_stats{};

    // Benchmark
    bool m_bench_stop_flag = false; // Stop button in GUI
    // - CPU usage
    float m_CPU_usage{};
    bool m_keep_measuring_CPU = false;

    // Icons
    QPointer<fa::QtAwesome> m_awesome;

signals:
    void BenchmarkStatsCrateUpdated();
    void BenchmarkDone();
};
