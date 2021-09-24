
? I have artifacts on the screen that look round swirls or circles which look like tree trunk rings

    This kind of artifact is called a Moiré pattern - https://en.wikipedia.org/wiki/Moiré_pattern

    The base cause of the Moiré pattern is usually the curvature, if you set the curvature to 0 the moire should go away
    You could try the royale preset as royale handles the moire problem with curvature better than most.

    Also in general moire artifacts are less of a problem when you are at an integer scale and using higher resolutions.


Is the issue particular to the Mega Bezel vs the Standard CRT Shader?
    Since the Mega Bezel uses already existing great CRT shaders which people really like rather than coming up with it's own, 
    the effect you are seeing could be in the CRT shader you are using rather than anything specific about the Mega Bezel 
    
    To test this:
    * Find what the resolution is for the portion of the screen displaying the tube by turning on the **[SCREEN SCALE]  Screen Scale Debug** parameter which will show the resolution onscreen.
    * Set as a custom aspect resolution in the retroarch video settings
    * Load the standard shader, without the mega bezel, e.g. **crt-guest-dr-venom.slangp**

    If the effect still appears then this is directly to do with the CRT shader and not something that can be fixed within the Mega Bezel.

    Possible fixes
    * Try the royale preset as royale handles the moire problem with curvature better than most.
    * Turn Curvature to 0

    Use Fake Scanlines and reduce interlace trigger