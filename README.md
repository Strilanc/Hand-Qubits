# Hand Qubits

*Work in progress*

The basic idea is to make some physical hand-sized balls that act like qubits.
To rotate a qubit, you just rotate the ball.
To measure a qubit, you slam it onto a surface.
To do conditional operations, you touch the target ball to the control ball then rotate the target.

Inside each ball there will be a gyrometer, accelerometer, and bluetooth coordinate by an arduino microcontroller.
The only time data needs to go *to* the microcontrollers is for a measurement.
The slamming motion should mask any latency in the result.
Data coming from the microcontroller may be raw motion data, or may be heavily processed to cut down on bandwidth.
Dunno yet.

# Progress

Breadboards hidden inside hollowed-out polystyrene balls, broadcasting motion data via bluetooth which is being received by the laptop.
(The USB cable is just for power, not serial output.)

![breadboard taped to cardboard](/progress.jpg)
