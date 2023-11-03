# Fast and Reliable File Transmission (FRFT)

_A suite of tools optimizing file transmission across various network conditions._

FRFT is engineered to maximize throughput in high-latency and packet-loss-prone environments through custom FTP protocol handling and file segmentation. Different scripts are tailored for distinct file size ranges, ensuring a seamless and efficient transmission process.

## Features

- **Custom FTP Protocol Handling**: Optimizes file transmission over FTP.
- **File Segmentation**: Parallelizes the transfer process to speed up transmission.
- **Diverse Scripts**: Tailored scripts for handling different file size ranges.
- **Network Channel Cleaning**: Scripts to clean network channels before and after transmission.
- **Transmission Time Measurement**: Performance benchmarking through transmission time measurement.

## Test Setting

- **Environment**: AWS EC2 instances for sender and receiver, and a Vyos router.
- **Instances**:
  - Ubuntu Sender and Receiver: `amazon/ubuntu/images/hvm-ssd/ubuntu-xenial-16.04-amd64-server-20210928`
  - Vyos Router: `507040250710/VyOS (HVM) 1.3-rolling-202208231231`

## Different Version

- Prototype version(auto_send_small.sh and auto_rec_small.sh):
  - Support for file size smaller than 800MB with great performance
- Formal (Improved) version(auto_send_0.sh and auto_rec_0.sh):
  - Support 1GB file transmission with optimal performance
 
## Performance 
(over a **100 Mbps link** with **200ms delay** and **20% packet loss**.)  

Throughput (Files < 800MB):  more than 45 Mbps     
**Throughput (1GB - Improved Version)**: more than **47 Mbps**   
**which is more than 1000 faster than SCP!!!**


## Run the Application

### Sender Side:
Clone the sender folder on the sender side then:
```bash
cd sender
#For files less than 800MB(prototype version):
./auto_send_small.sh <filename> <receiver_ip>
#For 1GB file transmission (Improved Version 2):
./auto_send_0.sh <filename> <receiver_ip>
```

### Receiver Side:
Clone the receiver folder on the receiver side then:
```bash
cd receiver
#For files less than 800MB(prototype version):
./auto_rec_small.sh <filename> <receiver_ip>
#For 1GB file transmission (Improved Version 2):
./auto_rec_0.sh <filename> <receiver_ip>
```



