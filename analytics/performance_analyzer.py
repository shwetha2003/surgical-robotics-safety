import numpy as np
import pandas as pd
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass
from datetime import datetime
import json
import warnings
warnings.filterwarnings('ignore')

@dataclass
class SurgicalMetrics:
    procedure_duration: float
    instrument_efficiency: float  
    movement_economy: float
    safety_violations: int
    force_variability: float
    completion_rate: float
    timestamp: datetime

@dataclass
class SafetyAlert:
    alert_id: str
    severity: str  # LOW, MEDIUM, HIGH, CRITICAL
    message: str
    timestamp: datetime
    recommended_action: str
    robot_component: str

class SurgicalPerformanceAnalyzer:
    def __init__(self, config_path: str = "config/safety_parameters.json"):
        self.metrics_history: List[SurgicalMetrics] = []
        self.safety_alerts: List[SafetyAlert] = []
        self.baseline_metrics = self.load_baseline_metrics()
        self.safety_thresholds = self.load_safety_thresholds(config_path)
        
    def load_baseline_metrics(self) -> Dict:
        """Load expert surgeon performance baselines"""
        return {
            'expert_procedure_time': 45.0,  # minutes
            'expert_movement_efficiency': 0.85,
            'expert_safety_score': 95.0,
            'expert_force_variability': 2.5
        }
    
    def load_safety_thresholds(self, config_path: str) -> Dict:
        """Load IEC 62304 compliant safety thresholds"""
        try:
            with open(config_path, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            # Default safety thresholds
            return {
                'max_force': 15.0,
                'max_velocity': 50.0,
                'min_safe_distance': 2.0,
                'max_joint_angle': 180.0,
                'safety_score_threshold': 75.0
            }
    
    def analyze_real_time_performance(self, robot_data: Dict) -> Dict:
        """Comprehensive real-time performance analysis"""
        try:
            metrics = {
                'procedure_time': self.calculate_procedure_efficiency(robot_data),
                'movement_efficiency': self.analyze_movement_economy(robot_data),
                'safety_compliance': self.calculate_safety_score(robot_data),
                'instrument_usage': self.analyze_instrument_efficiency(robot_data),
                'risk_assessment': self.assess_procedure_risks(robot_data),
                'anomalies_detected': self.detect_anomalies(robot_data)
            }
            
            # Compare against expert surgeon baselines
            performance_gap = self.compare_to_baseline(metrics)
            metrics['performance_gap'] = performance_gap
            metrics['recommendations'] = self.generate_improvement_recommendations(performance_gap)
            
            # Log metrics for historical analysis
            self.log_metrics(metrics, robot_data)
            
            return metrics
            
        except Exception as e:
            self.trigger_safety_alert("ANALYTICS_ERROR", f"Performance analysis failed: {str(e)}")
            return self.get_default_metrics()
    
    def calculate_safety_score(self, robot_data: Dict) -> float:
        """Calculate comprehensive safety score (0-100) based on IEC 62304"""
        try:
            safety_factors = {
                'force_compliance': self.check_force_limits(robot_data.get('force_readings', [])),
                'velocity_compliance': self.check_velocity_limits(robot_data.get('joint_velocities', [])),
                'boundary_compliance': self.check_workspace_boundaries(robot_data.get('positions', [])),
                'collision_avoidance': self.assess_collision_risk(robot_data.get('obstacle_distances', [])),
                'emergency_stop_status': 1.0 if not robot_data.get('emergency_stop', False) else 0.0
            }
            
            # IEC 62304 risk assessment weights
            weights = {
                'force_compliance': 0.30,
                'velocity_compliance': 0.25, 
                'boundary_compliance': 0.20,
                'collision_avoidance': 0.15,
                'emergency_stop_status': 0.10
            }
            
            safety_score = sum(score * weights[factor] for factor, score in safety_factors.items())
            return round(safety_score * 100, 2)
            
        except Exception as e:
            print(f"Safety score calculation error: {e}")
            return 0.0
    
    def check_force_limits(self, force_readings: List[float]) -> float:
        """Check force readings against safety limits"""
        if not force_readings:
            return 0.0
            
        max_force = self.safety_thresholds['max_force']
        compliant_readings = sum(1 for force in force_readings if force <= max_force)
        return compliant_readings / len(force_readings)
    
    def check_velocity_limits(self, velocities: List[float]) -> float:
        """Check velocity readings against safety limits"""
        if not velocities:
            return 0.0
            
        max_velocity = self.safety_thresholds['max_velocity']
        compliant_velocities = sum(1 for vel in velocities if abs(vel) <= max_velocity)
        return compliant_velocities / len(velocities)
    
    def check_workspace_boundaries(self, positions: List[float]) -> float:
        """Check if robot stays within safe workspace"""
        if not positions:
            return 0.0
            
        # Simplified boundary check - in practice would use 3D workspace model
        safe_positions = sum(1 for pos in positions if abs(pos) <= self.safety_thresholds['max_joint_angle'])
        return safe_positions / len(positions)
    
    def assess_collision_risk(self, obstacle_distances: List[float]) -> float:
        """Assess collision risk based on obstacle distances"""
        if not obstacle_distances:
            return 1.0  # No obstacles detected
            
        min_safe_distance = self.safety_thresholds['min_safe_distance']
        safe_distances = sum(1 for dist in obstacle_distances if dist >= min_safe_distance)
        return safe_distances / len(obstacle_distances)
    
    def detect_anomalies(self, real_time_data: Dict) -> List[SafetyAlert]:
        """Machine learning anomaly detection for safety events"""
        anomalies = []
        
        # Statistical process control for force readings
        force_anomalies = self.detect_force_anomalies(real_time_data.get('force_readings', []))
        anomalies.extend(force_anomalies)
        
        # Pattern recognition for unusual movement patterns
        movement_anomalies = self.detect_movement_anomalies(real_time_data.get('trajectory', []))
        anomalies.extend(movement_anomalies)
        
        # Temporal anomalies for procedure timing
        timing_anomalies = self.detect_timing_anomalies(real_time_data.get('procedure_phase', ''))
        anomalies.extend(timing_anomalies)
        
        return anomalies
    
    def detect_force_anomalies(self, force_readings: List[float]) -> List[SafetyAlert]:
        """Detect anomalous force patterns using statistical methods"""
        if len(force_readings) < 10:
            return []
            
        alerts = []
        forces = np.array(force_readings)
        
        # Check for sudden force spikes
        force_diff = np.diff(forces)
        spike_threshold = 5.0  # Newtons
        spikes = np.where(np.abs(force_diff) > spike_threshold)[0]
        
        for spike_idx in spikes:
            alerts.append(SafetyAlert(
                alert_id=f"FORCE_SPIKE_{len(alerts)}",
                severity="MEDIUM",
                message=f"Sudden force change detected: {force_diff[spike_idx]:.2f}N",
                timestamp=datetime.now(),
                recommended_action="Check instrument contact",
                robot_component="Force_Sensors"
            ))
        
        return alerts
    
    def trigger_safety_alert(self, alert_type: str, message: str):
        """Trigger a new safety alert"""
        alert = SafetyAlert(
            alert_id=f"{alert_type}_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            severity="HIGH",
            message=message,
            timestamp=datetime.now(),
            recommended_action="Review system logs and contact engineering",
            robot_component="Safety_System"
        )
        self.safety_alerts.append(alert)
        
        # Keep only recent alerts
        if len(self.safety_alerts) > 1000:
            self.safety_alerts = self.safety_alerts[-1000:]
    
    def log_metrics(self, metrics: Dict, robot_data: Dict):
        """Log performance metrics for historical analysis"""
        surgical_metrics = SurgicalMetrics(
            procedure_duration=metrics['procedure_time'],
            instrument_efficiency=metrics['instrument_usage'],
            movement_economy=metrics['movement_efficiency'],
            safety_violations=len(metrics['anomalies_detected']),
            force_variability=np.std(robot_data.get('force_readings', [0])),
            completion_rate=1.0,  # Would be calculated from procedure data
            timestamp=datetime.now()
        )
        self.metrics_history.append(surgical_metrics)
    
    def get_default_metrics(self) -> Dict:
        """Return default metrics when analysis fails"""
        return {
            'procedure_time': 0.0,
            'movement_efficiency': 0.0,
            'safety_compliance': 0.0,
            'instrument_usage': 0.0,
            'risk_assessment': 'UNKNOWN',
            'anomalies_detected': [],
            'performance_gap': {},
            'recommendations': ['System under maintenance']
        }
