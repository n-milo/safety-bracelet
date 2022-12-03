# Safety Bracelet

This repository holds the code for the Safety Bracelet, designed for
APSC 200. The user presses a button on the bracelet, and the app will
contact one of the user's emergency contacts. An Arduino listens for
button presses, and transmits data to a Bluetooth client running the
receiver app. The receiver app will then text an emergency contact.

## Demo

https://user-images.githubusercontent.com/12551569/205416682-bf3281f5-0984-43d7-bce5-7e4d4b6adbe3.mp4

Explanation: pressing the button (blue LED) activates the bracelet,
but only if the activation switch (pressure resistor; yellow LED) is
held. The Arduino then sends data to the receiver app over Bluetooth.
