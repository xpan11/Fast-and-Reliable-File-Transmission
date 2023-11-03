# Fast and Reliable File Transmission (FRFT)

FRFT is a suite of tools designed to optimize the transmission of files across networks with varying conditions. Through custom FTP protocol handling and file segmentation, FRFT strives to maximize throughput in high-latency and packet-loss-prone environments. The project encompasses different scripts tailored for distinct file size ranges, ensuring a smooth and efficient transmission process.

## Features

- Custom FTP protocol handling for optimized file transmission.
- File segmentation to parallelize the transfer process.
- Separate scripts for handling different file size ranges.
- Scripts to clean network channels before and after transmission.
- Transmission time measurement for performance benchmarking.



### Run the application

Sender side:
1.clone the sender folder on the sender side:
--this is for file that size less than 800MB(version 1):
a. bash auto_send_small.sh <filename> <receiver_ip>

--1GB file transmission(imrpoved version 2):
b. bash auto_send_0.sh <filename> <receiver_ip>


Receiver side:
1.clone the receiver folder on the sender side:
--this is for file that size less than 800MB(version 1):
a. bash auto_rec_small.sh <filename> <receiver_ip>

--1GB file transmission(imrpoved version 2):
b. bash auto_rec_0.sh <filename> <receiver_ip>


## Performance
Throughput for files below 800MB: More than 45 Mbps over 100 Mbps link with 200ms delay and 20% packet loss.
Throughput for Improved version(1GB): More than 47 Mbps under 100 Mbps limit with 200ms delay and 20% packet loss.

