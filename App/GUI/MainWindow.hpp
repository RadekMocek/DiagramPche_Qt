#pragma once

// Qt imports
#include <QMainWindow>
#include <QPointer>

// Qt fwd declrs
QT_BEGIN_NAMESPACE
class QLabel;
class QPlainTextEdit;
QT_END_NAMESPACE

// App imports
#include "../Logic/TOML/Parser.hpp"
#include "SceneItem/SceneNode.hpp"

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
    void ParseAndUpdate();

    void InitMainMenuBar();
    static void ApplyFontMenu(const QMenu* menu, const QFont& font);

    void InitCentralWidget();

    void LoadSourceFromFile(const char* filename);

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

    // Temporary
    static constexpr auto SOURCE_INIT = R"""([variables]
w = 110
h = 72

[node."0,0"]

[node.custom_size]
value = "it is\nwhat\nit is"
xy = ["Datové\nregistry", "top-right", -10, 10]
pivot = "bottom-left"
size = [200, 150]
label_pos = "bottom-right"
z = 3

[node.Cache]
xy = [70, 70]
#size = ["w", "h"]

[node.ALU]
pivot = "top"
xy = ["Cache", "bottom", 0, 35]
#size = ["w", "h"]
z = 6
color = "#006db680"

[node."Řídící\njednotka"]
pivot = "top"
xy = ["ALU", "bottom", 0, 35]
#size = ["w", "h"]

[node."Datové\nregistry"]
pivot = "left"
xy = ["ALU", "right", 35, 0]
#size = ["w", "h"]

[node."Stavové\nregistry"]
pivot = "left"
xy = ["Řídící\njednotka", "right", 35, 0]
#size = ["w", "h"]

[[path]]
start=["Cache", "left", 0, 0]
ends=[
  ["ALU", "left", 0, 0],
  ["Řídící\njednotka", "bottom",0,0]
]
shift = 30
points=[
  ["", "start", -25, "", "start", -15],
  ["Datové\nregistry", "top", 0, "", "", 5],
  ["", "prev", 0, "", "end", 0]
]
tips="<>"

[[path]]
start=["Cache","top-left",0,0]
end=["Cache","bottom-right",0,0]
color=[150,0,0,80]
tips="<>"

[[path]]
start=[400,400]
end=[400,500]
points=[
  ["","",500,"","",400],
  ["","",500,"","",500],
]
tips="<>"

[[path]]
start=["Datové\nregistry","right",0,20]
ends=[
  ["Datové\nregistry","right",200,0],
  ["Datové\nregistry","right",200,15],
  ["Datové\nregistry","right",200,30],
]
points=[["","start",50,"","end",0]]
tips="<-"
)""";
};
