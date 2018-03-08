![Logo](https://github.com/iotoasis/SO/blob/master/logo_oasis_m.png)

# Oasis Project

 This Oasis (Open-source Architecture Semantic IoT Service-platform) project aims to develop an open source, intelligent Internet service platform based on international standards.

 This Oasis project will be open source as the outcome of the "(ICBMS-3 specification) IoT-based Platform Technology Development Supporting Object Virtualization, Distributed Autonomic Intelligence, and Data Linkage/Analysis" as a newly supported by IT & broadcasting technology development project (2015).

 This Oasis project will continue to grow as an open source based on the open source community.

# DGW (Device Gateway)
 DGW is greatly divided into Sensor/Device that collects data and performs specific functions, and Gateway(Hub) which plays the role of exchanging data collected from Sensor/Device with SI Server.<br>
 - Hereinafter, the thing that contains only a data collecting function is called a "Sensor" and the thing that can collects data and performs specific functions are called "Device".


## Gateway(Hub)
 Hereinafter, the thing that contains only a data collecting function is called a "Sensor" and the thing that can collects data and performs specific functions are called "Device".<br>
 Gateway has "Report" function that collects data from sensor/device, processes it and sends it to SI server,<br>
 and has control function to control device by receiving control message from SI Server.
 

## Sensor/Device
 HW of Grip of Sensor/Device uses Aduino series(+CC2541 BLE Module).<br>
 Sensor/Device operates in Slave Mode during Gateway and BLE communication and transmits data at regular intervals.<br>
 <br>
 Please refer to the following link for a description of sensor developed independently at Hanyang University.<br>
 [Hanyang University Sensor Description](./HY_SENSOR/Readme_HY.md)<br>


## Documents
 - [SetUp Guide](./GRIB_DOC/SetupGuide.md)
 - [Build Guide](./GRIB_DOC/BuildGuide.md)
 - [Gateway Guide](./GRIB_DOC/GatewayGuide.md)


## Downloads
 - [Latest Release](https://github.com/iotoasis/DGW/releases/)


## License
 Licensed under the BSD License, Version 2.0
<br>