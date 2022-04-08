#ifndef GRAPHICSOBJECTBUTTON_H
#define GRAPHICSOBJECTBUTTON_H

#include <QGraphicsObject>

class GraphicsObjectButton : public QGraphicsObject {
    Q_OBJECT // Enable signals and slots
public:
    // constructor
    GraphicsObjectButton(int wid, float hei, int roundingPercentage = 20, QGraphicsItem* parent = nullptr);
    // Event methods
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;

    // pure virtual from QGraphicsItem (parent of QGraphicsObject) to replace
    virtual QRectF boundingRect() const override;
    // QPainterPath shape() const override; // by default == bounding rect, might change it if it has an important impact on hover events
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setSize(int hei, int wid);

private:
    // size
    int width;
    int height;
    int percent; // percentage of the rectangle that is rounded

    // This variable stores whether it has been clicked on or not
    int clickState = 0; // default value is 0 - not hovering, not clicked

signals:
    void clicked();
};

#endif // GRAPHICSOBJECTBUTTON_H
