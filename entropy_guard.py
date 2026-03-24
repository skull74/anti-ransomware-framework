import os
import time
import math
import psutil
import threading
from collections import deque, Counter
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

WATCH_DIR = os.path.join(os.path.expanduser('~'), 'Documents')
ENTROPY_THRESHOLD = 7.5
TRUSTED_WHITELIST = ["msmpeng.exe", "searchindexer.exe", "explorer.exe", "system", "svchost.exe", "pycharm64.exe", "python.exe"]
MALICIOUS_COMMANDS = ["vssadmin delete shadows", "wmic shadowcopy delete", "bcdedit /set {default} recoveryenabled no"]

def process_watcher_thread():
    while True:
        for proc in psutil.process_iter(['pid', 'name', 'cmdline']):
            try:
                proc_name = proc.info['name'].lower() if proc.info['name'] else ""
                if proc_name in TRUSTED_WHITELIST: continue
                cmdline = proc.info['cmdline']
                if cmdline:
                    cmd_str = " ".join(cmdline).lower()
                    for bad_cmd in MALICIOUS_COMMANDS:
                        if bad_cmd in cmd_str:
                            print(f"\n[!!!] PRE-ENCRYPTION SETUP DETECTED [!!!]")
                            proc.kill()
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        time.sleep(1)
file_events = deque(maxlen=50)
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

 
# ==========================================
class FrameworkFileSystemHandler(FileSystemEventHandler):
def execute_kill_switch(attacked_file_path):
    print("\n[>>>] INITIATING EDR LINEAGE & HEURISTIC TERMINATION...")
    threat_killed = False
    for proc in psutil.process_iter(['pid', 'name', 'exe']):
        try:
            proc_name = proc.info['name'].lower() if proc.info['name'] else ""
            exe_path = proc.info.get('exe')
            if not exe_path: continue
            exe_path_lower = exe_path.lower()

            if "c:\\users\\" in exe_path_lower and "python" not in exe_path_lower and "pycharm" not in exe_path_lower:
                try:
                    children = proc.children(recursive=True)
                    for child in children: child.kill()
                except: pass
                proc.kill()
                threat_killed = True
                continue
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
    if threat_killed: print("[>>>] EDR SWEEP COMPLETE. All Anomalies Purged.\n")
    def on_modified(self, event):
        if event.is_directory: return
        filepath = event.src_path
        score = calculate_entropy(filepath)
        if score > ENTROPY_THRESHOLD:
            print(f"\n[!!!] HIGH ENTROPY ALARM (Score: {score:.2f}/8.0) [!!!]")
            if not is_header_valid(filepath):
                execute_kill_switch(filepath)
                return
        current_time = time.time()
        file_events.append((filepath, current_time))
        recent_unique_files = set(path for path, t in file_events if current_time - t <= 1.0)
        unique_file_count = len(recent_unique_files)
        if unique_file_count > 4:
            print(f"\n[!!!] RAPID FILE TRAVERSAL DETECTED ({unique_file_count} unique files/sec) [!!!]")
            execute_kill_switch(filepath)
            file_events.clear()

if __name__ == "__main__":
    if not os.path.exists(WATCH_DIR): os.makedirs(WATCH_DIR)
    watcher = threading.Thread(target=process_watcher_thread, daemon=True)
    watcher.start()
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
