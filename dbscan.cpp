#include "dbscan.h"

int DBSCAN::run()
{
    int clusterID = 1; // current cluster id
    vector<Point>::iterator iter; // create vector of points from 2D array

    // iterate through all points
    for (iter = m_points->begin(); iter != m_points->end(); ++iter) {
        // only analyze unprocessed points
        if ( iter->clusterID == UNCLASSIFIED ) {
            // try to expand the cluster using this point
            if ( expandCluster(*iter, clusterID) != FAILURE ) {
                // move on to next cluster if the point led to a cluster formation
                clusterID += 1;
            } else { // expansion of point failed
                iter->clusterID = NOISE;
            }
        }
    }
    return 0;
}

int DBSCAN::expandCluster(Point point, int clusterID)
{
    // returns a vector of indices of the initial neighbors of 'point' - seed set
    vector<int> clusterSeeds = calculateCluster(point);

    if ( clusterSeeds.size() < m_minPoints )
    { // cannot be considered a part of a cluster
        // not a core point, so it is noise - cannot be expanded further
        return FAILURE;
    }
    else {
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        // analyze each neighbor point (includes the core point) of the core point - iterating through seed set
        for ( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
        {
            // accessing the neighbor point using its index, assign it to the cluster
            m_points->at(*iterSeeds).clusterID = clusterID;
            // if we are looking at the core point - mark its index as the core point index
            if ( m_points->at(*iterSeeds).x == point.x && m_points->at(*iterSeeds).y == point.y)
            {
                indexCorePoint = index;
            }
            ++index;
        }
        // Removes from the vector either a single element (position) or a range of elements ([first,last))
        // removing the core point from the vector of cluster seeds
        clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);

        // vector with size 0 - runs until vector is size of clusterSeeds vector
        for ( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            // finding the neighbors of the current cluster neighbor point (neighbors of neighbors)
            vector<int> clusterNeighbors = calculateCluster(m_points->at(clusterSeeds[i]));

            // if current neighbor point can be considered a core point as well
            if ( clusterNeighbors.size() >= m_minPoints )
            {
                vector<int>::iterator iterNeighbors;
                // iterate through its neighbors
                for ( iterNeighbors = clusterNeighbors.begin(); iterNeighbors != clusterNeighbors.end(); ++iterNeighbors )
                {
                    // if any of those neighbors were previously classified as noise or are still unclassified
                    if ( m_points->at(*iterNeighbors).clusterID == UNCLASSIFIED || m_points->at(*iterNeighbors).clusterID == NOISE )
                    {
                        if ( m_points->at(*iterNeighbors).clusterID == UNCLASSIFIED )
                        {
                            // add to seeds vector (the for-loop currently running will now need to access this point as well)
                            clusterSeeds.push_back(*iterNeighbors);
                            n = clusterSeeds.size(); // update the with the new size of clusterSeeds
                        }
                        // assign this neighbor-of-neighbor point as apart of the current cluster
                        m_points->at(*iterNeighbors).clusterID = clusterID;
                    }
                }
            }
        }
        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(Point point)
{
    int index = 0;
    vector<Point>::iterator iter;
    vector<int> clusterIndex;
    // run through all data points
    for ( iter = m_points->begin(); iter != m_points->end(); ++iter)
    {
        // *iter is address of current point
        if ( calculateDistance(point, *iter) <= m_epsilon )
        {
            // add index of point to the cluster index
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

inline float DBSCAN::calculateDistance( Point pointCore, Point pointTarget )
{
    return pow(pointCore.x - pointTarget.x,2) + pow(pointCore.y - pointTarget.y,2);
}


