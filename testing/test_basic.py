import unittest
import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

class TestBasicFunctionality(unittest.TestCase):
    def test_imports(self):
        """Test that required packages can be imported"""
        try:
            import numpy
            import pandas
            import dash
            import plotly
            print("✅ All imports successful")
            self.assertTrue(True)
        except ImportError as e:
            self.fail(f"Import failed: {e}")
    
    def test_basic_math(self):
        """Test basic functionality"""
        self.assertEqual(1 + 1, 2)
    
    def test_environment(self):
        """Test Python environment"""
        import sys
        self.assertTrue(sys.version_info >= (3, 8))
        print(f"✅ Python version: {sys.version}")

if __name__ == '__main__':
    unittest.main()
