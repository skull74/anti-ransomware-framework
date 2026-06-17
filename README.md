**Entropy-Guard: Automated Anti-Ransomware Framework**

Entropy-Guard is an advanced, multi-layered Endpoint Detection and Response (EDR) framework that neutralizes zero-day ransomware using continuous File System Entropy Analysis and Behavioral Velocity Tracking.

Designed to overcome the fundamental failures of traditional signature-based antivirus solutions, this framework identifies active cryptographic payloads in real-time by analyzing the mathematical transformations of file systems.

**Academic Recognition:** The methodology and empirical findings of this framework have been accepted for publication at the 21st International Conference on Security and Cryptography (SECRYPT 2026).

**Project Highlights**

This project demonstrates deep systems-level engineering, applied cryptography, and high-performance multithreading. It evolved from a single-threaded Python prototype to a highly optimized, native C++ production engine.

- **Systems Programming (C++ & Win32 API):** Leveraged low-level Windows APIs (ReadDirectoryChangesW, Toolhelp32Snapshot, WinVerifyTrust) for zero-latency I/O monitoring and process manipulation.
- **High-Performance Multithreading:** Engineered a custom Win32 ThreadPool using CRITICAL_SECTION and CONDITION_VARIABLE to process file events asynchronously, bypassing the Python Global Interpreter Lock (GIL) bottleneck that originally added 300-500ms of latency.
- **Applied Mathematics:** Implemented \$O(1)\$ constant-time Shannon Entropy algorithms using 8KB micro-chunking to instantly identify IND-CPA secure ciphertext.
- **Measurable Impact:** Empirically proven to achieve 97.3% detection accuracy against zero-day variants with a 1.8% false-positive rate across a 1,214-file testbed.

**Core Architecture & Defensive Mechanisms**

The framework utilizes a dual-layer fault-tolerant design to detect and neutralize threats before significant data loss occurs:

**Layer 1: Behavioral Velocity Watcher**

- Ransomware operates at machine speed; humans operate at human speed.
- Maintains a sliding time window queue of file modification events.
- If a process modifies more than 5 unique files per second, the "Dragnet" Kill Switch is immediately triggered.

**Layer 2: Mathematical Entropy Engine**

- Modern symmetric ciphers output ciphertext that is mathematically indistinguishable from random noise.
- The framework calculates the Shannon Entropy of file headers (first 8,192 bytes):

\$\$H=-\\sum\_{b=0}^{255}p(b)\\log_2p(b)\$\$

- If the entropy score exceeds the strict 7.5 threshold (indicating maximum randomness) and the file's magic-byte header is destroyed, the payload is flagged as malicious.

**Advanced Evasion Defeats**

Modern ransomware employs complex evasion tactics, which Entropy-Guard actively counteracts:

- **"Safe House" Evasion (Spoofing System Files):** Defeated via Cryptographic Gatekeeper. The framework uses the WinVerifyTrust and Crypt32 APIs to validate embedded Authenticode and Catalog signatures, instantly killing malicious processes hiding in C:\\Windows\\.
- **"Hydra-Style" Multi-Processing:** Defeated via Recursive Lineage Tracking. Ransomware that spawns dozens of child processes is mapped via CreateToolhelp32Snapshot and the entire process tree is terminated simultaneously.
- **Handle-Dropping:** Defeated via Zero-Trust Heuristics. Identifies and terminates offending processes even if they open, encrypt, and close a file in microseconds.

**Empirical Performance Benchmarks**

The C++ production engine was stress-tested against active, real-world ransomware families (including LockBit 3.0, Conti, Ryuk, and WannaCry) in an air-gapped laboratory environment.

| **Metric**                        | **Result**      | **Industry Context**                                                  |
| --------------------------------- | --------------- | --------------------------------------------------------------------- |
| **Detection Accuracy**            | 97.3%           | Outperforms traditional signature-based AV on zero-day payloads.      |
| **Mean Time-to-Neutralize (TTN)** | 3.72 seconds    | 5.4x faster than academic baselines like CryptoDrop.                  |
| **Data Preservation Rate (DPR)**  | 97.8% (Average) | Restricted blast radius to 42 out of 1,214 files against LockBit 3.0. |
| **CPU Overhead**                  | ~1.2% (Idle)    | Lightweight user-space daemon.                                        |

**Installation & Deployment**

**Prerequisites**

- Windows 10/11 Operating System (64-bit).
- MSVC Compiler (Visual Studio 2022) or MinGW.
- Required Windows Libraries: wintrust.lib, crypt32.lib, psapi.lib.

**Compilation Instructions**

To build the production C++ engine (new_guard.cpp):

DOS

cl.exe /EHsc /std:c++17 /O2 /MT /Fe:new_guard.exe new_guard.cpp /link wintrust.lib crypt32.lib psapi.lib

**Execution**

- Must be run as Administrator to acquire SeDebugPrivilege for system-level process termination and directory hooking.
- The framework will automatically map the number of logical processors and spin up the optimized ThreadPool to secure the target directories.

**Future Scope**

While the current implementation operates in Ring-3 (User-Space) with exceptional performance, future development aims to migrate the core logic to a Ring-0 Kernel Mini-Filter Driver. This architectural pivot will reduce detection latency to less than 1 microsecond (pre-operation callbacks) and provide absolute immunity against Bring Your Own Vulnerable Driver (BYOVD) exploitation.

_Developed by Aditya Singh as part of the Master of Technology (Computer Science and Engineering) program at UPES, Dehradun._