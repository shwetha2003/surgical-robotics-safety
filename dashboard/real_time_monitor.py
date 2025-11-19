import time
import threading
import queue
import random
from datetime import datetime
from typing import Dict, List
import json

class RealTimeMonitor:
    def __init__(self):
        self.safety_metrics = {
            'safety_score': 95.0,
            'emergency_stop': False,
            'force_readings': [0.0, 0.0, 0.0],
            'joint_positions': [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            'joint_velocities': [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            'obstacle_distances': [10.0, 15.0, 20.0],
            'procedure_phase': 'INITIALIZATION',
            'timestamp': datetime.now()
        }
        self.monitoring_active = False
        self.data_queue = queue.Queue()
        
    def start_monitoring(self):
        """Start the real-time monitoring loop"""
        self.monitoring_active = True
        print("Real-time safety monitoring started...")
        
        while self.monitoring_active:
            try:
                # Simulate real-time robot data (replace with actual DDS data)
                self.simulate_robot_data()
                
                # Update safety metrics
                self.update_safety_metrics()
                
                # Put data in queue for dashboard
                self.data_queue.put(self.safety_metrics.copy())
                
                time.sleep(0.1)  # 10 Hz update rate
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1)
    
    def simulate_robot_data(self):
        """Simulate robot data for demonstration"""
        # Simulate force readings (normally 0-10N, occasionally spiking)
        base_force = random.uniform(2.0, 8.0)
        if random.random() < 0.05:  # 5% chance of force spike
            base_force = random.uniform(12.0, 18.0)
            
        self.safety_metrics['force_readings'] = [
            base_force + random.uniform(-1.0, 1.0),
            base_force + random.uniform(-1.0, 1.0), 
            base_force + random.uniform(-1.0, 1.0)
        ]
        
        # Simulate joint positions
        self.safety_metrics['joint_positions'] = [
            random.uniform(-30, 30),
            random.uniform(-20, 20),
            random.uniform(-15, 15),
            random.uniform(-10, 10),
            random.uniform(-5, 5),
            random.uniform(-2, 2)
        ]
        
        # Simulate occasional emergency stop
        if random.random() < 0.01:  # 1% chance
            self.safety_metrics['emergency_stop'] = True
        elif random.random() < 0.02:  # 2% chance to resume
            self.safety_metrics['emergency_stop'] = False
            
        self.safety_metrics['timestamp'] = datetime.now()
    
    def update_safety_metrics(self):
        """Update safety score based on current data"""
        safety_score = 95.0  # Base score
        
        # Penalize for high forces
        max_force = max(self.safety_metrics['force_readings'])
        if max_force > 15.0:
            safety_score -= 20.0
        elif max_force > 10.0:
            safety_score -= 10.0
            
        # Penalize for emergency stop
        if self.safety_metrics['emergency_stop']:
            safety_score -= 30.0
            
        # Ensure score stays in bounds
        safety_score = max(0.0, min(100.0, safety_score))
        self.safety_metrics['safety_score'] = round(safety_score, 2)
    
    def get_latest_safety_metrics(self) -> Dict:
        """Get the latest safety metrics"""
        return self.safety_metrics.copy()
    
    def get_latest_robot_data(self) -> Dict:
        """Get the latest robot data"""
        return self.safety_metrics.copy()
    
    def stop_monitoring(self):
        """Stop the monitoring loop"""
        self.monitoring_active = False
        print("Real-time safety monitoring stopped.")
