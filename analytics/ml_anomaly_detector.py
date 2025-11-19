import numpy as np
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler
from typing import List, Dict, Any
import joblib

class MLAnomalyDetector:
    def __init__(self):
        self.force_model = IsolationForest(contamination=0.1, random_state=42)
        self.movement_model = IsolationForest(contamination=0.1, random_state=42)
        self.scaler = StandardScaler()
        self.is_trained = False
        
    def train_models(self, historical_data: List[Dict]):
        """Train anomaly detection models on historical surgical data"""
        if not historical_data:
            print("No historical data for training")
            return
            
        # Prepare features for force anomaly detection
        force_features = []
        movement_features = []
        
        for procedure in historical_data:
            forces = procedure.get('force_readings', [])
            movements = procedure.get('movement_patterns', [])
            
            if len(forces) >= 10:
                force_features.append(self.extract_force_features(forces))
            if len(movements) >= 10:
                movement_features.append(self.extract_movement_features(movements))
        
        if force_features:
            force_features = self.scaler.fit_transform(force_features)
            self.force_model.fit(force_features)
            
        if movement_features:
            movement_features = self.scaler.transform(movement_features)
            self.movement_model.fit(movement_features)
            
        self.is_trained = True
        print("Anomaly detection models trained successfully")
    
    def detect_force_anomalies(self, force_readings: List[float]) -> List[Dict]:
        """Detect anomalous force patterns using ML"""
        if not self.is_trained or len(force_readings) < 10:
            return []
            
        features = self.extract_force_features(force_readings)
        features_scaled = self.scaler.transform([features])
        
        prediction = self.force_model.predict(features_scaled)
        anomaly_score = self.force_model.decision_function(features_scaled)
        
        if prediction[0] == -1:
            return [{
                'type': 'FORCE_ANOMALY',
                'severity': 'HIGH' if abs(anomaly_score[0]) > 0.5 else 'MEDIUM',
                'score': float(anomaly_score[0]),
                'features': features
            }]
        
        return []
    
    def extract_force_features(self, forces: List[float]) -> List[float]:
        """Extract features from force readings for ML"""
        forces_array = np.array(forces)
        return [
            np.mean(forces_array),
            np.std(forces_array),
            np.max(forces_array),
            np.min(forces_array),
            np.median(forces_array),
            np.percentile(forces_array, 75) - np.percentile(forces_array, 25),  # IQR
            len([f for f in forces_array if f > 10.0]) / len(forces_array)  # % high forces
        ]
    
    def extract_movement_features(self, movements: List[List[float]]) -> List[float]:
        """Extract features from movement patterns"""
        movements_array = np.array(movements)
        if movements_array.size == 0:
            return [0.0] * 7
            
        return [
            np.mean(movements_array),
            np.std(movements_array),
            np.max(movements_array),
            np.min(movements_array),
            np.median(movements_array),
            np.mean(np.diff(movements_array, axis=0)),  # Average change
            np.std(np.diff(movements_array, axis=0))   # Variability of change
        ]
