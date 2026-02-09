#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QPlainTextEdit>

#include "Canvas.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:

private:
    void InitMainMenuBar();
    void InitCentralWidget();

    QPointer<QPlainTextEdit> m_source;
    QPointer<Canvas> m_canvas;

    // Temporary
    static constexpr auto SOURCE_INIT = R"""([variables]
w = 110
h = 72

[node."0,0"]

[node.Cache]
xy = [70, 70]
size = ["w", "h"]

[node.ALU]
pivot = "top"
xy = ["Cache", "bottom", 0, 35]
size = ["w", "h"]
z = 6
color = "#006db6AF"

[node."Řídící\njednotka"]
pivot = "top"
xy = ["ALU", "bottom", 0, 35]
size = ["w", "h"]

[node."Datové\nregistry"]
pivot = "left"
xy = ["ALU", "right", 35, 0]
size = ["w", "h"]

[node."Stavové\nregistry"]
pivot = "left"
xy = ["Řídící\njednotka", "right", 35, 0]
size = ["w", "h"]

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
color=[150,0,0,255]
tips="<>"

[[path]]
start=[400,400]
end=[400,500]
points=[
  ["","",500,"","",400],
  ["","",500,"","",500],
]
tips="<>"
)""";
};
