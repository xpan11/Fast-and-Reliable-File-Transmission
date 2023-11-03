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


make clean
wait
make
wait
large_file() {
	
	name="$1"
	ip="$2"
	split -n 20 -d "$name" small_file_

	./improved_sender.o "$ip" small_file_00 36660 33330&
	./improved_sender.o "$ip" small_file_01 36661 33331&
	./improved_sender.o "$ip" small_file_02 36662 33332&
	./improved_sender.o "$ip" small_file_03 36663 33333&
	./improved_sender.o "$ip" small_file_04 36664 33334&
	./improved_sender.o "$ip" small_file_05 36665 33335&
	./improved_sender.o "$ip" small_file_06 36666 33336&
	./improved_sender.o "$ip" small_file_07 36667 33337&
	./improved_sender.o "$ip" small_file_08 36668 33338&
	./improved_sender.o "$ip" small_file_09 36669 33339
	
	wait
	./improved_sender.o "$ip" small_file_10 36670 33340&
	./improved_sender.o "$ip" small_file_11 36671 33341&
	./improved_sender.o "$ip" small_file_12 36672 33342&
	./improved_sender.o "$ip" small_file_13 36673 33343&
	./improved_sender.o "$ip" small_file_14 36674 33344&
	./improved_sender.o "$ip" small_file_15 36675 33345&
	./improved_sender.o "$ip" small_file_16 36676 33346&
	./improved_sender.o "$ip" small_file_17 36677 33347&
	./improved_sender.o "$ip" small_file_18 36678 33348&
	./improved_sender.o "$ip" small_file_19 36679 33349&


}

small_file() {
	
	name="$1"
	ip="$2"

	split -n 9 -d "$name" small_file_
	touch small_file_09 small_file_10 small_file_11 small_file_12 small_file_13 small_file_14 small_file_15 small_file_16 small_file_17 small_file_18 small_file_19

	./improved_sender.o "$ip" small_file_00 36660 33330&
	./improved_sender.o "$ip" small_file_01 36661 33331&
	./improved_sender.o "$ip" small_file_02 36662 33332&
	./improved_sender.o "$ip" small_file_03 36663 33333&
	./improved_sender.o "$ip" small_file_04 36664 33334&
	./improved_sender.o "$ip" small_file_05 36665 33335&
	./improved_sender.o "$ip" small_file_06 36666 33336&
	./improved_sender.o "$ip" small_file_07 36667 33337&
	./improved_sender.o "$ip" small_file_08 36668 33338&
	./improved_sender.o "$ip" small_file_09 36669 33339
	
	wait
	./improved_sender.o "$ip" small_file_10 36670 33340&
	./improved_sender.o "$ip" small_file_11 36671 33341&
	./improved_sender.o "$ip" small_file_12 36672 33342&
	./improved_sender.o "$ip" small_file_13 36673 33343&
	./improved_sender.o "$ip" small_file_14 36674 33344&
	./improved_sender.o "$ip" small_file_15 36675 33345&
	./improved_sender.o "$ip" small_file_16 36676 33346&
	./improved_sender.o "$ip" small_file_17 36677 33347&
	./improved_sender.o "$ip" small_file_18 36678 33348&
	./improved_sender.o "$ip" small_file_19 36679 33349&

}



clear

FILE_SIZE=$(stat -c%s "$1")
#echo "$FILE_SIZE"


#start transmission

echo "start transmission"

start=$(date +%s.%N)

#if [ $FILE_SIZE -gt 800000000 ]; then
        large_file "$1" "$2"

#else
#       small_file "$1" "$2"
#fi
wait
#split the file into 10 file parts



end=$(date +%s.%N)  # Get the current time again at the end of your script
runtime=$(awk "BEGIN {print $end - $start}")
echo "Total time: $runtime seconds"


rm small_file_*
