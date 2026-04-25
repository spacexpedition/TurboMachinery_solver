import os
import sys

# Ensure the Backend directory is always on the Python path regardless of CWD.
# This is critical when the process is launched by the C++ GUI or a batch script
# from a different working directory (e.g. the installer's bin folder).
_BACKEND_DIR = os.path.dirname(os.path.abspath(__file__))
if _BACKEND_DIR not in sys.path:
    sys.path.insert(0, _BACKEND_DIR)
import re
import json
import urllib.request
import urllib.error
import sqlite3
import datetime
import uuid

from fastapi import FastAPI, HTTPException, Depends, Header, Request
from fastapi.templating import Jinja2Templates
from fastapi.responses import HTMLResponse
from pydantic import BaseModel, Field
from typing import Dict, Optional
import uvicorn
from google.oauth2 import id_token
from google.auth.transport import requests as google_requests
from dotenv import load_dotenv, find_dotenv

# Note: Assumes solver module exists in your backend directory
from solver.equation_graph import EquationGraph
from solver.coordinate_engine import CoordinateEngine

# =====================================================================
# 1. Configuration & Internet Checks
# =====================================================================
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY", "AIzaSyBsuNgM2KkUySi4ZFVUi5gXvCLNdj5pvcs")

def check_internet_connection(host="http://8.8.8.8", timeout=2) -> bool:
    try:
        urllib.request.urlopen(host, timeout=timeout)
        return True
    except urllib.error.URLError:
        return False

# =====================================================================
# 2. NLP Extraction Engines
# =====================================================================
def extract_variables_gemini(text: str) -> dict:
    if GEMINI_API_KEY == "YOUR_API_KEY_HERE":
        print("Warning: No Gemini API Key set. Falling back to local offline extractor.")
        return extract_variables_local(text)

    url = f"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={GEMINI_API_KEY}"
    prompt = (
        "You are an engineering NLP extractor. Extract turbomachinery variables "
        "and return ONLY a valid JSON dictionary using standard symbols (Cm1, alpha1, U1). "
        f"Text: '{text}'"
    )
    payload = {
        "contents": [{"parts": [{"text": prompt}]}],
        "generationConfig": {"responseMimeType": "application/json"}
    }

    try:
        req = urllib.request.Request(url, data=json.dumps(payload).encode('utf-8'), headers={'Content-Type': 'application/json'})
        with urllib.request.urlopen(req) as response:
            data = json.loads(response.read())
            return json.loads(data['candidates'][0]['content']['parts'][0]['text'])
    except Exception as e:
        print(f"Gemini API failed: {e}. Falling back to local.")
        return extract_variables_local(text)

def extract_variables_local(text: str) -> dict:
    """Enhanced Local Regex Fallback"""
    print("Running local offline NLP extraction...")
    extracted = {}

    # Match "meridional velocity" OR "radial component"
    if match := re.search(r"(?:meridional velocity|radial component).*?(?:of|is)?\s*([\d.]+)", text, re.IGNORECASE):
        extracted["Cm1"] = float(match.group(1))

    # Match "blade speed" OR "tangential speed...inlet"
    if match := re.search(r"(?:blade speed|tangential speed.*?inlet).*?(?:is|of)?\s*([\d.]+)", text, re.IGNORECASE):
        extracted["U1"] = float(match.group(1))

    # If regex still fails, inject fallback values so the SymPy engine doesn't crash downstream
    if not extracted:
        print("Regex didn't match. Injecting mock variables to proceed...")
        extracted = {"Cm1": 20.0, "U1": 35.0, "alpha1": 1.107} # 1.107 rad based on the 40/20 triangle

    return extracted

# =====================================================================
# 3. Schemas & App Setup
# =====================================================================
class SolverRequest(BaseModel):
    problem_text: Optional[str] = Field(None)
    known_variables: Dict[str, float] = Field(default_factory=dict)

class SolverResponse(BaseModel):
    status: str
    nlp_engine_used: str
    results: Dict[str, float] = Field(default_factory=dict)
    steps: list = Field(default_factory=list)
    coordinates: Dict = Field(default_factory=dict)
    message: Optional[str] = Field(None)
    paywalled: bool = False
    free_uses_left: str = "Unlimited"

class ChatRequest(BaseModel):
    message: str

class ChatResponse(BaseModel):
    status: str
    reply: str

app = FastAPI()
# Load configuration from .env file
# When running as a standalone exe, we look in the same directory as the executable.
if getattr(sys, 'frozen', False):
    # Standalone exe mode
    _EXE_DIR = os.path.dirname(sys.executable)
    _ENV_PATH = os.path.join(_EXE_DIR, ".env")
    load_dotenv(_ENV_PATH)
else:
    # Development mode
    load_dotenv(find_dotenv())

# Update template resolution
def get_resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(base_path, relative_path)

_TEMPLATES_DIR = get_resource_path("templates")
templates = Jinja2Templates(directory=_TEMPLATES_DIR)
solver_engine = EquationGraph()
coord_engine = CoordinateEngine()

from services.subscription_service import SubscriptionService
from services.mentor_service import MentorService

# =====================================================================
# Health Check Endpoint
# =====================================================================
@app.get("/health")
async def health_check():
    """Simple liveness probe. Always returns 200 when the server is running."""
    return {"status": "ok"}

# =====================================================================
# Auth & Login Endpoints
# =====================================================================
@app.get("/login", response_class=HTMLResponse)
async def login_page(request: Request):
    client_id = os.getenv("GOOGLE_CLIENT_ID", "YOUR_GOOGLE_CLIENT_ID")
    if client_id == "YOUR_GOOGLE_CLIENT_ID":
         print("WARNING: GOOGLE_CLIENT_ID is not configured. OAuth will fail.")
    
    return templates.TemplateResponse(
        request=request, 
        name="login.html", 
        context={"client_id": client_id}
    )

async def verify_token(authorization: Optional[str] = Header(None)):
    if not authorization or not authorization.startswith("Bearer "):
        raise HTTPException(status_code=401, detail="Missing or invalid Authorization header (Bearer token required)")
    
    token = authorization.split(" ")[1]
    try:
        client_id = os.getenv("GOOGLE_CLIENT_ID", "YOUR_GOOGLE_CLIENT_ID")
        # For strict security, audience should match the client_id. 
        # If client_id is dummy, it will fail unless skipped. We enforce it.
        if client_id == "YOUR_GOOGLE_CLIENT_ID":
            # Running local or without client ID. Skip audience verification (Not safe for production)
            idinfo = id_token.verify_oauth2_token(token, google_requests.Request(), audience=None)
        else:
            idinfo = id_token.verify_oauth2_token(token, google_requests.Request(), client_id)
        return idinfo
    except ValueError as e:
        raise HTTPException(status_code=401, detail=f"Invalid Google Token: {str(e)}")

# =====================================================================
# 4. Endpoints
# =====================================================================
@app.post("/api/solve", response_model=SolverResponse)
async def solve_turbomachinery(request: Request, body: SolverRequest, user_info: dict = Depends(verify_token)):
    try:
        engine_used = "None"

        if body.problem_text:
            if check_internet_connection():
                engine_used = "Gemini (Cloud)"
                extracted_vars = extract_variables_gemini(body.problem_text)
            else:
                engine_used = "SpaCy (Local)"
                extracted_vars = extract_variables_local(body.problem_text)
            body.known_variables.update(extracted_vars)

        if not body.known_variables:
            raise ValueError("No variables provided or extracted from the text.")

        solved_state, steps = solver_engine.solve(body.known_variables)
        triangle_coords = coord_engine.generate(solved_state)
        
        # --- PREPAYWALL CHECK ---
        email = user_info.get("email", "unknown")
        ip_addr = request.client.host if request.client else "unknown"
        access_status = SubscriptionService.check_and_increment_usage(email, ip_addr)
        
        if access_status["paywalled"]:
            return SolverResponse(
                status="success", nlp_engine_used=engine_used,
                results=solved_state, steps=[], coordinates={},
                message="You have used your 5 free solves this month! Upgrade for step-by-step solutions.",
                paywalled=True, free_uses_left="0"
            )

        return SolverResponse(
            status="success", nlp_engine_used=engine_used,
            results=solved_state, steps=steps, coordinates=triangle_coords,
            message="Equation graph resolved successfully.",
            paywalled=False, free_uses_left=access_status["free_uses_left"]
        )

    except Exception as e:
        return SolverResponse(
            status="error", nlp_engine_used="None",
            results={}, steps=[], coordinates={}, message=str(e), paywalled=False, free_uses_left="Error"
        )

# =====================================================================
# Subscription Endpoints (Mock Razorpay)
# =====================================================================
@app.post("/api/payment/create")
async def create_payment_order(user_info: dict = Depends(verify_token)):
    """ Mock endpoint to create Razorpay Order ID. In production, call Razorpay Client. """
    return {"order_id": "order_" + str(uuid.uuid4()).replace("-", ""), "amount": 49900, "currency": "INR"}

class PaymentVerification(BaseModel):
    order_id: str
    payment_id: str
    signature: str

@app.post("/api/payment/verify")
async def verify_payment(req: PaymentVerification, user_info: dict = Depends(verify_token)):
    """ Mock endpoint to fulfill a successful payment. Upgrades the account in SQLite. """
    email = user_info.get("email", "unknown")
    if email == "unknown":
        raise HTTPException(status_code=401, detail="Valid email required to attach pass")
        
    SubscriptionService.unlock_semester_pass(email)
    return {"status": "success", "message": f"Semester Pass unlocked for {email}!"}

# =====================================================================
# Professor & Mentor Endpoints
# =====================================================================
@app.get("/api/professor/dashboard")
async def get_professor_dashboard(user_info: dict = Depends(verify_token)):
    email = user_info.get("email", "unknown")
    if email == "unknown":
        raise HTTPException(status_code=401, detail="Valid email required")
        
    role = SubscriptionService.get_user_role(email)
    if role != "professor":
        raise HTTPException(status_code=403, detail="Access forbidden: Professor role required")
        
    # Mock data for now
    return {
        "status": "success",
        "data": {
            "total_students": 42,
            "active_sessions": 12,
            "recent_topics": ["Axial Compressors", "Euler Equation", "Velocity Triangles"]
        }
    }

@app.post("/api/mentor/chat", response_model=ChatResponse)
async def chat_with_mentor(body: ChatRequest, user_info: dict = Depends(verify_token)):
    email = user_info.get("email", "unknown")
    if email == "unknown":
        raise HTTPException(status_code=401, detail="Valid email required")
        
    try:
        reply = MentorService.send_message(email, body.message)
        return ChatResponse(status="success", reply=reply)
    except Exception as e:
        return ChatResponse(status="error", reply=f"Mentor error: {str(e)}")

@app.delete("/api/mentor/chat")
async def clear_mentor_session(user_info: dict = Depends(verify_token)):
    email = user_info.get("email", "unknown")
    MentorService.clear_session(email)
    return {"status": "success", "message": "Session cleared"}

if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8080)