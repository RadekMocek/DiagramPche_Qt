#include "Canvas.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>

Canvas::Canvas(QWidget* parent) : QWidget(parent)
{
    QGraphicsScene scene;
    scene.addText("Hello, world!");

    QGraphicsView view(&scene);
    view.show();
}
