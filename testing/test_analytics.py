import unittest
import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from analytics.performance_analyzer import SurgicalPerformanceAnalyzer

class TestSurgicalAnalytics(unittest.TestCase):
    
    def setUp(self):
        self.analyzer = SurgicalPerformanceAnalyzer()
    
    def test_safety_score_calculation(self):
        """Test safety score calculation with various inputs"""
        test_data = {
            'force_readings': [5.0, 6.0, 4.5],  # All within limits
            'joint_velocities': [10.0, 15.0, 20.0],  # All within limits
            'positions': [30.0, 20.0, 15.0],  # All within limits
            'obstacle_distances': [10.0, 15.0, 20.0],  # All safe distances
            'emergency_stop': False
        }
        
        safety_score = self.analyzer.calculate_safety_score(test_data)
        self.assertGreaterEqual(safety_score, 90.0)  # Should be high with good data
    
    def test_force_anomaly_detection(self):
        """Test detection of force anomalies"""
        # Simulate force spike
        force_readings = [5.0, 5.2, 5.1, 15.5, 5.3, 5.2]  # Spike at index 3
        anomalies = self.analyzer.detect_force_anomalies(force_readings)
        
        # Should detect the force spike
        self.assertGreater(len(anomalies), 0)
    
    def test_performance_analysis_completeness(self):
        """Test that performance analysis returns all expected metrics"""
        test_robot_data = {
            'force_readings': [5.0, 6.0, 4.5],
            'joint_velocities': [10.0, 15.0, 20.0],
            'positions': [30.0, 20.0, 15.0],
            'obstacle_distances': [10.0, 15.0, 20.0],
            'procedure_phase': 'SUTURING'
        }
        
        results = self.analyzer.analyze_real_time_performance(test_robot_data)
        
        expected_keys = [
            'procedure_time', 'movement_efficiency', 'safety_compliance',
            'instrument_usage', 'risk_assessment', 'anomalies_detected',
            'performance_gap', 'recommendations'
        ]
        
        for key in expected_keys:
            self.assertIn(key, results)

if __name__ == '__main__':
    unittest.main()
