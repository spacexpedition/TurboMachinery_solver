from openai import OpenAI

client = OpenAI(
    base_url="https://generativelanguage.googleapis.com/v1beta/openai/",
    api_key="AIzaSyBsuNgM2KkUySi4ZFVUi5gXvCLNdj5pvcs"
)

resp = client.chat.completions.create(
    model="gemini-2.0-flash",
    messages=[{"role": "user", "content": "Hello"}]
)

print(resp.choices[0].message.content)