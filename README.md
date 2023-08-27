# BNO085-Test
mcu esp8266（esp12f）
用于对BNO08X系列进行出厂校准和查看原始数据。
如果你的085半小时不到就歪了很多，可以考虑试试校准，希望对你有帮助。

校准前----
![image](https://github.com/FateBreaker/BNO085-Test/assets/25700846/95fcf05d-abf3-480b-be6f-79e21ac1e8c4)

校准后----
![image](https://github.com/FateBreaker/BNO085-Test/assets/25700846/ecad658b-9e00-4359-bc79-df66e2408660)

校准流程：
1.刷入校准固件
2.分别做加速度计（六面校准法），陀螺仪（平面静置），磁力计（缓慢画八字）校准动作，直到精度都为hign，在串口中输入s，将校准数据保存到BN008X的内存中。
3.刷回到正常的SlimeVR固件

配置方法：
修改Wire.begin(D2, D1);函数中的SDA,SCL引脚，对应到自己的PCB即可。


