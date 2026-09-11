//
// Created by fran on 01/08/2026.
//

#ifndef FLOWBROKER_TOPICGRAPH_HPP
#define FLOWBROKER_TOPICGRAPH_HPP

#include <QVBoxLayout>
#include <QLineSeries>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>

/**
 * @class TopicGraph
 * @brief Live chart of one topic, plotting one value against time.
 *
 * One widget per subscribed topic, created on demand by ClientWindow the first
 * time a value arrives for that topic. It keeps a bounded history (the oldest
 * points are dropped once the limit is reached) so that a long running session
 * neither grows without end nor slows the rendering down.
 *
 * Both axes are rescaled on every new point: the time axis follows a sliding
 * window ending at the newest timestamp, and the value axis is framed on the
 * points currently held.
 */
class TopicGraph : public QWidget {
    Q_OBJECT
public:
    explicit TopicGraph(const QString& topicName, QWidget* parent = nullptr);
    /// Append one point, drop the oldest ones if needed, then rescale both axes.
    void addPoint(qint64 tsMs, double value) const;
private:
    QLineSeries* m_lineSeries;
    QChart* m_chart;
    QChartView* m_chartView;
    QDateTimeAxis* m_axisX{};
    QValueAxis* m_axisY{};

    static constexpr int s_maxPoints = 300;
    static constexpr int s_windowSeconds = 60;
};


#endif //FLOWBROKER_TOPICGRAPH_HPP
