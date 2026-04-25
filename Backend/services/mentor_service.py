import os
import json
import urllib.request
import urllib.error
from typing import List, Dict

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY", "YOUR_API_KEY_HERE")

# In-memory session store for MVP
# Format: { "user_email": [ { "role": "user"|"model", "parts": [{"text": "..."}] } ] }
_chat_sessions: Dict[str, List[dict]] = {}

SOCRATIC_PROMPT = """You are an expert Turbomachinery Socratic Mentor.
Your goal is to teach the user how to solve turbomachinery problems (compressors, turbines, velocity triangles) by guiding them, NOT by giving them the answer immediately.

Follow these rules:
1. When a user presents a problem, do not solve it for them.
2. Break the problem down. Ask them what the first step should be.
3. Provide formula options. Example: "To find the meridional velocity (Cm1), which formula should we use? A) Cm1 = V / A, B) Cm1 = U * tan(alpha), C) Cm1 = W * sin(beta)"
4. Wait for their answer. If they are correct, praise them and ask them to calculate the value.
5. If they are wrong, provide a gentle hint.
6. Periodically give quick review questions to ensure they understand the concepts behind the math (e.g., "Why does the fluid enter axially in this compressor?").
7. Use Markdown and LaTeX (e.g., $C_{m1}$) for formatting.
"""

class MentorService:
    @staticmethod
    def _get_or_create_history(email: str) -> List[dict]:
        if email not in _chat_sessions:
            # Initialize with system prompt (Gemini API handles system instructions separately or as first message)
            # For Gemini REST API, system instructions are passed in a specific field, but for simplicity we can
            # just inject it as context if needed, or pass it in systemInstruction.
            _chat_sessions[email] = []
        return _chat_sessions[email]

    @staticmethod
    def send_message(email: str, message: str) -> str:
        history = MentorService._get_or_create_history(email)
        
        # Append user message
        history.append({"role": "user", "parts": [{"text": message}]})

        if GEMINI_API_KEY == "YOUR_API_KEY_HERE":
            reply = "Warning: No Gemini API Key set. I am a mock Socratic Mentor. What formula should we use for U? (A) $U=\\omega r$ (B) $U=C \\cos(\\alpha)$"
            history.append({"role": "model", "parts": [{"text": reply}]})
            return reply

        url = f"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key={GEMINI_API_KEY}"
        
        payload = {
            "systemInstruction": {
                "parts": [{"text": SOCRATIC_PROMPT}]
            },
            "contents": history,
            "generationConfig": {
                "temperature": 0.3
            }
        }

        try:
            req = urllib.request.Request(url, data=json.dumps(payload).encode('utf-8'), headers={'Content-Type': 'application/json'})
            with urllib.request.urlopen(req) as response:
                data = json.loads(response.read())
                reply_text = data['candidates'][0]['content']['parts'][0]['text']
                
                # Append model reply
                history.append({"role": "model", "parts": [{"text": reply_text}]})
                return reply_text
        except Exception as e:
            # Pop the user message if it failed so we don't corrupt history
            history.pop()
            return f"Error communicating with Mentor AI: {str(e)}"

    @staticmethod
    def clear_session(email: str):
        if email in _chat_sessions:
            del _chat_sessions[email]
