//
// Created by fran on 01/08/2026.
//

#include "TopicGraph.hpp"

TopicGraph::TopicGraph(const QString& topicName, QWidget* parent)
    : QWidget(parent)
    , m_lineSeries(nullptr)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_axisX(nullptr)
    , m_axisY(nullptr)
{
    setMinimumHeight(250);
    m_lineSeries = new QLineSeries();
    m_lineSeries->setPointsVisible(true);

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->addSeries(m_lineSeries);
    m_chart->setTitle(topicName);

    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("hh:mm:ss");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_lineSeries->attachAxis(m_axisX);

    m_axisY = new QValueAxis();
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_lineSeries->attachAxis(m_axisY);

    m_chartView  = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_chartView);
}

void TopicGraph::addPoint(const qint64 tsMs, const double value) const
{
    m_lineSeries->append(static_cast<double>(tsMs), value);
    while (m_lineSeries->count() > s_maxPoints)
        m_lineSeries->remove(0);

    // X axis: sliding time window, from the oldest kept point to the newest.
    const QDateTime newest = QDateTime::fromMSecsSinceEpoch(tsMs);
    const qint64 oldestMs = static_cast<qint64>(m_lineSeries->at(0).x());
    QDateTime oldest = QDateTime::fromMSecsSinceEpoch(oldestMs);
    if (oldest < newest.addSecs(-s_windowSeconds))
        oldest = newest.addSecs(-s_windowSeconds);
    m_axisX->setRange(oldest, newest);

    // Y axis: frame on the points actually displayed, NOT the server's rolling
    // min/max. The server computes them over a short window, but the graph keeps
    // a longer history, so kept points can fall outside that window and get
    // clipped. Scanning the real points guarantees every point stays visible.
    double yMin = m_lineSeries->at(0).y();
    double yMax = yMin;
    for (const QPointF& point : m_lineSeries->points()) {
        yMin = qMin(yMin, point.y());
        yMax = qMax(yMax, point.y());
    }
    double margin = (yMax - yMin) * 0.1;
    if (margin <= 0.0)
        margin = 1.0;
    m_axisY->setRange(yMin - margin, yMax + margin);
}
