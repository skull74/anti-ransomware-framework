import os
import time
import math
from collections import Counter
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

WATCH_DIR = os.path.join(os.path.expanduser('~'), 'Documents')
ENTROPY_THRESHOLD = 7.5
def calculate_entropy(file_path):
    """Ultra-fast calculation with File-Lock bypass."""
    for attempt in range(3):
        try:
            with open(file_path, 'rb') as f:
                data = f.read(8192)
                if not data: return 0.0
                entropy = 0
                length = len(data)
                counts = Counter(data)
                for count in counts.values():
                    p_x = count / length
                    entropy += - p_x * math.log2(p_x)
                return entropy
        except PermissionError:
            time.sleep(0.01)
        except Exception:
            return 0.0
    return 0.0

def is_header_valid(file_path):
    signatures = {
        '.jpg': b'\xFF\xD8\xFF', '.png': b'\x89\x50\x4E\x47', '.pdf': b'\x25\x50\x44\x46',
        '.zip': b'\x50\x4B\x03\x04', '.docx': b'\x50\x4B\x03\x04', '.xlsx': b'\x50\x4B\x03\x04', '.pptx': b'\x50\x4B\x03\x04'
    }
    ext = os.path.splitext(file_path)[1].lower()
    if ext not in signatures: return False
    try:
        with open(file_path, 'rb') as f:
            file_header = f.read(4)
            if file_header.startswith(signatures[ext][:len(file_header)]): return True
    except Exception:
        pass
    return False

            entropy = 0
            length = len(data)
            counts = Counter(data)
            for count in counts.values():
                p_x = count / length
                entropy += - p_x * math.log2(p_x)
            return entropy
    except Exception:
        return 0.0

class FrameworkFileSystemHandler(FileSystemEventHandler):
    def on_modified(self, event):
        if event.is_directory: return
        filepath = event.src_path
        score = calculate_entropy(filepath)
        if score > ENTROPY_THRESHOLD:
            print(f"\n[!!!] HIGH ENTROPY ALARM (Score: {score:.2f}/8.0) [!!!]")

if __name__ == "__main__":
    if not os.path.exists(WATCH_DIR): os.makedirs(WATCH_DIR)
    event_handler = FrameworkFileSystemHandler()
    observer = Observer()
    observer.schedule(event_handler, WATCH_DIR, recursive=True)
    observer.start()
    print(f"[*] Securing: {WATCH_DIR}")
    try:
        while True: time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
