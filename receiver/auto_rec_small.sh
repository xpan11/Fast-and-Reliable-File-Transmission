#pass the filename

if [ -z "$1" ]; then
    echo "Please provide a filename as an argument."
    exit 1
fi
# pass the ip address of the sender
if [ -z "$2" ]; then
     echo "please give the ip address of the sender."
     exit 3
fi

#clean up network channel
fuser -k 36660/udp
fuser -k 36661/udp
fuser -k 36662/udp
fuser -k 36663/udp
fuser -k 36664/udp
fuser -k 36665/udp
fuser -k 36666/udp
fuser -k 36667/udp
fuser -k 36668/udp
fuser -k 36669/udp

fuser -k 33330/tcp
fuser -k 33331/tcp
fuser -k 33332/tcp
fuser -k 33333/tcp
fuser -k 33334/tcp
fuser -k 33335/tcp
fuser -k 33336/tcp
fuser -k 33337/tcp
fuser -k 33338/tcp
fuser -k 33339/tcp

fuser -k 36670/udp
fuser -k 36671/udp
fuser -k 36672/udp
fuser -k 36673/udp
fuser -k 36674/udp
fuser -k 36675/udp
fuser -k 36676/udp
fuser -k 36677/udp
fuser -k 36678/udp
fuser -k 36679/udp

fuser -k 33340/tcp
fuser -k 33341/tcp
fuser -k 33342/tcp
fuser -k 33343/tcp
fuser -k 33344/tcp
fuser -k 33345/tcp
fuser -k 33346/tcp
fuser -k 33347/tcp
fuser -k 33348/tcp
fuser -k 33349/tcp


wait

make clean
wait
make
clear

#record time and start getting data
echo "start receiving data!"
start=$(date +%s.%N)

#split the file into file parts

#run fast transmit on these file parts

./improved_recv.o "$2" small_file_00 36660 33330&
./improved_recv.o "$2" small_file_01 36661 33331&
./improved_recv.o "$2" small_file_02 36662 33332&
./improved_recv.o "$2" small_file_03 36663 33333&
./improved_recv.o "$2" small_file_04 36664 33334&
./improved_recv.o "$2" small_file_05 36665 33335&
./improved_recv.o "$2" small_file_06 36666 33336&

#wait

./improved_recv.o "$2" small_file_07 36667 33337&
./improved_recv.o "$2" small_file_08 36668 33338&
#get rid of this if file is smaller than 800MB
#./improved_recv.o "$2" small_file_09 36669 33339



wait
end1=$(date +%s.%N)  # Get the current time again at the end of your script
runtime1=$(awk "BEGIN {print $end1 - $start}")
echo ""
echo "transmission time: $runtime1 seconds"

echo "end of tranmission, now assemble file"

cat small_file_00 small_file_01 small_file_02 small_file_03 small_file_04 small_file_05 small_file_06 small_file_07 small_file_08 > "$1"

end=$(date +%s.%N)  # Get the current time again at the end of your script
runtime=$(awk "BEGIN {print $end - $start}")

rm small_file_*

FILE_SIZE=$(stat -c%s "$1")
#thruput=$(awk "BEGIN {print ($FILE_SIZE/$runtime1*1000000)*8}")
#echo "Throughput of this transmission: $thruput Mbps"

