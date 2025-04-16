from ultralytics import YOLO
import cv2
import numpy as np
import requests

model = YOLO("yolov8n.pt")

esp32_url = "http://192.168.4.1/capture" 

response = requests.get(esp32_url, stream=True)
if response.status_code == 200:
    img_arr = np.asarray(bytearray(response.content), dtype=np.uint8)
    frame = cv2.imdecode(img_arr, cv2.IMREAD_COLOR)

    results = model(frame)
    results[0].show() 

    classes = results[0].names
    detected = results[0].boxes.cls.cpu().numpy().astype(int)

    labels = list(set([classes[i] for i in detected]))

    if labels:
        print("Object's Seen:", ", ".join(labels))
    else:
        print("Nothing Seen")
else:
    print("Failed to get image from ESP32:", response.status_code)
