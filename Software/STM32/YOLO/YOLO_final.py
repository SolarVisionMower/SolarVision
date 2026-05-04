import cv2
import numpy as np
import serial
from ultralytics import YOLO
import sys

if len(sys.argv) != 2 or sys.argv[1] == "":
    print("Please provide video capture device")
    sys.exit(1)

# Load YOLOv11 segmentation model
model = YOLO("yolov11-seg-best.pt")

# Serial connection to ESP32
# ser = serial.Serial('COM3', 115200)

# Open camera
cap = cv2.VideoCapture(sys.argv[1])

# --- VIDEO WRITER SETUP ---
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = int(cap.get(cv2.CAP_PROP_FPS)) or 30

# Changed codec to 'mp4v' for MP4 containers
fourcc = cv2.VideoWriter_fourcc(*'mp4v') 
out = cv2.VideoWriter('segmented_output.mp4', fourcc, fps, (frame_width, frame_height))

# Threshold (tune this)
GRASS_THRESHOLD = 0.6

print("Processing... Press Ctrl+C or ESC to stop.")

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Run inference
        results = model(frame)
        
        # plot() returns the BGR frame with segmentation masks drawn
        annotated_frame = results[0].plot()

        if results[0].masks is not None:
            # Logic for bounds checking
            mask = results[0].masks.data[0].cpu().numpy()
            mask = cv2.resize(mask, (frame_width, frame_height))

            h, w = mask.shape
            roi = mask[int(h*0.7):h, int(w*0.3):int(w*0.7)]
            grass_ratio = np.mean(roi)

            if grass_ratio < GRASS_THRESHOLD:
                status_text = "OUT OF BOUNDS"
                color = (0, 0, 255) # Red
                # ser.write(b'OUT_OF_BOUNDS\n')
            else:
                status_text = "IN BOUNDS"
                color = (0, 255, 0) # Green
                # ser.write(b'IN_BOUNDS\n')

            # Overlays
            cv2.putText(annotated_frame, status_text, (50, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
            cv2.rectangle(annotated_frame, (int(w*0.3), int(h*0.7)), 
                          (int(w*0.7), h), color, 2)

        # Write the segmented frame to the mp4 file
        out.write(annotated_frame)
        # cv2.imshow("YOLOv11 Segmentation", annotated_frame)

        if cv2.waitKey(1) == 27: # ESC key
            break

except KeyboardInterrupt:
    print("\nInterrupted by user. Closing gracefully...")

finally:
    # This block runs regardless of how the script ends
    print("Releasing resources...")
    cap.release()
    out.release()
    cv2.destroyAllWindows()
    print("Done.")
