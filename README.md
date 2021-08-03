# IoT-Remote-Robot
This quick and dirty IoT enabled InfraRed Remote control is a way to remotely control a Robosapien Robot. It uses web interface that looks like the remote control that comes with the Robosapien, AWS S3 to serve the web page, API Gateway receive the commands from the web remote, and processes these commands in a Lambda function that uses http to publish the command to an AWS IoT Core MQTT broker.  The Robot has a backpack that is 3D printed that hooks to its back.  Out of the backpack is a wire stalk that look like a bent antenna that holds an IR LED and that connects to electronics in the backpack.  The IR stalk is pointed out the head of the Robosapien and directs the IR light to the IR receive located there.  Inside the backpack there is a driver circuit for the IR LED, a ESP8266 (Could easily be converted to a ESP32) that has a WIFI module. Flashed into the ESP8266 is a firmware to access the local WIFI AP, create a secure connection to AWS IoT Core's MQTT broker and the logic to subscribe to the command messages originating in the Lambda function mentioned above. The backpack also contains the insides of a USB charging battery that has a lithium battery and the circuitry to charge from a 5 volt source and to provide 5 V to the ESP8266 and the IR LED. There are some diagrams, images and model rendering below.

WARNING: This is a quick and dirty project. Normal engineering rigor and cyber security has not been applied here and your UX might vary.

## Background
During Covid my nephew, who lives across the state from me, was getting bored couped up at home with my sister. I would call and facetime and we would crack jokes to each other but couldn't really play together. I was thinking about how we could make it so we could remotely play and thought of those things in my house that could be remotely controlled.  I had gotten the Robosapien as a gag gift years ago for Christmas and this project was born.

## Remote Play Use Case
The way my nephew and I would use this is as follows.  
 * I would text my nephew the URL for the web remote.  
 * Once he had the remote up on his phone I would Facetime/Zoom him and he would answer on his iPad/Laptop. 
 * I would turn on the Robosapien and his IoT Backpack and let it connect to the Wifi and then to AWS.  When the backpack glowed blue (LED in the ESP8266) I knew it was connected.
 * I would give the thumbs up to my nephew and turn my phone with Facetime or laptop with Zoom running on it so that he could see the robot.
 * My nephew would then begin to control the robot remotely and laughing would ensue.

## Block Diagram
Here you can see the system block diagram.  

![Example SimpleBox(s)](https://github.com/schnorea/Iot-Remote-Robot/blob/master/assets/Architecture.png?raw=true "Example SimpleBox(s)")

## The Remote
The remote is assembled from very basic web tech.  HTML, SVG, Javascript, AJAX and a bit of CSS. Each button of the remote, in the SVG, has the code associated with the three modes the remote can be in attached.  With a click event the Javascript pulls the correct IR code from the button and performs a get to the AWS API Gateway.  You will need to setup your own API gateway for your own robot.  

## AWS
In AWS we use S3 to host and server the Web Remotes content.  S3 can be setup to server content by allowing it to be exposed publicly.  Again, this isn’t meant to be a product and is something that isn’t running all the time so quick and dirty.

Commands from the Web Remote are received via an API Gateway.  The gateway is configured to recognize the GET method which then triggers a Lambda function.

The Lambda function’s job is to take the Remote commands received via API Gateway and to publish them to the MQTT broker.  Yes, I could have cut out the Lambda function by doing MQTT over websockets but I didn’t want to have to have my Nephew have to sign-in to access the Web Remote.

The IoT Core MQTT broker is setup with a policy that allows commands to by publish and subscribed to.  It also has authentication and encryption enabled.  This is perhaps the most secure part of the pipeline. It also means that the backpack has to have credentials to communicate with the MQTT broker.

## The Backpack
As mentioned above the backpack is built around a ESP8266 Wifi module I ordered from Amazon (~$6.00).  Around the module are circuitry for the IR LED transmitter, a lithium battery and charging system I stole from a cell phone charging battery.  This is housed in a 3D printed case that was partially designed for this application and partially borrowed from a light dimming project.

## Possible Improvements
The ESP8266 is has barely enough power to run this app. I think the problem is the encryption overhead.  As a result of this the IoT backpack sometimes takes a long time to connect as it times out due to potentially poor comms making either the computation take longer. (Anecdotally, it seems to connect better with a better WiFi signal). Improvements port it to ESP32 which I don’t believe will have this problem.
I think there is just enough performance in an ESP32-CAM to do both the IoT function and stream the video so one idea would be to incorporate the video stream in the Web Remote interface.  This would simplify the use and also mean that player would have a robot eye view (First Person Robot?).

## Other uses
Since building this and having my nephew remotely come over and play a couple of other uses cases have presented themselves. 
Change channels of TV at home.  Why?
Turn on Air condition before getting home. Some units have IR remotes.
Attach it to other IR controlled toys.

![image](https://user-images.githubusercontent.com/10821682/128093920-b6a80558-c215-4dca-b227-bcc15af8e706.png)
