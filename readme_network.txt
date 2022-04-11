PMP Mod v1.02 (M4g3) by jonny (http://jonny.leffe.dnsalias.com/)
Additions by malloc and Raphael
GUI graphics by argandona
Network code by optixx



1) Edit the pmp.ini file.

[net]

# This is index to your Wlan config, so that you can select which Wifi Setup
# to use.
con         = 0;

# Enable the Network module
enabled     = 1;

# Ip and port of your Remote Streaming Server
uri         = 192.168.1.100:3333;

3) Copy Binary and pmp.ini to your PSP

2) Edit the pmp_server.py and assign a proper ROOT dir for your movies.

3) Start the server.

$python ./pmp_server.py 			    

4) Start pmp_mod on your PSP


