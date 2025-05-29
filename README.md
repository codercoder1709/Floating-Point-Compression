# 🚀 Floating Point Data Compressor using FPC + rANS

This project implements a high-performance compression algorithm tailored for floating-point sensor data by combining **Fast Predictive Compression (FPC)** with **range Asymmetric Numeral Systems (rANS)**. It's designed for lossless and efficient streaming compression, especially in IoT and telemetry scenarios.

---

## ✨ Features

- 🔍 Dual Predictors: Finite Context Method (FCM) and Differential FCM (DFCM)
- 🧠 Adaptive: Picks the better predictor (FCM or DFCM) for each value
- 🧩 Zero-byte XOR encoding: Encodes only non-zero bytes of the residual
- 📉 rANS compression: Uses range Asymmetric Numeral Systems for entropy coding
- ⚡ Fast & efficient: Suitable for real-time, high-throughput streaming

---

## 📁 Project Structure
```plaintext
├── app
│   ├── CMakeLists.txt
│   └── src
│       └── main.cpp
├── dataset
│   ├── largeVolume
│   │   ├── city_temperature.csv
│   │   └── dataexport_20250126T094958.csv
│   └── readMe.txt
├── doc
│   └── readMe.txt
├── lib
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── dataProcessing.hpp
│   │   └── fileReader.hpp
│   └── src
│       ├── dataProcessing.cpp
│       └── fileReader.cpp
├── README.md
└── run.sh
```
## ▶️ Running the Project

To compile and run the program, simply execute the provided `run.sh` script:

```bash
chmod +x run.sh
./run.sh
```

# How It Works

### 1. **FPC Encoding**

- Predicts the next value using both FCM and DFCM.
- Computes XOR of prediction and actual value.
- Chooses the predictor with fewer leading zero bytes.
- Encodes only the non-zero bytes plus which predictor was used.

### 2. **rANS Compression**

- Feeds encoded residuals (as symbols) to rANS.
- rANS compresses the stream into compact binary form.
- Achieves near-entropy compression with high speed.

## Features

* ⚡ Excellent compression ratio on sensor data  
* 🔁 Fast enough for real-time streaming use  
* 💾 Lossless: Decompressed data is bit-identical to input


