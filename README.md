Was prototyped a hidraulic plant with its linear water level control by using DC pump, Wemos Lolin ESP32 Lite, ultrassonic sensor HC-SR04 and a analog circuit composed by a level adjust, subtractor, PI controller and a DC drive, both made with operational amplifiers such as TL084
![Image](https://github.com/user-attachments/assets/0e41d8fa-a17f-4397-ae7b-775c2a942993)
The image above shows its behavioral, the system requires a DC Source and a osciloscope to be understanded. The code has been uploaded by the arduino IDE to the microcontroller, with a microUSB cable. 
The system has been simulated in Simulink/MATLAB and compared to the real world behavioral, extracted from the output signal from the sensor. 
