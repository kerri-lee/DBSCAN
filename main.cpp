#include <stdio.h>
#include <iostream>
#include "dbscan.h"

#define MINIMUM_POINTS 4     // minimum number of cluster
#define EPSILON (0.75*0.75)  // distance for clustering, metre^2

void readBenchmarkData(vector<Point>& points) {
    // load point cloud
    FILE *stream;
    stream = fopen ("benchmark_hepta.dat","ra");

    unsigned int minpts, num_points, cluster, i = 0;
    double epsilon;

    // First line read in is the number of points in the data set
    fscanf(stream, "%u\n", &num_points);

    // Allocating memory (initialize to 0) for array of pointers
    Point *p = (Point *)calloc(num_points, sizeof(Point));

    while (i < num_points) {
        // Reads data from the stream and stores them according to the parameter
        // format into the locations pointed by the additional arguments
        fscanf(stream, "%f,%f,%f,%d\n", &(p[i].x), &(p[i].y), &cluster);

        // assign all points as unclassified
        p[i].clusterID = UNCLASSIFIED;

        // Adds a new element at the end of the vector, after its current last element
        // The content of p[i] is copied (or moved) to the new element
        points.push_back(p[i]);
        ++i;
    }
    free(p);
    fclose(stream);
}

void printResults(vector<Point>& points, int num_points) {
    int i = 0;
    printf("Number of points: %u\n"
           " x     y     cluster_id\n"
           "-----------------------------\n"
            , num_points);
    while (i < num_points) {
        printf("%5.2lf %5.2lf: %d\n",
               points[i].x,
               points[i].y,
               points[i].clusterID);
        ++i;
    }

}

int main() {
    vector<Point> points;
    // read point data
    readBenchmarkData(points);

    float pointArray [points.size()][4]; // array to pass in

    for (int i = 0; i < points.size(); ++i) {
        pointArray[i][0] = points.at(i).x;
        pointArray[i][1] = points.at(i).y;
        pointArray[i][2] = points.at(i).xVel;
        pointArray[i][3] = points.at(i).yVel;
    }

    // constructor
    DBSCAN ds(MINIMUM_POINTS, EPSILON, pointArray);

    // main loop
    ds.run();

    // result of DBSCAN algorithm
//    printResults(ds.m_points, ds.getTotalPointSize());

    return 0;
}