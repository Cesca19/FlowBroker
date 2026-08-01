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

class TopicGraph : public QWidget {
    Q_OBJECT
public:
    explicit TopicGraph(const QString& topicName, QWidget* parent = nullptr);
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
