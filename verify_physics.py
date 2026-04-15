import sys
import os

# Add Backend folder to path
sys.path.append(os.path.join(os.getcwd(), "Backend"))

from solver.equation_graph import EquationGraph
from solver.coordinate_engine import CoordinateEngine

def test_physics_engine():
    print("Testing Turbomachinery Physics Engine...")
    solver = EquationGraph()
    coords = CoordinateEngine()
    
    # Sample problem: Basic Velocity Triangle Stage
    # Given: U1, Cm1, alpha1, beta2, Cm2, U2
    knowns = {
        "U1": 250.0,
        "Cm1": 150.0,
        "alpha1": 0.0,
        "beta2": 0.785, # ~45 degrees
        "Cm2": 150.0,
        "U2": 250.0
    }
    
    try:
        print(f"Solving with inputs: {knowns}")
        results, steps = solver.solve(knowns)
        
        # Check for key results
        required_keys = ["U1", "Cm1", "W1", "beta1", "Cu1"]
        missing = [k for k in required_keys if k not in results]
        
        if missing:
            print(f"FAILED: Missing results for {missing}")
            return False
            
        print("SUCCESS: Solver returned primary velocity triangle variables.")
        print(f"Mean Blade Speed (U1): {results['U1']:.2f} m/s")
        print(f"Axial Velocity (Cm1): {results['Cm1']:.2f} m/s")
        
        # Test Coordinate Engine
        print("Testing Coordinate Engine...")
        points = coords.generate(results)
        if not points:
            print("FAILED: Coordinate engine returned no points.")
            return False
            
        print("SUCCESS: Coordinate engine generated triangle geometry.")
        return True
        
    except Exception as e:
        print(f"ERROR during physics test: {str(e)}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_physics_engine()
    sys.exit(0 if success else 1)
