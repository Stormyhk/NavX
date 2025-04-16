from ultralytics import YOLO
import cv2

model = YOLO("yolov8n.pt")

stream_url = "http://192.168.4.1:81/stream"

cap = cv2.VideoCapture(stream_url)

if not cap.isOpened():
    print("NO CONNECTION")
    exit()

print("Connected Sucessfully\n")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Unsucessful")
        break

    #puts the frame into the model
    results = model(frame, verbose=False)

    #live annotation
    annotated = results[0].plot()
    cv2.imshow("ESP32 Vision 🧠", annotated)

    #labels
    classes = results[0].names
    detected = results[0].boxes.cls.cpu().numpy().astype(int)

    labels = [classes[i] for i in detected]
    unique_labels = list(set(labels))

    #print to the terminal
    if unique_labels:
        print("Object's Detected:", ", ".join(unique_labels))
    else:
        print("Nothing Detected")

    #just to stop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
