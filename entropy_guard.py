import os
import time

WATCH_DIR = os.path.join(os.path.expanduser('~'), 'Documents')

if __name__ == "__main__":
    print("==================================================")
    print("   AUTOMATED ANTI-RANSOMWARE FRAMEWORK BOOTING    ")
    print("==================================================")
    
    if not os.path.exists(WATCH_DIR):
        os.makedirs(WATCH_DIR)
        
    print(f"[*] Securing: {WATCH_DIR}")
