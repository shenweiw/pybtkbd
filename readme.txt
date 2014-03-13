gcc -o bthidd bthidd.c -lbluetooth -Wall
sdptool browser local
gcc -o libbthidd.so bthidd.c sdp_helper.c -O2 -lbluetooth -Wall -fPIC -shared 
