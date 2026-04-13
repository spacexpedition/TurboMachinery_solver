import os
import re
import json
import urllib.request
import urllib.error

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from typing import Dict, Optional
import uvicorn

from solver.equation_graph import EquationGraph
from solver.coordinate_engine import CoordinateEngine

# =====================================================================
# 1. Configuration & Internet Checks
# =====================================================================
# In production, set this via environment variables
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY", "YOUR_API_KEY_HERE")

def check_internet_connection(host="http://8.8.8.8", timeout=2) -> bool:
    """Check if the system is currently connected to the internet."""
    try:
        urllib.request.urlopen(host, timeout=timeout)
        return True
    except urllib.error.URLError:
        return False

# =====================================================================
# 2. NLP Extraction Engines (Cloud vs Local)
# =====================================================================
def extract_variables_gemini(text: str) -> dict:
    """Uses Google's Gemini LLM to parse complex word problems into JSON."""
    if GEMINI_API_KEY == "YOUR_API_KEY_HERE":
        print("Warning: No Gemini API Key set. Falling back to local offline extractor.")
        return extract_variables_local(text)

    url = f"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={GEMINI_API_KEY}"

    # Prompt engineering to force a strict JSON response
    prompt = (
        "You are an engineering NLP extractor. Extract the turbomachinery known variables "
        "from the following text and return ONLY a valid JSON dictionary where keys are "
        "standard symbols (e.g., Cm1, alpha1, U1, Cm2, Cu2, m_dot) and values are floats. "
        f"Text to analyze: '{text}'"
    )

    payload = {
        "contents": [{"parts": [{"text": prompt}]}],
        "generationConfig": {"responseMimeType": "application/json"}
    }

    try:
        req = urllib.request.Request(
            url,
            data=json.dumps(payload).encode('utf-8'),
            headers={'Content-Type': 'application/json'}
        )
        with urllib.request.urlopen(req) as response:
            res_body = response.read()
            data = json.loads(res_body)
            text_response = data['candidates'][0]['content']['parts'][0]['text']
            return json.loads(text_response)
    except Exception as e:
        print(f"Gemini API connection failed: {e}. Falling back to local offline extractor.")
        return extract_variables_local(text)

def extract_variables_local(text: str) -> dict:
    """
    Local fallback using Regex/SpaCy.
    Runs entirely offline on the user's local machine.
    """
    print("Running local offline NLP extraction...")
    extracted = {}

    if re.search(r"meridional velocity.*?(?:of|is)?\s*([\d.]+)", text, re.IGNORECASE):
        extracted["Cm1"] = float(re.search(r"meridional velocity.*?(?:of|is)?\s*([\d.]+)", text, re.IGNORECASE).group(1))

    if re.search(r"blade speed.*?(?:of|is)?\s*([\d.]+)", text, re.IGNORECASE):
        extracted["U1"] = float(re.search(r"blade speed.*?(?:of|is)?\s*([\d.]+)", text, re.IGNORECASE).group(1))

    return extracted

# =====================================================================
# 3. JSON Schema Definitions (The Contract)
# =====================================================================
class SolverRequest(BaseModel):
    """
    JSON Schema for incoming data from the C++ Frontend.
    """
    problem_text: Optional[str] = Field(
        None,
        description="Optional: A natural language turbomachinery word problem to be parsed."
    )
    known_variables: Dict[str, float] = Field(
        default_factory=dict,
        json_schema_extra={
            "example": {
                "Cm1": 50.0,
                "alpha1": 0.5236,
                "U1": 100.0,
            }
        },
        description="Key-value pairs of known turbomachinery parameters."
    )

class SolverResponse(BaseModel):
    """
    JSON Schema for outgoing data to the C++ Frontend.
    """
    status: str = Field(..., description="'success' or 'error'")
    nlp_engine_used: str = Field(..., description="'Gemini (Cloud)', 'SpaCy (Local)', or 'None'")
    results: Dict[str, float] = Field(default_factory=dict, description="Fully resolved parameters.")
    coordinates: Dict = Field(default_factory=dict, description="Cartesian points for rendering triangles.")
    message: Optional[str] = Field(None, description="Error message or warnings if any.")

# =====================================================================
# 4. FastAPI Application Initialization
# =====================================================================
app = FastAPI(
    title="Intelligent Turbomachinery Solver API",
    description="Local microservice for symbolic physics resolution with Hybrid NLP.",
    version="1.0.0"
)

# Instantiate the real symbolic math engine and coordinate generator
solver_engine = EquationGraph()
coord_engine = CoordinateEngine()

# =====================================================================
# 5. RESTful Endpoints
# =====================================================================
@app.post("/api/solve", response_model=SolverResponse)
async def solve_turbomachinery(request: SolverRequest):
    """
    Receives input from Qt6. Routes text to Cloud/Local NLP, then passes variables to SymPy.
    """
    try:
        engine_used = "None"

        # 1. NLP Extraction Phase (If text is provided)
        if request.problem_text:
            if check_internet_connection():
                engine_used = "Gemini (Cloud)"
                extracted_vars = extract_variables_gemini(request.problem_text)
            else:
                engine_used = "SpaCy (Local)"
                extracted_vars = extract_variables_local(request.problem_text)

            # Merge extracted variables into the known_variables dict
            request.known_variables.update(extracted_vars)

        if not request.known_variables:
            raise ValueError("No variables provided or extracted from the text.")

        # 2. Physics Solving Phase (Always runs locally)
        solved_state = solver_engine.solve(request.known_variables)

        # 3. Geometry Generation Phase
        triangle_coords = coord_engine.generate(solved_state)

        return SolverResponse(
            status="success",
            nlp_engine_used=engine_used,
            results=solved_state,
            coordinates=triangle_coords,
            message="Equation graph resolved successfully."
        )

    except Exception as e:
        return SolverResponse(
            status="error",
            nlp_engine_used="Error",
            results={},
            coordinates={},
            message=str(e)
        )

# =====================================================================
# 6. Standalone Runner
# =====================================================================
if __name__ == "__main__":
    print("Starting local Turbomachinery backend on 127.0.0.1:8080...")
    uvicorn.run(app, host="127.0.0.1", port=8080, log_level="info")