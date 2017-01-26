# Build Guide

Gateway를 사용하기 위한 Build 방법등에 대해 설명한다.


## Dependency Lib.
* Common Util Lib.
* Bluetooth Lib.
* MySQL Lib.


#### Dependency Lib. Install Guide
- Common Lib.
Util 성격의 기본적인 Lib를 설치한다.<br>
Install Command:<br>
sudo apt-get install libdbus-1-dev<br>
sudo apt-get install libglib2.0-dev<br>
sudo apt-get install libdbus-glib-1-dev<br>
sudo apt-get install libusb-dev<br>
sudo apt-get install libudev-dev<br>
sudo apt-get install libreadline-dev<br>
sudo apt-get install libical-dev<br>
<br>

- Bluetooth Lib.
BLE 통신을 위해 BlueZ Lib를 설치한다.<br>
Install Command:<br>
wget http://www.kernel.org/pub/linux/bluetooth/bluez-5.4.tar.xz<br>
tar -xvf bluez-5.4.tar.xz<br>
./configure --disable-systemd<br>
make<br>
※ BlueZ 공식 사이트: [http://www.bluez.org](http://www.bluez.org/)<br>

- MySQL Lib.
Device의 정보등을 저장하기 위해 MySQL Lib를 설치한다.<br>
Install Command:<br>
sudo apt-get install mysql-server<br>
sudo apt-get install mysql-client<br>
<br>
MySQL 설정 파일:<br>
설정 파일 위치: /etc/mysql/my.cnf<br>
IP, Port, Memory 등은 사용자에 맞게 설정.<br>
※ MySQL 공식 사이트: [https://www.mysql.com](https://www.mysql.com/)<br>


#### Gateway Build Guide
Grib Ble Hub Source를 다운로드 받는다.<br>
Grib Ble Hub Source는 하기와 같은 구조로 되어있다.<br>
![Guide SourceDirectory](./IMG/SourceDirectory.png)<br>
<br>
Grib Hub Build Command:<br>
cd “GRIB HUB DIRECTORY”<br>
make hub_clean<br>
make hub<br>
grib_hub 실행파일 생성<br>
<br>
Grib HCI Build Command:<br>
cd “GRIB HUB DIRECTORY”<br>
make hci_clean<br>
make hci<br>
grib_hci 실행파일 생성<br>
<br>
