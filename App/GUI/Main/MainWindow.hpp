#pragma once

// Qt imports
#include <QMainWindow>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QLabel;
class QPlainTextEdit;
QT_END_NAMESPACE

// App imports
#include "../../Logic/TOML/Parser.hpp"
#include "../Dialog/ExportSVGDialog.hpp"
#include "../SceneItem/SceneNode.hpp"

// App fwd declrs
class GUIScene;
class GUISceneViewer;

// =====================================
class GUIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GUIMainWindow();
    ~GUIMainWindow() override = default;

private:
    // = Methods =
    void ParseAndRedraw();
    void ExportToSvg() const;

    // AppFile
    void LoadSourceFromFile(const char* filename) const;

    // Init GUI
    void InitMainMenuBar();
    static void ApplyFontMenu(const QMenu* menu, const QFont& font);
    void InitCentralWidget();
    void InitState();

    // = Members=
    // Text editor with the TOML describing the diagram
    QPointer<QPlainTextEdit> m_source;

    // This is where diagram will be rendered
    QPointer<GUIScene> m_scene;

    // Parent for the scene
    QPointer<GUISceneViewer> m_viewer;

    // TOML parser
    Parser m_parser;

    QPointer<QLabel> m_error_label;

    // State
    ExportSVGDialogState m_state_dialog_export;
};
