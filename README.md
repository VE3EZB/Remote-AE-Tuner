# Remote-AE-Tuner
Remote antenna tuner utilizing a WiFi connection

This project begain because my buddy has an 80/160m vertical antenna in a field beside his home. This antenna requires the use of an Antenna Tuning Unit (ATU) at the base of the antenna to match the antenna to the feed line. In this case, the ATU is nothing more than a couple of inductors (one for each band) and a relay to switch between the two inductors all installed in a box at the base of the antenna. The 12v dc power required for the relay is provided from a power supply located in the shack and fed via a ~36m (120ft) cable buried under the driveway and lawn.

For the 80m band, the inductor can be tuned by soldering the tap to the inductor at a specific point on the inductor. This should remain unchanged unless there are modifications made to the feedline or radials. However, at 160m, the antenna demands periodic "re-matching" or "re-tuning" when the operating frequency shifts across the band. Here lies the challenge. Using a manually adjusted inductor via the solder tap method becomes impractical, as it necessitates constant readjustment as the operating frequency is moved up/down the band.

How to tune it from the shack? Running new wire and installing a motor and end stop switches to adjust the inductor from the shack was certainly a workable idea but that involved tearing up the driveway and lawn to install more cables. Not ideal because the RF transmission cable (LDF-4 1/2" heliax) and the 12v dc relay power as well as the many ground radials buried around the antenna and digging another trench for a new wire, risked damaging the existing cable. 



The initial setup of this tuner will involve several edits and uploads of the software to the NodeMCU board 
before it will operate as intended. Below are the step by step instructions to setup and operate the tuner 
as designed. You will need the Arduino IDE 2.2.1 (or the latest version) which you can get from this 
website [https://www.arduino.cc/en/software]
