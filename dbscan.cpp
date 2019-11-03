#include "dbscan.h"

float* DBSCAN::run() {
    int clusterID = 1; // current cluster id
    numClusters = 0;
    int rows = sizeof(m_points) / sizeof(m_points[0]);
    maxClusterSize = 0;

    // create vector of points from 2D array
    for (int r = 0; r < rows; ++r) {
        points.at(r).x = m_points[r][0];
        points.at(r).y = m_points[r][1];
        points.at(r).xVel = m_points[r][2];
        points.at(r).yVel = m_points[r][3];
        points.at(r).clusterID = UNCLASSIFIED;
    }

    vector<Point>::iterator iter;
    // iterate through all points
    for (iter = points.begin(); iter != points.end(); ++iter) {
        // only analyze unprocessed points
        if ( iter->clusterID == UNCLASSIFIED ) {
            // try to expand the cluster using this point
            if ( expandCluster(*iter, clusterID) != FAILURE ) {
                numClusters++;
                // move on to next cluster if the point led to a cluster formation
                clusterID += 1;
            }
        }
    }
    // 3D array
    float* toReturn = new float[numClusters][maxClusterSize][4];
    int* currSizes = new int[numClusters];

    // after processing, convert vector to 3D array
    // 1st dimension: cluster id
    // 2nd dimension: points
    // 3rd dimension: point data
    for (iter = points.begin(); iter != points.end(); ++iter) {
        int id = iter->clusterID;
        if (id != NOISE) {
            toReturn[id - 1][currSizes[id - 1]][0] = iter->x;
            toReturn[id - 1][currSizes[id - 1]][1] = iter->y;
            toReturn[id - 1][currSizes[id - 1]][2] = iter->xVel;
            toReturn[id - 1][currSizes[id - 1]][3] = iter->yVel;
            ++currSizes[id - 1];
        }
    }
//    printResults(toReturn, numClusters, maxClusterSize);
    delete[] currSizes;
    return toReturn;
}

int DBSCAN::expandCluster(Point point, int clusterID) {
    // returns a vector of indices of the initial neighbors of 'point' - seed set
    vector<int> clusterSeeds = calculateCluster(point);

    if ( clusterSeeds.size() < m_minPoints ) { // cannot be considered a part of a cluster
        // not a core point, so it is noise - cannot be expanded further
        point.clusterID = NOISE;
        return FAILURE;
    }
    else {
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        // analyze each neighbor point (includes the core point) of the core point - iterating through seed set
        for ( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds) {
            // accessing the neighbor point using its index, assign it to the cluster
            points.at(*iterSeeds).clusterID = clusterID;
            // if we are looking at the core point - mark its index as the core point index
            if (points.at(*iterSeeds).x == point.x && points.at(*iterSeeds).y == point.y) {
                indexCorePoint = index;
            }
            ++index;
        }
        // Removes from the vector either a single element (position) or a range of elements ([first,last))
        // removing the core point from the vector of cluster seeds
        clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);

        // vector with size 0 - runs until vector is size of clusterSeeds vector
        for ( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i ) {
            // finding the neighbors of the current cluster neighbor point (neighbors of neighbors)
            vector<int> clusterNeighbors = calculateCluster(points.at(clusterSeeds[i]));

            // if current neighbor point can be considered a core point as well
            if ( clusterNeighbors.size() >= m_minPoints ) {
                vector<int>::iterator iterNeighbors;
                // iterate through its neighbors
                for ( iterNeighbors = clusterNeighbors.begin(); iterNeighbors != clusterNeighbors.end(); ++iterNeighbors ) {
                    // if any of those neighbors were previously classified as noise or are still unclassified
                    if ( points.at(*iterNeighbors).clusterID == UNCLASSIFIED || points.at(*iterNeighbors).clusterID == NOISE ) {
                        if ( points.at(*iterNeighbors).clusterID == UNCLASSIFIED ) {
                            // add to seeds vector (the for-loop currently running will now need to access this point as well)
                            clusterSeeds.push_back(*iterNeighbors);
                            n = clusterSeeds.size(); // update the with the new size of clusterSeeds
                        }
                        // assign this neighbor-of-neighbor point as apart of the current cluster
                        points.at(*iterNeighbors).clusterID = clusterID;
                    }
                }
            }
        }

        if (maxClusterSize < clusterSeeds.size() + 1) {
            maxClusterSize = clusterSeeds.size() + 1;
        }
        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(Point point) {
    int index = 0;
    vector<Point>::iterator iter;
    vector<int> clusterIndex;
    // run through all data points
    for (iter = points.begin(); iter != points.end(); ++iter) {
        // *iter is address of current point
        if (calculateDistance(point, *iter) <= m_epsilon ) {
            // add index of point to the cluster index
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

inline double DBSCAN::calculateDistance( Point pointCore, Point pointTarget ) {
    return pow(pointCore.x - pointTarget.x,2) + pow(pointCore.y - pointTarget.y,2);
}

//void DBSCAN::printResults(float* points, int numClusters, int maxClusterSize) {
//    for (int i = 0; i < numClusters; ++i) {
//        for (int j = 0; j < maxClusterSize; ++j) {
//            printf("%5.2lf %5.2lf %5.2lf %5.2lf: %d\n",
//            points[i][j][0],
//            points[i][j][1],
//            points[i][j][2],
//            points[i][j][3],
//            i);
//        }
//    }
//}


