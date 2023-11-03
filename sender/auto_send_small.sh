#pass the filename

if [ -z "$1" ]; then
    echo "Please provide a filename as an argument."
    exit 1
fi

if [ -z "$2" ]; then
     echo "please give the ip address of the receiver."
     exit 3
fi
# Check if the file exists
if [ ! -f "$1" ]; then
    echo "File '$1' not found!"
    exit 2
fi
#clean network channel
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

make clean
wait
make
clear

#start transmission

echo "strart transmission"

start=$(date +%s.%N)



#split the file into 10 file parts
split -n 9 -d "$1" small_file_

./improved_sender.o "$2" small_file_00 36660 33330&
./improved_sender.o "$2" small_file_01 36661 33331&
./improved_sender.o "$2" small_file_02 36662 33332&
./improved_sender.o "$2" small_file_03 36663 33333&
./improved_sender.o "$2" small_file_04 36664 33334&
./improved_sender.o "$2" small_file_05 36665 33335&
./improved_sender.o "$2" small_file_06 36666 33336&
./improved_sender.o "$2" small_file_07 36667 33337&
./improved_sender.o "$2" small_file_08 36668 33338&
#./improved_sender.o "$2" small_file_09 36669 33339&


wait

end=$(date +%s.%N)  # Get the current time again at the end of your script
runtime=$(awk "BEGIN {print $end - $start}")
echo "Total time: $runtime seconds"
FILE_SIZE=$(stat -c%s "$1")
echo "$FILE_SIZE"
rm small_file_*
