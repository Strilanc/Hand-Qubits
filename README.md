# Hand Qubits

*Work in progress*

The goal of this project is to make toy qubits that you can hold.

Each qubit will be a ball.
To rotate a qubit, turn the ball.
To measure a qubit, knock the ball against a surface.
To interact two qubits, press them together and turn one.
A nearby laptop talks to the qubits over bluetooth and handles the actual quantum simulation.

# Making

Inside each ball is a breadboard with an Arduino Uno, an MPU-6050 accelerometer/gyrometer, an HC-05 (or HC-06) bluetooth module, a buzzer, and a 9V battery:

The balls are wrapped in copper tape so that, when they touch, the Arduinos inside each can talk to each other over the janky shared electrical connection.

0. Clone the repository.

    `git clone https://github.com/Strilanc/Hand-Qubits.git`

0. Get the necessary parts.

    - [ ] Arduino [Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno) or [Nano](https://www.arduino.cc/en/Main/arduinoBoardNano) or similar
    - [ ] [MPU-6050](http://playground.arduino.cc/Main/MPU-6050) Accelerometer/Gyrometer ([datasheet](https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf))
    - [ ] [HC-05](https://www.amazon.com/CHENBO-Wireless-Bluetooth-Transceiver-Arduino/dp/B00Y0D112O) Bluetooth Module ([datasheet](https://www.olimex.com/Products/Components/RF/BLUETOOTH-SERIAL-HC-06/resources/hc06.pdf))
    - [ ] [Buzzer](https://www.amazon.com/gp/product/B00B0Q4KKO/)
    - [ ] [Switch](https://www.amazon.com/gp/product/B00ZWWZ5BA)
    - [ ] [~15cm Diameter Polystyrene Ball](https://www.amazon.com/Crafts-Brand-Smooth-Polystyrene-Styrofoam/dp/B00ETI28MC/)
    - [ ] [Conductive Paint](https://www.amazon.com/Bare-Conductive-Electric-Paint-10ml/dp/B01IO2JSCG)
    - [ ] Breadboard / Various Wires / 9V Battery / Appropriate USB Cable / Soldering Stuff / Etc

0. Configure parts.

    - Open `Hand-Qubits/qubit-microcontroller/qubit-microcontroller.ino` with the [Arduino IDE](https://www.arduino.cc/en/main/software)
	- Upload to the Arduino
    - Also [Configure the HC-05 Bluetooth Modules](https://arduino-info.wikispaces.com/BlueTooth-HC05-HC06-Modules-How-To).
    - Change the addresses and pins in `Hand-Qubits/hub/src/config/KnownBoards.cs` to match.
    - Pair your computer with the Bluetooth modules.

0. Run server.

    - Open `Hand-Qubits/hub/HandQubitServer.sln` with [Visual Studio](https://www.visualstudio.com/vs/community/).
    - Run the project.
	- Once you've assembled things, the powered Arduinos can connect and start feeding in data.

0. Assemble electronics.

    Connect `vcc` and `ground` pins appropriately.
    The MPU-6050's `SDA` and `SCL` pins go to `A5` and `A6` respectively.
    The HC-06's `RX` and `TX` pins go to `D4` and `D3` respectively.
    The buzzer goes from `A3` to `ground`.
    `A0` will go to the conductive paint or copper tape wrapping around the ball.
    Also, connect the battery and switch.

    A diagram of the pin connections:

    ![layout](/img/layout.png)
	
	Once the electronics are ready, test that the powered Arduino connects and feeds motion data to the server.

0. Assemble balls.

    First, paint over the polystyrene ball with an acrlyic paint.
    This stops the polystyrene from shedding constantly when touched.
    After the acrylic has dried, add conductive paint traced in tangled lines all over the surface.
    Once the conductive paint has dried, cut the ball in half.
    
    Make a hole for the electronics to sit in by carving into the insides of each half-ball using a butter knife or scissors.
    Try to make the fit snug, and positioned so that the switch just barely pokes out the side.
    Once the hole is carved, paint it and the rest of the insides with acrylic so they don't shed.
    
    After placing the electronics inside, and somehow connecting A0 to the conductive paint traces, close up the halves and use an office stapler to staple them together.
    You can position the staples so they act as a conductive path from conductive paint on the top half to conductive paint on the the bottom half.

    Here are some pictures of balls in progress:

    ![guts](/img/guts.jpg)

    Staples can be pulled out easily, if you need to make changes.
    If you want a more permanent connection, such as a hinge between the backs of the two halves, I recommend sewing them together with needle and thread.
	
0. Play with your qubits!

# Current Progress

The main things left to do are:

1. Better debugging and testing of the Arduino code. Something is causing the contact detection to go flaky when the Arduinos are reset.
2. Correcting gyrometer drift with accelerometer data.
3. Correctly turning the motion into quantum rotations.
4. Make a demo video demonstrating quantum teleportation.
