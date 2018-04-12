================================================================================
Optimus; Autonomous RC Car Project
================================================================================
A self-driving car prototype whose destination is configured over a mobile
application.
We use sensors to detect obstacles; GPS and compass to learn the car's
location and desired direction of movement.
The self driving solution is realised using five controllers each of which
have a dedicated task. The different controllers talk to each other in realtime
using high-speed CAN transceivers.

================================================================================
Platform
================================================================================
SJ One Board (ARM Cortex M3; LPC 1758 based carrier board; 
        more info: http://www.socialledge.com/sjsu/index.php?title=SJ_One_Board)

================================================================================
Modules
================================================================================
1) Master Controller
2) Sensor Controller & I/O
3) Bridge Controller + Mobile Application (Android)
4) Motor Controller (Servo)
5) Geo Controller (GPS, Compass)
