## Wonderwinder

OSC to stepper control.

Let's you know it's alive by sending an osc message every 5 seconds:
/mirror/ack <i:id>

Where id, is the last number of the steppers IP address in the network, so if the IP is AA.BB.CC.DD, the id of the stepper will be "DD".

To make it spin:

```
/mirror/rot <i:dir> <i:steps> <i:speed>
where:
	- dir: direction of rotation [1, -1] CC or CCW.
	- steps: number of steps, see stepper specs for steps per rotation
	- speed: speed in steps per second

/mirror/stop - stops the motor 
```

werkze

