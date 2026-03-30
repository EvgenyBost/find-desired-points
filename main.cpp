#include <QCoreApplication>
#include <iostream>
#include "line.h"
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <algorithm>
#include <QMap>


int main(int argc, char *argv[])
{
    int M, N; // M - number of lines, N - number of points that we need to find
    QVector<Line> lines;
    QVector<Line::Point> intersectionPoints;

    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.untitled

    QString fileName = "data.txt";
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Cant't open data file: " << fileName.toStdString();
        return -1;
    }

    QTextStream in(&file);

    in >> M >> N;
    if (M <= 0 || N <= 0) return 0;

    lines.reserve(M);
    int maxIntersectionsCount = (M * (M - 1)) / 2;
    intersectionPoints.reserve(maxIntersectionsCount);


    if (in.status() == QTextStream::Ok) {
        std::cout << "Number of lines: M = " << M << ", Number of points to find: N = " << N << std::endl;
    }


    //Step 1: Read M, N, lines, create lines with formalization
    int count = M;
    while (!in.atEnd() && count-- > 0) {
        double x1, y1, x2, y2;
        in >> x1 >> y1 >> x2 >> y2;

        Line::Point p1(x1, y1);
        Line::Point p2(x2, y2);

        // Check if we read coordinats correctly
        if (in.status() == QTextStream::Ok) {
            lines.append(Line(p1, p2));
        }
    }

    file.close();

    std::cout << "Lines count: " << lines.size() << std::endl;
    for (const auto &line : lines) {
        line.printInfo();
    }

    //Step 2: Find intersection points
    for (int i = 0; i < lines.size(); ++i) {
        for (int j = i + 1; j < lines.size(); ++j) {
            if(lines[i].mayIntersect(lines[j])){
                Line::Point inter;
                if (lines[i].intersect(lines[j], inter)) {
                    //Add only points inside working field {[-1000;-1000],[1000;1000]}
                    if ((inter.x >= -1000) && (inter.x <= 1000) && (inter.y >= -1000) && (inter.y <= 1000)){
                        intersectionPoints.append(inter);
                    }
                }
            }
        }
        if(i % 1000 == 0){
            std::cout << "Found " << intersectionPoints.size() << " intersections. Approximately " << (double)intersectionPoints.size()*100/maxIntersectionsCount << " % done" << std::endl;
        }
    }

    std::cout << "Total intersections found: " << intersectionPoints.size() << std::endl;

    //Step 3: Filter intersection points: remove all points outside working field {[-1000;-1000],[1000;1000]}
    // ALREADY MADE it in previous step to increase performance
    /*for (int i = intersectionPoints.size() - 1; i >= 0; --i){
        Line::Point point = intersectionPoints[i];
        if ((point.x < -1000) || (point.x > 1000) || (point.y < -1000) || (point.y > 1000)){
            intersectionPoints.removeAt(i);
        }
    }*/

    //Step 4: Filter intersection points: Combine points into one cluster if distance between them <=2
    //        to take into account the error in the passage of lines relative to the desired point <=1


    QMap<QPair<int, int>, int> grid; // Хранит индекс кластера для координат сетки
    QVector<QVector<Line::Point>> pointsCluster;
    const double threshold = 2.0;

    int i = 0;
    for (const auto& newPoint : std::as_const(intersectionPoints)) {
        bool found = false;
        i++;

        // Find cell coordinates. Cell size = threshold*threshold
        int gridX = qFloor(newPoint.x / threshold);
        int gridY = qFloor(newPoint.y / threshold);

        // Check center cell and 8 cells round it
        for (int dx = -1; dx <= 1 && !found; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                QPair<int, int> cell(gridX + dx, gridY + dy);

                if (grid.contains(cell)) {


                    int clusterIdx = grid.value(cell);
                    // Check distance
                    if (newPoint.distanceTo(pointsCluster[clusterIdx][0]) <= threshold) {
                        pointsCluster[clusterIdx].append(newPoint);
                        found = true;
                        break;
                    }
                }
            }
        }

        if (!found) {
            // Create new cluster
            pointsCluster.append({newPoint});
            int newIdx = pointsCluster.size() - 1;
            grid.insert(QPair<int, int>(gridX, gridY), newIdx);
        }

        if (intersectionPoints.size() > 100 && i % (intersectionPoints.size()/100) == 0) {
                std::cout << "Processed " << i << " points, current clusters: " << pointsCluster.size() << ", "<< (double)i*100/intersectionPoints.size() << "% done" << std::endl;
            }
    }

    //======================================SLOW=============
    // Sort intersectionPoints to increase performance on the next step
    /*std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](const Line::Point& a, const Line::Point& b) {
        return a.x < b.x;
    });*/
    // QList<int> activeClusterIndices; //Optimization. check only nearest clusters, sorted by X
    // QVector< QVector<Line::Point>> pointsCluster;
    // pointsCluster.reserve(N * 2);
    // const double threshold = 2.0;

    // for (int i = 0; i < intersectionPoints.size(); ++i) {
    //     const auto& newPoint = intersectionPoints[i];
    //     bool found = false;

    //     // 1. Clear not nearest clusters
    //     auto it = activeClusterIndices.begin();
    //     while (it != activeClusterIndices.end()) {
    //
    //         if (pointsCluster[*it][0].x < newPoint.x - threshold) {
    //             it = activeClusterIndices.erase(it); // Do not check this cluster anymore
    //         } else {
    //             ++it;
    //         }
    //     }

    //     // 2. Try to find necessary cluster in active clusters
    //     for (int clusterIdx : activeClusterIndices) {
    //         if (newPoint.distanceTo(pointsCluster[clusterIdx][0]) <= threshold) {
    //             pointsCluster[clusterIdx].append(newPoint);
    //             found = true;
    //             break;
    //         }
    //     }

    //     if (!found) {
    //         pointsCluster.append({newPoint});
    //         activeClusterIndices.append(pointsCluster.size() - 1);
    //     }

    //     if (i % 10000 == 0) {
    //         std::cout << "Processed " << i << " points, current clusters: " << pointsCluster.size() << ", Approximately "<< (double)i*100/intersectionPoints.size() << "% done" << std::endl;
    //     }
    // }
    //==================TOOOOOO SLOOOOOW=========================================
    // size_t i = 0;
    // for (const auto& newPoint : std::as_const(intersectionPoints)) {
    //     bool found = false;
    //     i++;

    //     // Check if cluster for this point already exists
    //     for (int i = 0; i < pointsCluster.size(); ++i) {
    //         if (newPoint.distanceTo(pointsCluster[i][0]) <= threshold) {
    //             pointsCluster[i].append(newPoint);
    //             found = true;
    //             break;
    //         }
    //     }

    //     // If not exist - create new cluster
    //     if (!found) {
    //         pointsCluster.append({newPoint});
    //         if(pointsCluster.size()%1000 == 0) {
    //             std::cout << "Append new cluster " << pointsCluster.size() << ", Approximately "<< (double)i*100/intersectionPoints.size() << "% done" << std::endl;
    //         }
    //     }
    // }
    //======================================================

    std::sort(pointsCluster.begin(), pointsCluster.end(),
              [](const  QVector<Line::Point> &a, const QVector<Line::Point> &b) {
                return a.size() > b.size(); // '>' for sorting from largest to smallest
              });

    std::cout << "\nStatistics of intersections:" << std::endl;
    for (int i = 0; i <= N && i < pointsCluster.size(); i++) {
        std::cout << "Point " << i << ": (" << pointsCluster[i][0].x+0.0 << ", " << pointsCluster[i][0].y+0.0
                  << ") repeated " << pointsCluster[i].size() << " times." << std::endl;
    }


    //Step 5: remove unnecessary clusters. We need to find N points; ???Also the number of points in clusters should not be differ by more than 2: 1/2 <= Mi/Mj <= 2
    /*double maxPointsInClusters = (double)pointsCluster[0].second;
    for (int i = pointsCluster.size() - 1; i >= 0; --i){
        double ratio = maxPointsInClusters / pointsCluster[i].second;

        if (pointsCluster.size() > N && ratio > 2.0) {
            pointsCluster.removeAt(i);
        } else {
            break;
        }
    }*/

    if (pointsCluster.size() > N) {
        pointsCluster.resize(N);
    }

    //Step 6: Find the center of masses for each cluster and put this point into QVector
    QVector<Line::Point> desiredPoints;
    for (int i = 0; i < pointsCluster.size(); ++i){
        Line::Point centerOfMass;
        double xSum = 0.0, ySum = 0.0;
        for(const auto& point : std::as_const(pointsCluster[i])){
            xSum += point.x;
            ySum += point.y;
        }
        centerOfMass.x = xSum/pointsCluster[i].size();
        centerOfMass.y = ySum/pointsCluster[i].size();

        desiredPoints.append(centerOfMass);
    }

    std::sort(desiredPoints.begin(), desiredPoints.end(), [](const Line::Point& a, const Line::Point& b) {
        if (a.x != b.x) {
            return a.x < b.x;
        }
        return a.y < b.y;
    });

    std::cout << "\nFound desired points:\n";
    i = 0;
    for(const auto&desiredPoint : desiredPoints){
        i++;
        std::cout << i << " : (" << desiredPoint.x+0.0 << ", " << desiredPoint.y+0.0 << ")" << std::endl;
    }

    return 0; //a.exec();
}


