# How to run the TUG demo

This tutorial will show you how to run the TUG demo on the [real robot](#running-the-tug-demo-on-the-robot) and within the [simulation environment `gazebo`](#running-the-tug-demo-on-gazebo).

## Running the TUG demo on the robot

The related application can be found [here](https://github.com/robotology/assistive-rehab/blob/master/app/scripts/AssistiveRehab-TUG.xml.template).

### Requirements

The following hardware is required:

- **NVIDIA graphics card**: for running `yarpOpenPose`;
- **[Mystrom wifi button](https://mystrom.ch/wifi-button/)**: for triggering the speech pipeline;

    !!! tip "Alternatively"
        The speech pipeline can be alternatively triggered by the hand up / down option (see [below](#running-the-demo))

- **Setup for external microphone**

### Setting up the robot

Turn on the robot, by switching on cpu and motors.

Open `yarpmanager`, click on the `Cluster` tab on the left. Click on the play icon below the `nameserver node`, then select the systems in the window underneath and click on play.


!!! tip "R1-mk2"
    Specifically, R1-mk2 has the following systems available:

    - `r1-base`: where `yarpserver` runs;
    - `r1-console-linux`: where the main modules and visualizers run;
    - `r1-console-cuda`: connected to an external Cuda box, where `yarpOpenPose` runs. Here we also deployed the [Virtual Machine](#virtual-machine-for-speech-interaction) for the speech pipeline;
    - `r1-torso1`: where the camera runs;
    - `r1-face`: where the speak and the face expression modules run;

Now that all systems are running, open a terminal and type:

```
ssh -X r1-base
yarprobotinterface
```

!!! caution
    The robot is going to move around during the demo.
    Therefore, after running the `yarprobotinterface`, be sure to unplug it from the power supply.

#### Virtual Machine for speech interaction

For running the [`speechInteraction`](https://github.com/robotology/assistive-rehab/tree/master/modules/speechInteraction) modules, you will need access to [**Google Cloud API**](https://cloud.google.com/natural-language/), which is currently deployed on the `Ubuntu 18 Virtual Machine (VM)` installed on `r1-console-cuda`.

On this system, open the `VM` and start `Ubuntu18-VM`.

When the system is on, open `yarpmanager` and run the `Google_Speech_Processing` app. Hit play and then connect.

!!! warning
    Be sure that the external microphone is connected to the system.

### Running the demo

Now we are ready to run the demo!

In `yarpmanager`, click on `Applications` and then `Assistive Rehabilitation TUG App` app, hit run and then connect.
The demo is now running!

Before starting the TUG, we need to define our world made of lines, robots and skeletons!
For doing so, you can follow [this tutorial](tug_lines.md#detecting-lines-on-the-robot).

Let's run a TUG session!

!!! question "Want to ask questions to the robot?"
    Remember to be equipped the WiFi button, the external microphone and the Virtual Machine running.

    !!! tip "Alternative to WiFi button"
        The speech pipeline can be alternatively triggered by the hand up / down option: the 3D skeleton is analyzed to detect if the hand is raised up / dropped down and consequently a start / stop is sent to the speech pipeline (the hand has to stay up during the question). This can be achieved by running `managerTUG` with the option `--detect-hand-up true`.
        However, such approach is less robust than using the button for two reasons:

          1. the person is assumed to be always in the FOV, which cannot be guaranteed in a HRI scenario;
          2. it may lead to false detection and thus trigger the speech when not required.

Open a terminal and type:

```tab="Terminal 1"
yarp rpc /managerTUG/cmd:rpc
start
```

The robot will move to the starting position defined by `starting-pose` [here](https://github.com/robotology/assistive-rehab/blob/master/modules/managerTUG/app/conf/config-it.ini).
When the robot has reached this point, the interaction starts!

The interaction keeps going on, with the robot keeping engaging the user.
You can stop it by typing in `Terminal 1`:

```tab="Terminal 1"
stop
```

## Running the TUG demo on `gazebo`

The related application can be found [here](https://github.com/robotology/assistive-rehab/blob/master/app/scripts/AssistiveRehab-TUG_SIM.xml.template).

### Dependencies

After [installing](https://robotology.github.io/assistive-rehab/doc/mkdocs/site/install/) `assistive-rehab`, you will need the following dependencies:

- [cer](https://github.com/robotology/cer/tree/devel): for running the gaze-controller and face expressions;
- [navigation](https://github.com/robotology/navigation): for controlling the robot wheels;
- [gazebo](https://github.com/vvasco/gazebo): for running the virtual environment;

!!! warning "Work in progress"
    This is a fork of `gazebo` which contains the required changes to the `Actor` class. We are working to open a pull request on the upstream repository.

- [gazebo-yarp-plugins](https://github.com/robotology/gazebo-yarp-plugins): for exposing YARP interfaces in `gazebo`;
- [cer-sim](https://github.com/robotology/cer-sim): which includes the model loaded by `gazebo` in `tug-scenario.world`;
- [speech](https://github.com/robotology/speech): for running the `iSpeak` module;
- [nodejs](https://nodejs.org/en/download/package-manager/): for handling triggers from the wifi button.

### Requirements

The following hardware is required:

- **NVIDIA graphics card**: for running `yarpOpenPose`;

#### Optional

If you want to simulate the speech interaction, you will need:

- access to [**Google Cloud API**](https://cloud.google.com/natural-language/): for running the [`speechInteraction`](https://github.com/robotology/assistive-rehab/tree/master/modules/speechInteraction) modules;

!!! warning
    You will need to have an account to access Google Cloud API.

- a [Mystrom wifi button](https://mystrom.ch/wifi-button/).

!!! info "How to configure the button"
    On the robot, the WiFi button is already configured to send triggers within the robot network. To configure your own button to run within your network, follow [this tutorial](wifi_button.md).

### Running the demo

Now that all dependencies and requirements are met, you are ready to run the demo in `gazebo`!

The first step is to open a terminal and run `yarpserver`.
Open `yarpmanager`, run the `AssistiveRehab-TUG_SIM App` and hit connect.

!!! important
    If you wish to simulate the speech interaction, be sure to run `node-button.js` on the machine you used to configure the wifi button, as explained [here](wifi_button.md).
    This script is in charge of getting the `POST` request from the button and sending a trigger to `managerTUG`.

The TUG scenario appears within the simulation environment, including the robot, a human model standing in front of it, a chair and two ArUco lines:

![default_gzclient_camera(1)-2020-05-15T18_01_58 893982](https://user-images.githubusercontent.com/9716288/82071312-3b488e80-96d6-11ea-8cee-d9119a396060.jpg)

!!! info
    The loaded scenario is described in [`tug-scenario.world`](https://github.com/robotology/assistive-rehab/blob/master/app/gazebo/tug/tug-scenario.world).

!!! tip
    By default, `gazebo` has the origin and the external gui visible. To remove the origin, you can click on `View` and deselect `Origin`. To remove the gui, you can click on `Window` and `Full screen`.

When the demo is launched, `managerTUG` waits for the command `start` to run the TUG session, which can be sent to the rpc port `/managerTUG/cmd:rpc`.
Once this is provided, the interaction starts and the simulated user successfully completes the test, while its step length is computed in real-time.
The following video shows a successful test:

[![normal](https://user-images.githubusercontent.com/9716288/82469666-a2928400-9ac4-11ea-8f1f-170f7349fdc5.png)](https://youtu.be/qbAJ97jQw_c)

!!! tip
    The demo keeps going on even after the TUG is completed, with the robot engaging the human model, which executes the TUG.

    If you want to stop the demo, you will need to provide a `stop` command to  the rpc port `/managerTUG/cmd:rpc`.

#### Simulating the verbal interaction

The verbal interaction between the robot and the human model is simulated through the following steps:

- the [Mystrom wifi button](https://mystrom.ch/wifi-button/) is integrated within the simulated demo: when the button is pressed, the human model / the robot (depending which one is moving) stops and a question can be "asked";
- a question is simulated through the [`question-simulator.sh`](https://github.com/robotology/assistive-rehab/blob/master/app/scripts/question-simulator.sh) script. The user can choose 4 keywords, specifically `speed`, `repetition`, `aid`, `feedback`, each associated to a set of questions. The script randomly selects a question associated to the chosen keyword and sends it to `googleSpeechProcess` which analyses the sentence; when `googleSpeechProcess` provides the output to `managerTUG`, the human model / the robot starts walking / navigating from where it stopped.

The following video shows how the verbal interaction is integrated within the demo:

[![questions](https://user-images.githubusercontent.com/9716288/82469863-e5ecf280-9ac4-11ea-900e-53e96ee3267d.png)](https://youtu.be/b7_848Rt98E)

#### Simulating failures

Thrift services are provided in order to simulate the human model failing the TUG.
Two possible failures are implemented:

- **the human model does not reach the target**: the robot says that the test was not passed. This can be achieved providing the command `set_target` to `/managerTUG/cmd:rpc`, as shown in the following video:

[![not-reached](https://user-images.githubusercontent.com/9716288/82470125-395f4080-9ac5-11ea-8aca-09fb4916de20.png)](https://youtu.be/OZFbunThnVE)

!!! note
    The target is defined with respect to the gazebo world frame, thus `X` pointing forward (with respect to the human model), `Y` pointing left, `Z` pointing up.

- **the human model stops**:  the robot encourages the user to finish the test, reminding to push the button to ask questions. If the human model does not complete the test, the test is not passed. This can be achieved providing the command `pause` to `/tug_input_port`, as shown in the following video:

[![pause](https://user-images.githubusercontent.com/9716288/82470264-6875b200-9ac5-11ea-8889-3f06eca99877.png)](https://youtu.be/ehFkVaHG8tA)

!!! tip
    You can also set the time during which the human model has to be paused, by specifying the seconds after `pause`. For example, to pause it for `2 s`, you can provide `pause 2.0` to `tug_input_port`: after the `2 s`, the human model starts walking again.
