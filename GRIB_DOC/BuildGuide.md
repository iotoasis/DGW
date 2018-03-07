# Build Guide

This guide shows how to build for using a gateway, and so on.


## Dependency Lib.
* Common Util Lib.
* Bluetooth Lib.
* MySQL Lib.


### Dependency Lib. Install Guide
- Common Lib.<br>
Installs a basic Lib of Util nature.<br>
Install Command:<br>
sudo apt-get install libdbus-1-dev<br>
sudo apt-get install libglib2.0-dev<br>
sudo apt-get install libdbus-glib-1-dev<br>
sudo apt-get install libusb-dev<br>
sudo apt-get install libudev-dev<br>
sudo apt-get install libreadline-dev<br>
sudo apt-get install libical-dev<br>
<br>

- Bluetooth Lib.<br>
Install BlueZ Lib for BLE communication.<br>
Install Command:<br>
wget http://www.kernel.org/pub/linux/bluetooth/bluez-5.4.tar.xz<br>
tar -xvf bluez-5.4.tar.xz<br>
./configure --disable-systemd<br>
make<br>
※ BlueZ official site: [http://www.bluez.org](http://www.bluez.org/)<br>

- MySQL Lib.<br>
Install MySQL Lib to store device information and so on.<br>
Install Command:<br>
sudo apt-get install mysql-server<br>
sudo apt-get install mysql-client<br>
<br>
MySQL configuration file:<br>
Configuration file location: /etc/mysql/my.cnf<br>
IP, Port and Memory are customized.<br>
※ MySQL official site: [https://www.mysql.com](https://www.mysql.com/)<br>


### Gateway Build Guide

Grib Ble Hub Source has the following structure.<br>
<br>
![Guide SourceDirectory](./IMG/SourceDirectory.png)<br>
<br>
Grib Hub Build Command:<br>
cd “GRIB HUB DIRECTORY”<br>
make hub_clean<br>
make hub<br>
Check the creation of "grib_hub" executable file<br>
<br>
Grib HCI Build Command:<br>
cd “GRIB HUB DIRECTORY”<br>
make hci_clean<br>
make hci<br>
Check the creation of "grib_hci" executable file<br>
<br>
