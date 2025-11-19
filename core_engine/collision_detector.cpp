#include "collision_detector.h"
#include <iostream>
#include <cmath>
#include <algorithm>

CollisionDetector::CollisionDetector() {
    // Initialize with default safety margins
    min_safe_distance = 2.0;  // 2mm minimum safe distance
    warning_distance = 5.0;   // 5mm warning distance
}

bool CollisionDetector::checkInstrumentCollision(const Eigen::Vector3d& instrument_tip,
                                                const std::vector<Eigen::Vector3d>& obstacles) {
    for (const auto& obstacle : obstacles) {
        double distance = (instrument_tip - obstacle).norm();
        
        if (distance < min_safe_distance) {
            std::cout << "🚨 COLLISION DETECTED! Distance: " << distance << "mm" << std::endl;
            return true;
        } else if (distance < warning_distance) {
            std::cout << "⚠️  Collision warning! Distance: " << distance << "mm" << std::endl;
        }
    }
    return false;
}

bool CollisionDetector::checkSelfCollision(const std::vector<Eigen::Vector3d>& joint_positions) {
    // Simplified self-collision detection between robot links
    // In a real system, this would use detailed robot geometry
    
    if (joint_positions.size() < 2) return false;
    
    for (size_t i = 0; i < joint_positions.size() - 1; ++i) {
        for (size_t j = i + 2; j < joint_positions.size(); ++j) {
            double distance = (joint_positions[i] - joint_positions[j]).norm();
            
            if (distance < min_safe_distance * 2) {  // Larger margin for self-collision
                std::cout << "🤖 Self-collision risk between joints " << i << " and " << j 
                          << ", distance: " << distance << "mm" << std::endl;
                return true;
            }
        }
    }
    return false;
}

double CollisionDetector::calculateMinimumDistance(const Eigen::Vector3d& point,
                                                  const std::vector<Eigen::Vector3d>& obstacles) {
    if (obstacles.empty()) return std::numeric_limits<double>::max();
    
    double min_distance = std::numeric_limits<double>::max();
    for (const auto& obstacle : obstacles) {
        double distance = (point - obstacle).norm();
        min_distance = std::min(min_distance, distance);
    }
    
    return min_distance;
}

void CollisionDetector::setSafetyMargins(double min_safe, double warning) {
    min_safe_distance = min_safe;
    warning_distance = warning;
    std::cout << "Safety margins updated - Min: " << min_safe_distance 
              << "mm, Warning: " << warning_distance << "mm" << std::endl;
}
