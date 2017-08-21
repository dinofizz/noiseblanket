# NoiseBlanket
Arduino White Noise Player with IR Remote Control

After a recent sinus/ear infection I began to experience tinnitus. In my case it presents itself as a constant high frequency static hiss/whine in my left ear. It’s been about a month since I first noticed it. Hopefully it will eventually disappear. During the day when I’m at work or around friends I don’t really notice it. However when I’m in a quiet room, such as when falling asleep or in the early morning after waking up, it is quite noticeable and distracting.

I found that playing white noise found on various Spotify playlists (this one is my current favourite) as I fall asleep helps mask the hiss in my ear.

So to help me fall asleep and relax in the bedroom I put together a remote control capable Arduino-based white noise player, using the Adafruit Wave Shield and an IR receiver from SparkFun. This allows me to use my phone to watch Netflix or YouTube, whilst still having the white noise play in the background.

A more detailed description of this project can be found on my blog:
https://www.dinofizzotti.com/blog/2017-08-21-noiseblanket-arduino-white-noise-player-with-ir-remote-control/

# Repo contents

There are two Arduino sketches in this repo:

1. IRRecorder: sketch which prints out IR codes as recieved by the SparkFun IR Reciever.
2. NoiseBlanket: sketch which integrates the Wave Shield operation (using the WaveHC library) with the IRRemote library to load .WAV files and control the volume using an IR remote.

