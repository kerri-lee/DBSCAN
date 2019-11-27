#include <stdio.h>
#include <iostream>
#include "dbscan.h"
#include "dbscan.cpp"

#define MINIMUM_POINTS 4     // minimum number of cluster
#define EPSILON (0.4*0.4)  // distance for clustering, metre^2

void readBenchmarkData(vector<Point>& points)
{
    // load point cloud
    FILE *stream;
    stream = fopen ("test_data_three.dat","r"); // may require full file path!

    unsigned int num_points = 0;
    if (stream != NULL)
    {
        // Allocating memory (inizialize to 0) for a Point
        Point *p = (Point *)calloc(1, sizeof(Point));
        // Reads data from the stream and stores them according to the parameter
        // format into the locations pointed by the additional arguments
        while (fscanf(stream, "%f,%f,%f,%f,%f,%f\n", &(p->x), &(p->y), &(p->z), &(p->xVel), &(p->yVel), &(p->zVel)) != EOF) {
            // assign all points as unclassified
            p->clusterID = UNCLASSIFIED;

            // Adds a new element at the end of the vector, after its current last element
            // The content of p[i] is copied (or moved) to the new element
            points.push_back(*p);
            ++num_points;
        }
        free(p);
        fclose(stream);
    } else {
        printf("Cannot find file\n");
    }
}

void printResults(vector<Point>& points, int num_points)
{
// For printing the results to std out
//    int i = 0;
//    printf("Number of points: %u\n"
//           " x     y     xvel     yvel     cluster_id\n"
//           "-----------------------------------------\n"
//            , num_points);
//    while (i < num_points)
//    {
//        printf("%5.2lf %5.2lf  %5.2lf   %5.2lf:     %d\n",
//               points[i].x,
//               points[i].y,
//               points[i].z,
//               points[i].xVel,
//               points[i].yVel,
//               points[i].zVel,
//               points[i].clusterID);
//        ++i;
//    }

// For print the results to a .dat file (only coordinates and clusterID)
    int i = 0;
    FILE *stream;
    stream = fopen ("test_data_three_results.dat","w"); // may require full file path!
    if (stream != NULL) {
        while (i < num_points) {
            fprintf(stream, "%5.5lf,%5.5lf,%5.5lf,%d\n",
                    points[i].x,
                    points[i].y,
                    points[i].z,
                    points[i].clusterID);
            ++i;
        }
    }
}

int main()
{
    vector<Point> points;
    // read point data
    readBenchmarkData(points);

    // constructor
    DBSCAN ds(MINIMUM_POINTS, EPSILON, &points);

    // main loop
    ds.run();

    // result of DBSCAN algorithm
    printResults(points, ds.getTotalPointSize());

    return 0;
}
