![Logo](https://github.com/iotoasis/SO/blob/master/logo_oasis_m.png)


## HY Sensor

 HY Sensor의 HW는 Cortex M4를 사용하며 RTOS를 기반으로 동작한다.
 
 Sensing 결과를 Gateway로 전송하기위한 Task를 동작 시키기 위한 Source Code 이다.
 
 Gateway와 로컬 통신중 BLE에서는 Sensor가 Slave Mode로 동작하여, 일정 주기로 데이터를 전달한다.
