# News Channel PSP
A news program for PSP that attempts to mimic the style of the Wii's News Channel. It will feature a GUI with high resolution images and news obtained from official RSS sources in real time.

The news backend is being written in Golang and can be found in the News-GO repository. It will check if the connecting client is a PSP, and if so, it will return PSP-friendly RSS files ready for use.

The client will be written in C and will be a very straightforward GUI in the style of the Wii News Channel, which parses the RSS data into usable form.
