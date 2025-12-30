#include "viewer.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>

Viewer::Viewer(QWidget *parent)
    : QGraphicsView(parent),
    m_scene(new QGraphicsScene(this)),
    m_pixmapItem(nullptr),
    m_panning(false),
    m_currentScale(1.0)
{
    setScene(m_scene);

    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setBackgroundBrush(Qt::black);
}

void Viewer::display(const cv::Mat &image)
{
    if (image.empty())
        return;

    QPixmap pix = QPixmap::fromImage(matToImage(image));

    if (m_pixmapItem == nullptr) {
        m_pixmapItem = m_scene->addPixmap(pix);
    } else {
        m_pixmapItem->setPixmap(pix);
    }

    m_scene->setSceneRect(pix.rect());
    resetView();
}

QImage Viewer::matToImage(const cv::Mat &mat)
{
    cv::Mat rgb;

    if (mat.channels() == 1) {
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    } else {
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    }

    return QImage(
               rgb.data,
               rgb.cols,
               rgb.rows,
               static_cast<int>(rgb.step),
               QImage::Format_RGB888
               ).copy();
}

void Viewer::resetView()
{
    resetTransform();
    m_currentScale = 1.0;

    if (m_pixmapItem != nullptr) {
        fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    }
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    if (m_pixmapItem == nullptr)
        return;

    const double zoomIn = 1.15;
    const double zoomOut = 1.0 / zoomIn;

    double factor = (event->angleDelta().y() > 0) ? zoomIn : zoomOut;
    double newScale = m_currentScale * factor;

    if (newScale < 0.05 || newScale > 20.0)
        return;

    m_currentScale = newScale;
    scale(factor, factor);
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_currentScale > 1.0) {
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    QGraphicsView::mousePressEvent(event);
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() - delta.y());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsView::mouseReleaseEvent(event);
}
