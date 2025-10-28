from ultralytics import YOLO
import cv2
import urllib.request
import numpy as np

# Load YOLOv5s model
model = YOLO("yolov5s.pt")

# MJPEG stream URL
url = 'http://192.168.57.206:7123/stream.mjpg'
stream = urllib.request.urlopen(url)
bytes_data = b''

while True:
    try:
        # Read chunk of bytes from stream
        bytes_data += stream.read(1024)
        a = bytes_data.find(b'\xff\xd8')  # JPEG start
        b = bytes_data.find(b'\xff\xd9')  # JPEG end

        if a != -1 and b != -1:
            jpg = bytes_data[a:b+2]
            bytes_data = bytes_data[b+2:]

            # Decode JPEG to OpenCV BGR image
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)

            if frame is None:
                print("Warning: Decoded frame is None")
                continue

            # Pass frame to YOLO
            results = model(frame)

            # Draw detections
            for result in results:
                for box, cls in zip(result.boxes.xyxy, result.boxes.cls):
                    class_id = int(cls)
                    if model.names[class_id] == "person":
                        x1, y1, x2, y2 = map(int, box)
                        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                        cv2.putText(frame, "Person", (x1, y1 - 10),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

            # Show frame
            cv2.imshow("MJPEG YOLO Person Detection", frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except Exception as e:
        print("Error:", e)
        break

cv2.destroyAllWindows()
