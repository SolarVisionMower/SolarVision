import cv2
import numpy as np
import serial
from ultralytics import YOLO

# ---------------- SETTINGS ----------------
MODEL_PATH = "yolov11-seg-best.pt"
VIDEO_SOURCE = 0          # change to video path if needed
# SERIAL_PORT = "COM3"      # Windows: COM3, Mac/Linux: /dev/ttyUSB0 or /dev/cu.usbserial
BAUD_RATE = 115200

GRASS_THRESHOLD = 0.45    # lower = less strict, higher = more strict
CONF_THRESHOLD = 0.50

LEFT_BIT  = 0x04
AHEAD_BIT = 0x02
RIGHT_BIT = 0x01

# ---------------- SETUP ----------------
model = YOLO(MODEL_PATH)

# ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

VIDEO_SOURCE = input("video source: ")

cap = cv2.VideoCapture(VIDEO_SOURCE)

frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = int(cap.get(cv2.CAP_PROP_FPS)) or 30

fourcc = cv2.VideoWriter_fourcc(*'mp4v') 
out = cv2.VideoWriter('segmented_output.mp4', fourcc, fps, (frame_width, frame_height))

def get_combined_grass_mask(result, frame_shape):
    """
    Combines all YOLO segmentation masks into one grass mask.
    White/1 = grass
    Black/0 = not grass
    """
    h, w = frame_shape[:2]

    if result.masks is None:
        return np.zeros((h, w), dtype=np.uint8)

    combined = np.zeros((h, w), dtype=np.uint8)

    for mask in result.masks.data:
        mask = mask.cpu().numpy()
        mask = cv2.resize(mask, (w, h))
        mask = (mask > 0.5).astype(np.uint8)
        combined = np.maximum(combined, mask)

    return combined


def check_box(mask, x1, y1, x2, y2):
    """
    Returns True if this box is OUT OF BOUNDS.
    """
    roi = mask[y1:y2, x1:x2]
    grass_ratio = np.mean(roi)

    return grass_ratio < GRASS_THRESHOLD, grass_ratio


def draw_box(frame, box, is_out, label, ratio):
    x1, y1, x2, y2 = box

    color = (0, 0, 255) if is_out else (0, 255, 0)

    cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
    cv2.putText(
        frame,
        f"{label}: {ratio:.2f}",
        (x1, y1 - 10),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.6,
        color,
        2
    )


while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame, conf=CONF_THRESHOLD)
    result = results[0]

    grass_mask = get_combined_grass_mask(result, frame.shape)

    h, w = frame.shape[:2]

    # ---------------- 3 BOX LOCATIONS ----------------
    # These are near the bottom because that is where the mower is about to drive.
    left_box = (
        int(w * 0.12),
        int(h * 0.65),
        int(w * 0.32),
        int(h * 0.92)
    )

    ahead_box = (
        int(w * 0.40),
        int(h * 0.58),
        int(w * 0.60),
        int(h * 0.85)
    )

    right_box = (
        int(w * 0.68),
        int(h * 0.65),
        int(w * 0.88),
        int(h * 0.92)
    )

    # ---------------- CHECK EACH BOX ----------------
    left_out, left_ratio = check_box(grass_mask, *left_box)
    ahead_out, ahead_ratio = check_box(grass_mask, *ahead_box)
    right_out, right_ratio = check_box(grass_mask, *right_box)

    boundary_code = 0x00

    if left_out:
        boundary_code |= LEFT_BIT

    if ahead_out:
        boundary_code |= AHEAD_BIT

    if right_out:
        boundary_code |= RIGHT_BIT

    # ---------------- SEND TO ESP32 ----------------
    message = f"BOUNDARY:0x{boundary_code:02X}\n"
    # ser.write(message.encode())

    print(message.strip())

    # ---------------- VISUAL DEBUG ----------------
    draw_box(frame, left_box, left_out, "LEFT", left_ratio)
    draw_box(frame, ahead_box, ahead_out, "AHEAD", ahead_ratio)
    draw_box(frame, right_box, right_out, "RIGHT", right_ratio)

    if boundary_code != 0:
        cv2.putText(
            frame,
            f"OUT OF BOUNDS: 0x{boundary_code:02X}",
            (30, 50),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 0, 255),
            3
        )
    else:
        cv2.putText(
            frame,
            "IN BOUNDS",
            (30, 50),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 255, 0),
            3
        )

    # cv2.imshow("YOLO Boundary Detection", result.plot())
    out.write(result.plot())

    if cv2.waitKey(1) == 27:
        break

cap.release()
# ser.close()
out.release()
cv2.destroyAllWindows()
