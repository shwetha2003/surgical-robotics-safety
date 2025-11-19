from dash import Dash, html, dcc, Input, Output, callback
import plotly.graph_objects as go
import plotly.express as px
from datetime import datetime
import pandas as pd
import threading
import queue
import json

from real_time_monitor import RealTimeMonitor
from safety_alerts import SafetyAlertManager

# Initialize components
app = Dash(__name__)
monitor = RealTimeMonitor()
alert_manager = SafetyAlertManager()

# Global data queue for real-time updates
data_queue = queue.Queue()

app.layout = html.Div([
    html.Div([
        html.H1("J&J Surgical Robotics Safety Dashboard", 
               style={'textAlign': 'center', 'color': '#2c3e50', 'marginBottom': '30px'}),
    ]),
    
    # Safety Status Overview
    html.Div([
        html.Div([
            html.H3("Overall Safety Status", style={'textAlign': 'center'}),
            html.Div(id="safety-status-indicator", 
                    style={'fontSize': '32px', 'fontWeight': 'bold', 'textAlign': 'center', 
                          'padding': '20px', 'borderRadius': '10px', 'margin': '10px'}),
        ], className="six columns"),
        
        html.Div([
            html.H3("Emergency Stop Status", style={'textAlign': 'center'}),
            html.Div(id="emergency-stop-status",
                    style={'fontSize': '32px', 'fontWeight': 'bold', 'textAlign': 'center',
                          'padding': '20px', 'borderRadius': '10px', 'margin': '10px'}),
        ], className="six columns"),
    ], className="row", style={'marginBottom': '30px'}),
    
    # Real-time Metrics Row
    html.Div([
        html.Div([
            dcc.Graph(id='force-metrics-graph'),
        ], className="four columns"),
        
        html.Div([
            dcc.Graph(id='position-metrics-graph'),
        ], className="four columns"),
        
        html.Div([
            dcc.Graph(id='safety-score-trend'),
        ], className="four columns"),
    ], className="row", style={'marginBottom': '30px'}),
    
    # Performance Metrics
    html.Div([
        html.Div([
            dcc.Graph(id='performance-metrics'),
        ], className="six columns"),
        
        html.Div([
            dcc.Graph(id='anomaly-detection'),
        ], className="six columns"),
    ], className="row", style={'marginBottom': '30px'}),
    
    # Alerts Panel
    html.Div([
        html.H3("Real-Time Safety Alerts", style={'textAlign': 'center'}),
        html.Div(id="safety-alerts-panel",
                style={'height': '300px', 'overflowY': 'scroll', 'border': '2px solid #e74c3c',
                      'borderRadius': '10px', 'padding': '15px', 'backgroundColor': '#f9f9f9'}),
    ], style={'marginBottom': '30px'}),
    
    # System Information
    html.Div([
        html.Div([
            html.H4("System Information"),
            html.Div(id="system-info",
                    style={'padding': '15px', 'border': '
