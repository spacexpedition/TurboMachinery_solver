from openai import OpenAI

client = OpenAI(
    base_url="https://generativelanguage.googleapis.com/v1beta/openai/",
    api_key="AIzaSyBez1uTSC3t6LP7Di5fn1djJIhT-kc6QsQ"
)

resp = client.chat.completions.create(
    model="gemini-2.0-flash",
    messages=[{"role": "user", "content": "Hello"}]
)

print(resp.choices[0].message.content)