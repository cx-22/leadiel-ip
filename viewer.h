#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <opencv2/opencv.hpp>

class Viewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = nullptr);

    void display(const cv::Mat &image);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage matToImage(const cv::Mat &mat);
    void resetView();

    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_pixmapItem;

    bool m_panning;
    QPoint m_lastMousePos;
    double m_currentScale;
};

#endif // VIEWER_H
