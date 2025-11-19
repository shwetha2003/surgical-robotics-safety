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
                    style={'padding': '15px', 'border': '1px solid #bdc3c7', 'borderRadius': '5px'})
        ], className="six columns"),
        
        html.Div([
            html.H4("Recent Safety Events"),
            html.Div(id="recent-events",
                    style={'height': '200px', 'overflowY': 'scroll', 'padding': '10px',
                          'border': '1px solid #bdc3c7', 'borderRadius': '5px'})
        ], className="six columns"),
    ], className="row"),
    
    # Auto-refresh component
    dcc.Interval(id='interval-component', interval=1000, n_intervals=0),
    
    # Hidden div to store latest data
    dcc.Store(id='latest-data-store')
], style={'padding': '20px', 'fontFamily': 'Arial, sans-serif'})

@app.callback(
    [Output('safety-status-indicator', 'children'),
     Output('safety-status-indicator', 'style'),
     Output('emergency-stop-status', 'children'),
     Output('emergency-stop-status', 'style')],
    [Input('interval-component', 'n_intervals')]
)
def update_safety_status(n):
    """Update safety status indicators"""
    try:
        # Get latest safety data
        safety_data = monitor.get_latest_safety_metrics()
        safety_score = safety_data.get('safety_score', 0)
        emergency_stop = safety_data.get('emergency_stop', False)
        
        # Determine safety status
        if safety_score >= 90:
            status_text = "EXCELLENT"
            status_style = {'color': 'green', 'backgroundColor': '#d4edda', 'border': '2px solid #28a745'}
        elif safety_score >= 75:
            status_text = "GOOD"
            status_style = {'color': 'orange', 'backgroundColor': '#fff3cd', 'border': '2px solid #ffc107'}
        else:
            status_text = "ATTENTION REQUIRED"
            status_style = {'color': 'red', 'backgroundColor': '#f8d7da', 'border': '2px solid #dc3545'}
        
        # Emergency stop status
        if emergency_stop:
            stop_text = "🔴 ENGAGED"
            stop_style = {'color': 'white', 'backgroundColor': '#dc3545', 'border': '2px solid #c82333'}
        else:
            stop_text = "🟢 NORMAL"
            stop_style = {'color': 'white', 'backgroundColor': '#28a745', 'border': '2px solid #218838'}
        
        return f"{status_text} ({safety_score}%)", status_style, stop_text, stop_style
        
    except Exception as e:
        return "ERROR", {'color': 'red'}, "UNKNOWN", {'color': 'red'}

@app.callback(
    Output('force-metrics-graph', 'figure'),
    [Input('interval-component', 'n_intervals')]
)
def update_force_metrics(n):
    """Update force metrics graph"""
    try:
        data = monitor.get_latest_robot_data()
        force_readings = data.get('force_readings', [0, 0, 0])
        
        fig = go.Figure()
        
        fig.add_trace(go.Indicator(
            mode = "gauge+number+delta",
            value = force_readings[0] if force_readings else 0,
            domain = {'x': [0, 1], 'y': [0, 1]},
            title = {'text': "Force X (N)"},
            delta = {'reference': 15},
            gauge = {
                'axis': {'range': [None, 20]},
                'bar': {'color': "darkblue"},
                'steps': [
                    {'range': [0, 10], 'color': "lightgray"},
                    {'range': [10, 15], 'color': "yellow"},
                    {'range': [15, 20], 'color': "red"}
                ],
                'threshold': {
                    'line': {'color': "red", 'width': 4},
                    'thickness': 0.75,
                    'value': 15
                }
            }
        ))
        
        fig.update_layout(height=300, margin=dict(l=20, r=20, t=50, b=20))
        return fig
        
    except Exception as e:
        return go.Figure()

# Add more callback functions for other graph updates...

if __name__ == '__main__':
    # Start background monitoring thread
    monitor_thread = threading.Thread(target=monitor.start_monitoring, daemon=True)
    monitor_thread.start()
    
    app.run_server(debug=True, host='0.0.0.0', port=8050)
