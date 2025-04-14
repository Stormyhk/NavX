from inference_sdk import InferenceHTTPClient
from dotenv import load_dotenv
import os

load_dotenv()
api_key = os.getenv("ROBOFLOW_API_KEY")

client = InferenceHTTPClient(
    api_url="https://detect.roboflow.com",
    api_key=api_key
)

result = client.run_workflow(
    workspace_name="navx",
    workflow_id="detect-and-classify",
    images={
        "image": "banana.webp"
    },
    use_cache=True 
)

preds = result[0]["predictions"]["predictions"]

fruit_names = [p["class"] for p in preds]
print("Fruits detected:", fruit_names)
