import os
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

WATCH_DIR = os.path.join(os.path.expanduser('~'), 'Documents')

class FrameworkFileSystemHandler(FileSystemEventHandler):
    def on_modified(self, event):
        if event.is_directory:
            return
        filepath = event.src_path
        filename = os.path.basename(filepath)
        print(f"File modified: {filename}")

if __name__ == "__main__":
    print("==================================================")
    print("   AUTOMATED ANTI-RANSOMWARE FRAMEWORK BOOTING    ")
    print("==================================================")
    if not os.path.exists(WATCH_DIR):
        os.makedirs(WATCH_DIR)

    event_handler = FrameworkFileSystemHandler()
    observer = Observer()
    observer.schedule(event_handler, WATCH_DIR, recursive=True)
    observer.start()

    print(f"[*] Layer 2 (Entropy-Guard) Online. Securing: {WATCH_DIR}")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[*] Shutting down framework.")
        observer.stop()
    observer.join()
