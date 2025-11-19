#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include <eigen3/Eigen/Dense>
#include <vector>

class CollisionDetector {
private:
    double min_safe_distance;
    double warning_distance;
    
public:
    CollisionDetector();
    
    // Check for collisions between instrument and obstacles
    bool checkInstrumentCollision(const Eigen::Vector3d& instrument_tip,
                                 const std::vector<Eigen::Vector3d>& obstacles);
    
    // Check for self-collision between robot components
    bool checkSelfCollision(const std::vector<Eigen::Vector3d>& joint_positions);
    
    // Calculate minimum distance to any obstacle
    double calculateMinimumDistance(const Eigen::Vector3d& point,
                                   const std::vector<Eigen::Vector3d>& obstacles);
    
    // Configuration methods
    void setSafetyMargins(double min_safe, double warning);
    
    // Getters
    double getMinSafeDistance() const { return min_safe_distance; }
    double getWarningDistance() const { return warning_distance; }
};

#endif // COLLISION_DETECTOR_H
