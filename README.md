![Logo](https://github.com/iotoasis/SO/blob/master/logo_oasis_m.png)

# Oasis Project

�� Oasis (Open-source Architecture Semantic Iot Service-platform) ������Ʈ�� ���� ǥ���� �ؿ��ϴ� ���� �ҽ� ��� ������ �繰 ���ͳ� ���� �÷����� �����ϴ� ���� ��ǥ�� �ϰ� �ֽ��ϴ�.

�� Oasis ������Ʈ�� 2015�⵵ �������/��� ������߻�� �ű����� ������ "(ICBMS-3����) �繰 ����ȭ, �л� �������� �� ������ ����/�м��� �����ϴ� IoT ��� �÷��� ��� ����" ������ ������ν� ���¼ҽ��� �����˴ϴ�.

�� Oasis ������Ʈ�� ���� �ҽ� Ŀ�´�Ƽ�� ������� ���¼ҽ��ν� ��������� ������ ���� ��ȹ�Դϴ�.

# DGW (Device Gateway)
 DGW�� ũ�� �����͸� �����ϰ� Ư�� ����� �����ϴ� Sensor/Device��<br>
 Sensor/Device�� ���� ������ �����͸� SI Server�� ������ �ְ�޴� ������ �����ϴ� Gateway(Hub)�� ������.<br>
 �� ����, ������ ���� ��ɸ� ������ ���� Sensor, ������ ���� �� Ư�� ����� ������ �� �ִ� ���� Device�� �θ���.


## Gateway(Hub)
 Gateway�� HW�� Raspberry Pi3 Model B�� ����ϸ�, OS�� Raspbian(Debian �迭 Linux)�̴�.<br>
 Gateway�� Sensor/Device�� ���� �����͸� �����ϰ� �̸� �����Ͽ� SI Server�� �����ϴ� Report ��ɰ�<br>
 SI Server�� ���� ���� �޽����� �޾Ƽ� Device�� �����ϴ� Control ����� ������.
 

## Sensor/Device
 Grib�� Sensor/Device�� HW�� Aduino �迭(+CC2541 BLE Module)�� ����Ѵ�.<br>
 Gateway�� BLE ����߿� Sensor/Device�� Slave Mode�� �����Ͽ�, ���� �ֱ�� �����͸� �����Ѵ�.<br>
 <br>
 �Ѿ�뿡�� ���������� ������ ������ ���� ������ �ϱ� ��ũ�� ����.<br>
 [�Ѿ�� ���� ����](./HY_SENSOR/Readme_HY.md)<br>


## Documents
 - [SetUp Guide](./GRIB_DOC/SetupGuide.md)
 - [Build Guide](./GRIB_DOC/BuildGuide.md)
 - [Gateway Guide](./GRIB_DOC/GatewayGuide.md)


## Downloads
 - [Latest Release](https://github.com/iotoasis/DGW/releases/)


## License
 Licensed under the BSD License, Version 2.0
<br>