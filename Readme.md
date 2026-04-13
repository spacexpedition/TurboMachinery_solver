Intelligent Turbomachinery Solver ⚙️

A neuro-symbolic physics engine designed to bridge the gap between engineering education and automated design. The Intelligent Turbomachinery Solver eliminates tedious manual calculations and arithmetic propagation errors by instantly parsing word problems, solving complex thermodynamic equations, and rendering dynamic velocity triangles.

🚀 Key Features

Natural Language Processing (NLP): Type or paste textbook word problems directly into the app. The system uses a hybrid NLP approach (Google Gemini API with a local offline fallback) to extract known variables and units automatically.

Neuro-Symbolic Math Engine: Powered by SymPy, the solver traverses a non-linear graph of kinematics and thermodynamic equations (Euler turbomachine equations) to solve for missing parameters—without relying on hardcoded, step-by-step procedural paths.

Dynamic Geometry Engine: Hardware-accelerated vector graphics automatically draw and scale inlet and outlet velocity triangles ($C, U, W$) in real-time based on the solved physics state.

Fully Portable: Designed to be compiled into a standalone Windows executable that can be run from a USB drive without requiring the user to install Python, C++, or any external dependencies.

🏗️ Hybrid Architecture

This application utilizes a modern local microservice architecture to combine the speed of C++ with the scientific computing power of Python.

The "Face" (Frontend): Built with C++ and Qt6. It handles the native Windows UI, hardware-accelerated vector graphics (QPainter), and asynchronous HTTP communication.

The "Brain" (Backend): Built with Python 3 and FastAPI. It runs as a hidden local microservice (127.0.0.1:8080) handling stateless RESTful JSON requests, symbolic math resolution, and NLP extraction.

💻 Developer Setup (CLion / CMake)

Prerequisites

C++: CMake (3.16+) and Qt6 (Core, Gui, Widgets, Network)

Python: Python 3.10+

1. Start the Backend

cd backend
python -m venv venv
venv\Scripts\activate  # On Windows
pip install -r requirements.txt

# Start the FastAPI server on port 8080
python api_server.py


2. Build and Run the Frontend

Open the project root in CLion (or your preferred CMake-compatible IDE).

Ensure CMake can locate your Qt6 installation (e.g., set -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64").

Build the TurbomachinerySolver executable.

Run the application.

Note: In the final compiled release, the C++ application will automatically spawn and manage the Python backend process using QProcess.

🗺️ Roadmap (Version 2.0)

3D Rendering: Integrating Qt3D / OpenGL to automatically extrude and rotate 3D blade representations based on the calculated $\beta$ and $\alpha$ angles.

OCR Integration: Allow users to take a photograph of a textbook problem, extract the text using local Tesseract OCR, and feed it directly into the solver.

PDF Reporting: One-click generation of fully formatted, step-by-step technical reports suitable for university homework submission or corporate documentation.

🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the issues page if you want to contribute.

📝 License

This project is licensed under the MIT License - see the LICENSE file for details.