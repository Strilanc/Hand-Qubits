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

0. Get the necessary parts.

    - [ ] Arduino [Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno) or [Nano](https://www.arduino.cc/en/Main/arduinoBoardNano) or similar
    - [ ] [MPU-6050](http://playground.arduino.cc/Main/MPU-6050) ([datasheet](https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf))
    - [ ] [HC-05](https://www.amazon.com/Wireless-Bluetooth-Serial-Transceiver-Module/dp/B00INWZRNC) or [HC-06](https://www.amazon.com/Pass-Through-Communication-Compatible-Atomic-Market/dp/B00TNOO438) Bluetooth Module ([datasheet](https://www.olimex.com/Products/Components/RF/BLUETOOTH-SERIAL-HC-06/resources/hc06.pdf))
    - [ ] [Buzzer](https://www.amazon.com/gp/product/B00B0Q4KKO/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1) *(a constant 5V should produce sound)*
    - [ ] [~15cm Diameter Polystyrene Ball](https://www.amazon.com/Crafts-Brand-Smooth-Polystyrene-Styrofoam/dp/B00ETI28MC/ref=sr_1_cc_3?s=aps&ie=UTF8&qid=1485400789&sr=1-3-catcorr)
    - [ ] Repeat above for each qubit
    - [ ] [Copper Tape](https://www.amazon.com/BCP-Single-sided-Conductive-Shielding-Copper/dp/B01DVZ1G1C/ref=sr_1_3?s=industrial&ie=UTF8&qid=1485400752&sr=1-3&keywords=copper+tape)
    - [ ] Breadboard / Various Wires / 9V Battery / Appropriate USB Cable / Soldering Stuff / Etc

0. Configure the Bluetooth modules

    - [See this how-to](https://arduino-info.wikispaces.com/BlueTooth-HC05-HC06-Modules-How-To).
    - Pair your computer with the Bluetooth modules.
    - Change the addresses and pins in `Hand-Qubits/circuit-server/KnownBoards.cs` to match.


0. Assemble the parts.

    Besides the obvious vcc and ground pin connections... the MPU-6050's SDA and SCL pins go to A5 and A6 respectively, the HC-06's RX and TX pins go to D4 and D3 respectively, the buzzer goes from A3 to ground, and A0 goes to the copper tape wrapped around the hollowed-out spheres.


    A diagram of the pin connections:

    ![layout](/img/layout.png)

    A picture of the nearly-assembled product:

    ![guts](/img/guts.jpg)

    The amount of copper tape affects how easily the Arduinos can talk to each other via the A0-to-copper-to-copper-to-A0 connection, and probably how easy it is for the bluetooth signal to escape.
    I haven't done much experimenting to determine what works and what doesn't so... good luck.

0. Clone the repository.

    `git clone https://github.com/Strilanc/Hand-Qubits.git`

0. Open `Hand-Qubits/qubit-microcontroller/qubit-microcontroller.ino` with the [Arduino IDE](https://www.arduino.cc/en/main/software).

0. Upload the project to the Arduino(s).

0. Seal Arduino(s) inside ball(s), with 9V battery power.

    - And maybe some kind of switch to turn it off and on?

0. Open `Hand-Qubits/circuit-server/QubitServer.sln` with [Visual Studio](https://www.visualstudio.com/vs/community/).

0. Run the project, and wait for the powered Arduinos to connect and start feeding in data.

0. Play with your qubits!

# Current Progress

Most of the individual elements are working, but not reliably enough to have a nice experience.
The Arduinos notice when they are in electrical contact... sometimes.
The lack of a common ground really hurts.
The motion tracking is decent... but does drift, and the slamming to measure doesn't help.
The quantum simulation is half-done, but still applying operations in gyro coordinates instead of world coordinates and there's no qubit interaction done yet.

I still need to do:

- See if the balls can be covered in conductive paint without breaking the contact signalling or the bluetooth signal.
- Optimize the Arduino code (currently there's more than 1ms of work to do in some milliseconds).
- Get the contact signalling detection working better.
- Do actual fully-enclosed-in-balls tests.
- Correct accumulated gyrometer readings with accelerometer readings.
- See about 3d-printing the balls.
- Balance the contents of the balls so they don't want to roll so much.
- Make a demo video demonstrating quantum teleportation.

