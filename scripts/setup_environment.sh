#!/bin/bash

# Surgical Robotics Safety Platform - Environment Setup Script
echo "Setting up Surgical Robotics Safety Platform..."

# Check if running as root (for some package installations)
if [ "$EUID" -eq 0 ]; then
    echo "Please do not run as root"
    exit 1
fi

# Update package list
echo "Updating package list..."
sudo apt-get update

# Install C++ build dependencies
echo "Installing C++ build dependencies..."
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libeigen3-dev \
    libboost-all-dev

# Install Python dependencies
echo "Installing Python dependencies..."
sudo apt-get install -y \
    python3 \
    python3-pip \
    python3-venv

# Create Python virtual environment
echo "Creating Python virtual environment..."
python3 -m venv venv
source venv/bin/activate

# Install Python packages
echo "Installing Python packages..."
pip install --upgrade pip
pip install -r analytics/requirements.txt
pip install -r dashboard/requirements.txt

# Install DDS dependencies (RTI Connext)
echo "Installing DDS dependencies..."
# Note: RTI Connext requires manual installation from RTI website
# This is a placeholder for the installation process
echo "Please install RTI Connext DDS manually from: https://www.rti.com/downloads"

# Create necessary directories
echo "Creating project directories..."
mkdir -p logs
mkdir -p data
mkdir -p build

# Set up permissions
echo "Setting up permissions..."
chmod +x scripts/*.sh

echo "Environment setup complete!"
echo "To activate Python environment: source venv/bin/activate"
echo "To build project: ./scripts/build_project.sh"
