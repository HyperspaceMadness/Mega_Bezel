------------------------------------------------------------------------------------------------------------
HyperspaceMadness Mega Bezel Shader Readme
------------------------------------------------------------------------------------------------------------
This file is best viewed in a markdown editor/viewer if possible.

Find more conversation about the shader here:
https://forums.libretro.com/t/hsm-mega-bezel-reflection-shader-feedback-and-updates

----------------------------------------
What does it do?
----------------------------------------

  * Adds an auto-generated bezel around the screen with reflection
  * Enable easier use of bezels and more "natural" presentation
  * Ease of use for screen scaling and automatic aspect ratio with existing shaders
  * Provide a consistent set of enhanced features wrapped around the core crt shaders
  * Layering images to add artwork and visual effects



----------------------------------------
How does it work?
----------------------------------------

  * In general there is a background image which fills the screen, then the scaled down game screen with an automatically generated bezel image is drawn on top. 
  * The bezel and frame you see around the screen is auto generated and not part of the background image
  * Additional Images can be layered on top to augment the look
  * Most things can be changed to your taste with adjustment of the parameters, so try them out!



----------------------------------------
Installation
----------------------------------------

* **You MUST use Retroarch Version 1.9.8 or Later** (It will fail to load on earlier versions)
  * If you have previous versions of the Mega Bezel installed:
    * Delete the old Mega Bezel stuff from shaders/shaders_slang/bezel
  * Inside the .zip is a bezel folder, copy the bezel folder into your Retroarch/shaders/shaders_slang folder
    * The final path to the Mega bezel should be `Retroarch/shaders/shaders_slang/bezel/Mega_Bezel`
  * Set video driver to **Vulcan**
    * It will run in **GLCore** but seems faster in **Vulcan**
    * Some users have run it successfully in **D3D11** but with a slow load time
  * Set **Video / Scaling / Aspect Ratio** to **Full**
    * This will match your monitor aspect aspect ratio
  * Set **Video / Scaling / Integer Scale** to **OFF**
  * Set **Video / Output / Rotation** to **Normal**
  * Set **Settings / Core / Allow Rotation** to **OFF** -- **Important for FB Neo**
  * Load a preset in the shaders menu, Mega Bezel shader presets are found in:
    * shaders/shaders_slang/bezel/Mega_Bezel/Presets
  * When you save a preset make sure you have the **Simple Presets** feature set to **ON**
    * This will save a preset with a reference to the preset you loaded plus whatever parameter changes you made
    * This will keep your presets loading properly when the shader updates in the future



----------------------------------------
Choosing a Preset
----------------------------------------

  * Presets are named/sorted by performance
  * The most flexible and most resource hungry start with index 0.
  * As the name's index number increases the performance of the preset improves but but flexibility decreases.

**Presets in Mega_Bezel / Presets**

- All in the root of the Presets folder use @guest.r's awesome Guest-DrVenom-Advanced CRT shader which is considered the default CRT shader for the Mega Bezel, the only exception to this is the POTATO preset which uses GDV-MINI for performance reasons.

|                       |Reflection  |Image Layering |Tube Effects  |Pre-CRT Chain
|-----------------------|------------|---------------|--------------|----------------
|MBZ__1__ADV            |✔           |✔             |✔             | FULL
|MBZ__2__ADV__Glass     |✔           |               |✔             | FULL
|MBZ__2__ADV-NO-REFLECT |            |✔              |✔             | FULL
|MBZ__3__STD            |✔           |✔             |✔             | SIMPLIFIED
|MBZ__3__STD__Glass     |✔           |               |✔             | SIMPLIFIED
|MBZ__4__STD-NO-REFLECT |            |✔              |✔             | SIMPLIFIED
|MBZ__5__POTATO         |            |BG ONLY        |              | MINIMAL

**Descriptions:**

  * **Glass** 
    * Presets which show a blurry reflection in the area around the screen
  * **Image Layering**
    * Layering of multiple images for background, crt housing, leds etc
  * **Full Pre-CRT shader chain** 
    * Includes Grade, MDAPT, ScaleFX & GTU
    * Resolution is tripled in the middle of the chain for ScaleFX 
  * **Simple Pre-CRT shader chain**
    * Fewer passes
    * Includes Grade
    * No Upscaling in the chain so works better with higher res core output, e.g. 4x Internal Res

**Preset Folders in Mega_Bezel / Presets**

  * **Base_CRT_Presets**
    - Includes presets using different crt shaders for the screen
    - Look here for the LCD preset

  * **Base_CRT_Presets_DREZ**
    * Presets which set the resolution at the beginning of the shader chain
    * Good for reducing the resolution from the core to native res to use with a crt shader
    * Helps working with cores which are outputting at increased internal resolution, e.g. 2x, 4k this creates antialiasing at native res which comes from rendering at a higher resolution then reducing it 

  * **Community_CRT_Variations**
    - Presets with crt settings created by community members

  * **Variations**
    * These presets are simple presets referencing one of the presets in the Mega_Bezel folder
    * They reference the original preset then have adjusted parameters or texture paths

  * **Experimental**
    * These presets are work in progress and therfore 
    * **Use at your own risk!**
    * These may disappear, be renamed, or change behavior at any future release



----------------------------------------
Troubleshooting
----------------------------------------
---

 * **If you have difficulties loading the shader** try loading it with the imageviewer core
    * **Steps**
        * Add the imageviewer core to Retroarch with the online updater
        * Open an image with this core
        * Load the shader
        * If the shader loads correctly then the shader is working.
        
     * When the shader works in imageviewer, but doesn’t work when using a core, it is probably related to the core, or the graphics card being overwhelmed by resource usage with both the core requirements and the shader requirements
     * If it is the core resolution overwhelming the resources, try a STD preset, or a STD-DREZ preset to reduce the resolution used within the shader chain
     * If you still have difficulties loading the shader with a specific core, try updating the core
     * If you still have difficulties download a new separate version of Retroarch and try it there. Sometimes problems lurk in a random config file which is very hard to track down
 * **To see errors** coming from Retroarch you need to set up your logging settings:
    * **Logging - Logging Verbosity - ON**
      * **Frontend Logging - 0(Debug)**
      * **Log to File - Off**
    
* These settings will **cause a log window to come up** when you launch, and you should see any Retroarch errors, and shader loading details

 * **If the Screen is changing size unexpectedly**
    * If the screen changes size when loading a game or switching between different parts of the game e.g. gameplay vs cinematic, this is because of the interaction between the different resolutions the core is outputting on different screens and the shader's integer scale or automatic aspect ratio settings.
    
* **To fix**
  
  * **Make sure Integer Scale is OFF in the RetroArch Video Settings**
  * **If BOTH the HEIGHT and WIDTH of the screen size are changing size**
    
    * Set the Integer scale mode to OFF (0)
  * **If ONLY the WIDTH of the screen is changing size** (the HEIGHT stays constant)
    * Set the Aspect Ratio Type to Explicit (1) This will use the explicit aspect ratio number instead of guessing
    
    * If this solves your issue please consider posting on the thread at the top of this document the issue you had so that we can improve the auto aspect ratio in the future
    
  * Artifacts on the screen that look round swirls or circles which look like tree trunk rings

    * This kind of artifact is called a Moiré pattern - https://en.wikipedia.org/wiki/Moiré_pattern

    * The base cause of the Moiré pattern is usually the curvature, if you set the curvature to 0 the moire should go away

    * Also in general moire artifacts are less of a problem when you are at an integer scale and using a higher resolution monitor.

----------------------------------------
---

Bug Reporting
----------------------------------------

- When reporting a bug, **please post images of the issue if it is at all visual in nature**. This often helps communicate the issue better & quicker, even if the issue seems simple.
- Please make sure you are using the latest version of the shader when reporting bugs
- If you are having any issues with the shader not loading or crashing please include a log (See above for turning on logs)
- Please include info about your setup
  - Preset
  - Core
  - Core Internal Res 1x, 2x etc?
  - Monitor resolution?
  - GPU


-----------------------------------------------------------------------------------------------
----
Parameter Descriptions
-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------
**[ --- HSM MEGA BEZEL X.X.XXX 20XX-XX-XX --- ]:** - Title, Version, Date

  * **Show Resolution Info** --- Show Resolution info from different aspects of the shader chain with onscreen text

-----------------------------------------------------------------------------------------------
**[ GRAPHICS CACHE ]:**

  * **Cache Graphics**
    * **0: OFF** - Graphics Layering and Bezel Generation are regenerated every frame
    * **1: ON** - Graphics & Bezel are generated once and cached for subsequent frames. The cache auto updates when changes in parameters are detected
  * **Cache Update Indicator** 
    * **0: OFF** - Never show the red dot indicator on screen when the cache updates
    * **1: ON** - Appears whenever the graphics are regenerated and cache is updated, when caching is off or if the cache is auto-updated
    * **2: ONLY WHEN CACHE OFF** - Indicator does not appear on auto-update, It only appears when caching is off

-----------------------------------------------------------------------------------------------
**[ GRAPHICS GLOBAL BRIGHTNESS ]:**
  * **Graphics Brightness** - Brightness of all graphics and images which are not the CRT Shader
  * **Graphics Gamma Adjust** - Apply a gamma adjustment on all graphics and images which are not the CRT Shader 

-----------------------------------------------------------------------------------------------
**[ AMBIENT LIGHTING ]:** - Usually used to apply night lighting on all graphics
- **Opacity**
  - How much of the ambient lighting darkening effect is applied
- **Hue**
  - Shift the hue of the color of the image
- **Value**
  - How dark or bright the ambient lighting is
- **Saturation**
  - How saturated the night lighting is
- **Scale Mode**
  - **VIEW WITH ZOOM** Scale to the full view, but scale with the viewport Zoom
  - **FULL** Scale the image to the full window
- **Scale Offset**
  - Scale the lighting image
- **Mirror Horizontal**
  - Mirror the ambient lighting image

-----------------------------------------------------------------------------------------------
**[ VIEWPORT ZOOM ]:** 
* **Viewport Zoom** --- Zoom in or out on all the graphics and screen
* **Viewport Position X** 
* **Viewport Position Y**

-----------------------------------------------------------------------------------------------
**[ FLIP & ROTATE ]:**

* **Flip Viewport Vertical** --- Some cores flip the viewport (full monitor area), this flips it back
* **Flip Viewport Horizontal**
* **Flip Core Image Vertical** --- Some cores flip the core image, this flips it back
* **Flip Core Image Horizontal**
* **Rotate CRT Tube** --- Turns the tube 90 degrees counter counter-clockwise

-----------------------------------------------------------------------------------------------
**[ ASPECT RATIO ]:**

* **Orientation** --- Should the aspect be tall or wide? This does not rotate the image.
  * **0 - Auto** - Chooses vertical vs horizontal based on the core resolution
  * **1 - Horizontal** - uses horizontal aspect
  * **2 - Vertical** - uses vertical aspect

* **Type**
  * **0 - Auto** - Choose aspect ratio based on an educated guess about the core's resolution
  * **1 - Explicit** - Use the aspect ratio from the **Explicit Aspect Ratio** parameter
  * **2 - 4:3** - Almost all arcade games are 4:3
  * **3 - 3:2**
  * **4 - 16:9**
  * **5 - PAR** - The aspect ratio of the core's pixel resolution
  * **6 - Full** - The screen will scale to the full viewport

  * **Explicit Aspect Ratio** - Ratio used when [Aspect Ratio] Type is set to Explicit or if Auto is chosen and no matching resolution can be found

-----------------------------------------------------------------------------------------------
**[ CRT SCREEN SCALING ]:**

- **Show Resolution Info**
- **Int Scale Mode**
  - **0 - Off,** Use Non-Integer Scale
  - **1 - ShortAxis Integer Scale On** - for the viewport (monitor) in landscape mode this is the vertical axis, If the screen/tube aspect ratio is vertical then integer scale is used for both horizontal and vertical axes
  - **2 - Integer Scale on both axes**

- **Int Scale Multiple Offset**
  - Adjusts the size of the screen by increasing the multiple of the core resolution (on both axes) when using integer scale, to make the screen larger or smaller
- **Int Scale Multiple Offset Long Axis**
  - Adds an additional multiple offset but for only the long axis, with a horizontal aspect ratio this is the horizontal axis
- **Int Scale Border Min Height %**
  - The amount of vertical screen space reserved for the area around the screen when using integer scale
- **Vertical Preset (E.G. 4K Vertical)**
  - Turn on if this is a vertical monitor preset, E.G. if your physical monitor is turned vertical
- **Non-Integer Scale %**
  - If integer scale isn't used, this sets the vertical size of the vertical percentage of the full viewport


-----------------------------------------------------------------------------------------------
**[ EXTENDED CRT SCREEN SCALING ]:**

- **Use Image For Placement (Scale & Y Pos)**
  - TODO Description
- **Placement Image Mode: TRANSPARENCY : WHITE ON BLACK**
  - TODO Description
- **Non-Integer Scale Offset**
  - TODO Description
- **Snap To Int-Scale Tolerance**
  - Snaps the non-integer scale to the closest integer scale within the tolerance. This allows you to set an approximate size for the screen and have it snap to an integer scale but not if it would be too much different


-----------------------------------------------------------------------------------------------
**[ POSITION OFFSET ]:**

- **Position X** - Moves the entire screen and frame left and right
- **Position Y** - Moves the entire screen and frame up and down


-----------------------------------------------------------------------------------------------
**[ CROPPING CORE IMAGE ]:** ---
Cropping removes parts of the game image at the edges of the screen which were never meant to be seen. Negative values can add more black area at the edges of the screen

- **Crop Mode  -  OFF | CROP BLACK ONLY | CROP ANY**
- **Crop Zoom %**
- **Crop Overscan Top**
- **Crop Overscan Bottom**
- **Crop Overscan Left**
- **Crop Overscan Right**
- **Black Threshold for 'CROP BLACK ONLY'**


-----------------------------------------------------------------------------------------------
**[ --- FAST SHARPEN - GUEST.R --- ]:**
- **Sharpen ON**
- **Sharpen Strength**
- **Amount of Sharpening**
- **Details Sharpened**


-----------------------------------------------------------------------------------------------
**[ --- SMOOTHING - SCALEFX - ADV Presets Only ---- ]:**

**ScaleFX** applies a shape smoothing on the core image and creates a higher resolution smoothed image
  - **ScaleFx ON** - After you turn this on you must increase **Core Res Sampling**, or **Downsample Blur** in the next section to see a difference


-----------------------------------------------------------------------------------------------
**[ SCANLINE DIRECTION ]:**

- **Scanline Direction (GDV & Easymode)**
  - **0 - Auto** --- Chooses horizontal or vertical scanline direction based on aspect ratio
  - **1 - Horizontal** scanlines
  - **2 - Vertical** scanlines


-----------------------------------------------------------------------------------------------
**[ CORE RES SAMPLING ]:**

**Adjusting core res sampling changes how the CRT perceives the core resolution**
  e.g. If you use a core with 4X internal resolution you can set core res sampling to 0.25 it will appear if it was at 1x resolution
  To use **ScaleFX** to smooth the shapes in the core image you will need to set the core res sampling higher than 1, otherwise you will not see the increased smoothing coming from ScaleFX

- **Scanline Direction Multiplier (X-Prescale for H Scanline)**
  - Adjust the sampling in direction of the scanlines
  - E.G. if the scanlines are horizontal adjust sampling along the horizontal axis
- **Scanline Dir Downsample Blur**
  - Add blur along the scanline direction
- **Opposite Direction Multiplier (Y Downsample for H Scanline)**
  - Adjust the sampling in direction opposite of the scanlines
  - E.G. if the scanlines are horizontal adjust sampling along the vertical axis
- **Opposite Dir Downsample Blur**
  - Add blur along the opposite direction of the scanlines

-----------------------------------------------------------------------------------------------
**[ INTERLACING From Guest.r :) ]:**

- **Interlace and Fake Scanlines Trigger Res**
  - Resolution where the shader should switch into its interlace or high res content mode.
- **Interlacing Mode: OFF | Normal 1-3 | Interpolation 4-5**
- **Interlacing Effect Smoothness**
- **Interlacing Scanline Effect**
- **Interlacing (Scanline) Saturation**

-----------------------------------------------------------------------------------------------
**[ FAKE SCANLINES ]:**

- **Show Fake Scanlines - OFF | ON | USE TRIGGER RES**
- **Opacity**
  - Opacity of scanlines added on top of the crt image.
  - These scanlines are not tied to the core image resolution
- **Scan Resolution**
- **Int Scale Scanlines**
- **Rolling Scanlines**
- **Scanline Curvature**

-----------------------------------------------------------------------------------------------
**[ CURVATURE ]:**
    Applies tube curvature

- **CURVATURE MODE**
  - **0 - Turn Curvature Off**
  - **1 - 2D** - Simplest and fastest curvature
  - **2 - 2D-CYL** - Simplest and fastest curvature but for a cylindrical tube like a Trinitron
  - **3 - 3D Sphere**  -  Geometric projection from the surface of a sphere to the viewport - Same as CRT-Royale
  - **4 - 3D Sphere with adjusted corner mapping** --- Very similar to #1
  - **5 - 3D Cylindrical Mapping** --- Vertically flat curvature like a Trinitron, e.g. PVM or BVM
- **2D Curvature on Long Axis** - Curvature multiple on long axis, for a horizontal aspect ratio this is the horizontal axis
- **2D Curvature on Short Axis** - Curvature multiple on short axis, for a horizontal aspect ratio this is the vertical axis
- **3D Radius** - Radius for the sphere the 3D projection is done on, values from 1-4 then to be useful
- **3D View Distance** - This is the distance of the virtual camera from the Sphere
- **3D Tilt Angle Y** - Vertical Tilt, with split screen this will tilt both screens towards or away from the center

-----------------------------------------------------------------------------------------------
**[ DUAL SCREEN ]:**

- **Dual Screen Mode**
  - **0 - OFF** - Single Screen
  - **1 - VERTICAL** - Split into 2 screens one on the top and one on the bottom
  - **2 - HORIZONTAL** - Split into 2 screens one on the left and one on the right
  
- **Core Image Split Mode**
  - **0 - AUTO** - Split in the same direction as the dual screen mode
  - **1 - VERTICAL**
  - **2 - HORIZONTAL**
- **Core Image Split Offset**
  - Adjusts where we split the core image into two
  - This is an offset in pixels from the center
   the screen 
  - Value in Pixels
- **Viewport Split Offset**
  - Sets where the viewport split placed. The split defines the area where one screen or the other appears
  - Value is a percentage from the center of the screen
- **Scale Screens from Center of Split**
  - 0 - OFF - The screens will scale their center
  - 1 - ON - The screens will scale from the split position rather than from their own centers
- **Position Offset Between Screens**
  - Positive values move screens apart
  - Negative values move screens closer to each other
- **2nd Screen Aspect Ratio Mode**
  - 0 - Use the same Aspect ratio as the first Screen
  - 1 - PAR (Uses the square pixel aspect of the bottom screen's resolution)
- **2nd Screen Use Independent Scale**
  - Don't affect the second screen with the scale of the first
- **2nd Screen Scale Offset**
  - Increase or Decrease scale of second screen
- **2nd Screen Pos X**
  - Move the second screen Horizontally
- **2nd Screen Pos Y**
  - Move the second screen Vertically
- **2nd Screen Crop Zoom %** 
- **2nd Screen Crop Overscan Top**
- **2nd Screen Crop Overscan Bottom**
- **2nd Screen Crop Overscan Left**
- **2nd Screen Crop Overscan Right**


-----------------------------------------------------------------------------------------------
**[ ANTI-FLICKER ]:**

Blend parts of the image which flicker on/off repeatedly between frames often used for Character's Shadow, giving a blended result.

- **Anti-Flicker ON** --- Turn the effect ON / OFF
- **Luma Difference Threshold**
  - Brightness difference required before the colors will be blended

-----------------------------------------------------------------------------------------------
**[ CRT vs ORIGINAL A/B COMPARE ]:**

- **Compare ON**
  - Shows a split screen between the raw game image and the with processing by the crt shader and effects and 

- **Splitscreen Position**
  - Shift the split in the middle of the screen towards the left or right


-----------------------------------------------------------------------------------------------
**[ CRT SCREEN GAMMA & BRIGHTNESS ]:**

- **CRT Gamma In** (Gamma to Linear Space Decode) Def 2.4

- **CRT Gamma Out** (Linear to Gamma Space Encode) Def 2.4

- **Post CRT Brightness** - Brightness adjustment after the CRT effect


-----------------------------------------------------------------------------------------------
**[ SCREEN VIGNETTE ]:**

- **Use Vignette** 
  - Fade out the game screen as we move away from the center of the screen
- **Amount (Strength)** - Overall Darkness
- **Corner Amount (Power)** - Darkeness towards the edges
- **Show Vignette in Reflection** - Darken the reflection or not


-----------------------------------------------------------------------------------------------
**[ MONOCHROME ]:** --- Have the screen act as if it is a monochrome CRT

- **Monochrome Color:** 
  - **0: OFF**
  - **1: BLACK & WHITE**
  - **1: AMBER**
  - **1: GREEN**

- **Monochrome Gamma** 
- **Monochrome Hue Offset** 
- **Monochrome Saturation** 

-----------------------------------------------------------------------------------------------
**[ TUBE DIFFUSE COLOR ]:**

The color/texture of the tube which appears behind the CRT image
- **Opacity** 
  - Opacity of the tube background, With opacity of 0 you will see through to the background, good for things like Tron's Deadly Discs 
- **Use Tube Diffuse Image** 
  - If 0 black is used instead
- **Colorize On** - Colorize the image
- **Hue Offset**
- **Saturation**
- **Brightness**
- **Gamma Adjust**


-----------------------------------------------------------------------------------------------
**[ TUBE COLORED GEL IMAGE ]:**
    Colored effect added on top of the CRT image to tint it

- **Use Tube Colored Gel** - Apply the image or not
- **Multiply Blend Amount** - Image applied like a colored gel in photography
  - Used to make vector games which output black and white colored, E.G Battlezone
- **Additive Blend Amount** - Image applied as a brightening of the tube area
  - Used to add color to the screen as if it was the gel being diffusely lit from outside the monitor
- **Normal Blend Amount** - Used for the more opaque parts of a gel image like for the Vectrex
- **Normal Blend Brightness** - Brightness for these more opaque parts
- **Normal Blend Transparency Threshold** - Adjusts at what transparency of the image the area should be fully transparent
- **Show CRT on Top of Colored Gel Normal** - Put the CRT image on top of the gel image so it is not color shifted or obscured.


-----------------------------------------------------------------------------------------------
**[ TUBE STATIC REFLECTION IMAGE  ]:**
- **Use Tube Static Reflection Image** --- OFF/ON to apply the image
- **Opacity** --- This is the shine on the tube which imitates reflection from the environment
- **Ambient Lighting Multiplier** --- How much of the ambient lighting image to apply, default is 100
- **Scale** --- Scales the tube reflection image from the center of the tube


-----------------------------------------------------------------------------------------------
**[ SCREEN BLACK EDGE ]:**

- **Global Corner Radius** --- Global radius of all corners before their own multipliers are applied
- **Black Edge Corner Radius Scale** --- the roundness of the corner of the screen area
- **Black Edge Sharpness** --- Blends the edge of the game screen image to black, lower values will fade the edge
- **Black Edge Curvature Scale Multiplier** --- How much the black edge will follow the screen curvature
- **Black Edge Thickness** --- Distance between the illuminated game screen and the inner edge of the bezel
  - Set this to 0 or less to remove the black edge


-----------------------------------------------------------------------------------------------
**[ REFLECTION POSITION & SCALE ]:**

- **Screen Reflection Scale**
  - Scales the reflection from the center
  - With a larger scale, the image from the screen will appear without mirroring, like the Big Blur preset
- **Screen Reflection Pos X**
  - Shift the reflection left or right
- **Screen Reflection Pos Y**
  - Shift the reflection up or down


-----------------------------------------------------------------------------------------------
**[ BEZEL INDEPENDENT SCALE & CURVATURE ]:**

- **Use Independent Scale & Curvature**
  - Scale the bezel independent of the screen
- **Independent Scale**
  - Scale of the bezel when scale from image is not used
  - Base scale for the bezel default is 82.97 which is the same as the default screen size
- **Use Independent Curvature** --- Define curvature separately from the screen
- **Independent Curvature X** --- Horizontal curvature for the bezel when independent
- **Independent Curvature Y** --- Vertical curvature for the bezel when independent
- **Inner Curvature Scale Multiplier**
  - Multiplier the curvature of the inner edge of the bezel
  - Default is 100 which will match the tube curvature
  - Multiplies the curvature in normal and independent mode

-----------------------------------------------------------------------------------------------
**[ BEZEL GENERAL ]:**

- **Opacity**
  - At 100 the bezel is fully visible

- **Blend Mode**
  - **0 - Off**
  - **1 - Normal Blending**
  - **2 - Additive Blending** - Added on as added with a projector
  - **3 - Multiply Blending** - Image is applied by darkening the under layer

- **Width**
  - Thickness of the bezel on the sides of the tube, default is 125

- **Height**
  - Thickness of the bezel on the top and bottom of the tube, default is 100

- **Scale Offset**
  - Scale offset of the Bezel & Frame from its default position

- **Inner Curvature Scale Multiplier**
  - How much the bezel's curvature follows the screen curvature

- **Inner Corner Radius Scale** - Def 50
  - Roundness of the inner corner of the bezel, it is a multiplier of the roundness of the screen corner
  - 100 gives you the same roundness as the screen corner

// TODO review subsequent parameter help

- **Inner Edge Thickness**
  - Thickness of edge of inner, default 100

- **Inner Edge Sharpness** - Def 90

- **Outer Corner Radius Scale** - Def 100
  - Roundness of the inner corner of the bezel, it is a multiplier of the roundness of the screen corner
  - 100 gives you the same roundness as the screen corner

- **Outer Curvature Scale**
  - Amount of curvature on the outside of the bezel it is a multiplier of the roundness of the screen corner
  - Default is 0 which gives a straight edge of the outside of the bezel

- **Outer Edge Position Y**
  - This moves the outer edge of the bezel and the frame up and down

- **Brightness**
  - Brightness of the bezel, the default is 30 so only 30% brightness

- **Highlight**
  - The highlight or shininess in the middle of the bezel

- **Opacity of Shadow from Bezel on Tube**
  - How much of a darkness from the bezel onto the illuminated screen
  - Only visible when the black ring around the screen is reduced so that the bezel is almost on top of the screen

-----------------------------------------------------------------------------------------------
**[ BEZEL & FRAME COLOR ]:**

- **Hue**
  - The hue or "color" of the bezel and frame E.G. Blue vs Orange
- **Saturation**
  - How saturated or strong the color is
- **Value/Brightness**
  - The brightness of the color, default is 10 which is 10%
- **Noise**
  - Noise or speckles in the color, default is 30

-----------------------------------------------------------------------------------------------
**[ FRAME ]:**

- **Opacity**
  - Opacity of the frame default is 100 which means it is fully visible
- **Blend Mode** - Default is 1
  - **0 - Off**
  - **1 - Normal Blending**
  - **2 - Additive Blending** - Added on as added with a projector
  - **3 - Multiply Blending** - Applied by darkening what is underneath
- **Texture Overlay Opacity (Highlight)**
  - Opacity of the texture applied on top of base color of the frame
  - The default texture is a white highlight and so adds a highlight effect to the frame giving it a bit more dimension
- **Texture Overlay Blend Mode** - Default is 2 so it is additive
  - **0 - Off**
  - **1 - Normal Blending**
  - **2 - Additive Blending** - Added on as added with a projector
  - **3 - Multiply Blending** - Applied by darkening what is underneath
- **Inner Edge Thickness**
  - Thickness of the inner edge of the frame
- **Inner Corner Radius Scale**
  - Roundness of the inner corner, 
- **Thickness**
  - Base thickness of the frame
- **Thickness Scale X**
  - Adjusts the frame thickness of frame at the left and right
- **Outer Pos Y**
  - Shift the outside of the frame up and down which can make the top of the frame larger than the bottom or vice versa 
- **Outer Curvature Scale**
  - Curvature of the outside of the frame, at 100 it will match the curvature of the inside of the frame
- **Outer Corner Radius**
  - Roundness of the frame outer corner
- **Outer Edge Thickness**
  - Thickness of the shading on the outer edge
- **Outer Edge Shading**
  - Controls the darkness of the shading on the outer edge
- **Shadow Opacity**
  - Controls the darkness of the shadow under and around the frame
- **Shadow Width**
  - Controls how wide the shadow is around the frame

-----------------------------------------------------------------------------------------------
**[ REFLECTION ]:**

- **Blend Mode** - How the reflection is applied to the layer underneath Default is 2
  - **0 - Off** - The layer is not shown
  - **1 - Normal Blending**
  - **2 - Additive Blending** - Applied additively to brighten what's underneath
- **Mask** - Mask the reflection with the area inside the:
  - **0 - ALL** - Whole viewport
  - **1 - Screen** - Illuminated area of the tube
  - **2 - Tube**
  - **2 - Bezel and Inward** - Bezel and inward
  - **3 - Bezel**
  - **4 - Bezel +** - Bezel and outward
  - **5 - Frame**
  - **6 - Frame +** - Frame and outward
  - **7 - Background**  - Outside the frame
- **Global Amount**
  - Overall multiplier on the amount of reflection shown
- **Global Gamma Adjust**
  - Gamma adjustment on the reflection, allows you to reduce the amount of reflection in dark areas, or reduce contrast in the reflections
- **Direct Reflection**
  - Amount of the most detailed reflection shown
- **Diffused Reflection**
  - Amount of a very blurry and diffused reflection shown, helps blend between the main reflection to make a more natural effect
- **FullScreen Glow**
  - Amount of a very diffused reflection shown which mimics lighting from the overall brightness of the screen
- **FullScreen Glow Gamma**
  - Adjust the gamma of the full screen glow, this has the effect of controlling how bright the screen needs to be to see the fullscreen glow effect 
- **Bezel Inner Edge Amount**
  - How much reflection on the small inner edge right at the outside of the tube
- **Bezel Inner Edge Fullscreen Glow**
  - Same as above but a non-directional glow from all over the screen
- **Frame Inner Edge Amount**
- **Frame Inner Edge Sharpness**
  - How soft or sharp the reflection is at the inner edge of the frame

-----------------------------------------------------------------------------------------------
**[ REFLECTION FADE ]:**

- **Fade Amount**
  - At 100 the reflection fades out as it comes away from the screen, at 0 the reflection does not fade and is full strength everywhere  
- **Radial Fade Width**
  - The distance away from the sides of the screen where the reflection to completely fades out
- **Radial Fade Height**
  - The distance away from the top and bottom of the screen where the reflection to completely fades out
- **Lateral Outer Fade Position**
  - When the reflection fades out towards the corners, for example on the bottom bezel the reflection fades out towards the left and right. The position where the fade starts.
- **Lateral Outer Fade Distance**
  - For the lateral fade the distance for it to fade out
- **Viewport Vignette (For Glass Presets)**
  - Adds a vignette over the entire viewport to darken the areas as it goes towards the edges used to darken the reflection in the glass preset

-----------------------------------------------------------------------------------------------
**[ REFLECTION CORNER ]:**

- **Corner Fade**
  - How much should the corner fade out
- **Corner Fade Distance**
  - The distance from the corner where the reflection fully fades out
- **Corner Inner Spread**
  - How much the inner corner reflection spreads out
- **Corner Outer Spread**
  - How much the outer corner reflection spreads out
- **Corner Rotation Offset Top**
  - Adjust the rotation of the highlight in the top corners
- **Corner Rotation Offset Bottom**
  - Adjust the rotation of the highlight in the bottom corners
- **Corner Spread Falloff**
  - Controls the profile of the falloff, small values make falloff faster near the center. 

-----------------------------------------------------------------------------------------------
**[ REFLECTION BLUR ]:**

- **Blur Samples - 0 for OFF**
  - Default is 12
- **Min Blur**
  - What is the least amount of blur in the reflection this is nearest the screen 
- **Max Blur**
  - The highest amount of blur in the reflection, this is the farther away from the screen

-----------------------------------------------------------------------------------------------
**[ REFLECTION NOISE ]:**

- **Noise Amount**
  - How much noise seen in the reflection, gives the effect of the scattered reflection of a slightly textured surface 
- **Noise Samples (0 for OFF)**
  - How many samples taken for the effect, more samples the smoother the effect, fewer samples makes the surface look more like it has little bumps in it
- **Sample Distance**
  - What is the farthest distance away from the point being drawn where the scattered sample come from

-----------------------------------------------------------------------------------------------
**[ GLASS BORDER ]:**

- **Glass Border ON (Glass Preset Only)**
  - Changes the appearance of the reflection to look like the glass effect, this is here for technical reasons, not very useful to change interactively




## **BASIC-BORDER Presets Only**

-----------------------------------------------------------------------------------------------
**[ BASIC BACKGROUND IMAGE LAYER ]:**

- **Aspect Ratio**

  - **Full** - Image fills the viewport
  - **16:9** - For landscape aspect images
  - **9:16** - For portrait aspect images

- **Background Image Opacity**

- **Background Blend Mode** - Default is Additive

  - **Off** - Image is not applied
  - **Normal**
  - **Additive** - Adds the image as if it is being projected on top
  - **Multiply** - Image is applied as if it was a colored plastic film

- **Background Show over Screen**

  - Background is applied on top of the screen

  - Since the default blend mode is additive this gives a backdrop effect

    

## **GLASS Presets Only**

-----------------------------------------------------------------------------------------------
**[ GLASS IMAGE LAYER ]:**

- **Background Image Opacity**

- **Background Blend Mode** - Default is Additive

  - **Off** - Image is not applied

  - **Normal**

  - **Additive** - Adds the image as if it is being projected on top

  - **Multiply** - Image is applied as if it was a colored plastic film

    

## **STANDARD Presets Only**

-----------------------------------------------------------------------------------------------
**[ STANDARD IMAGE LAYERS ]:**

- **Aspect Ratio**
  - **Full** - Image fills the viewport
  - **16:9** - For landscape aspect images
  - **9:16** - For portrait aspect images
- **Background Image Opacity**
- **Background Blend Mode** - Default is Additive
  - **Off** - Image is not applied
  - **Normal**
  - **Additive** - Adds the image as if it is being projected on top
  - **Multiply** - Image is applied as if it was a colored plastic film
- **Background Show over Screen**
  - Background is applied on top of the screen
  - Since the default blend mode is additive this gives a backdrop effect
- **LED Image Opacity**
- **LED Blend Mode**
- **Top Extra Image Opacity**
- **Top Extra Image Blend Mode**


-----------------------------------------------------------------------------------------------
**[ LAYER ORDER ]:**

Layer order adjusts the order in which the layers are composited or "layered" on top of each other, the index 0 is the bottom or base layer. If two layers are given the same index they fall back to being composited in the order seen here.

- **Background Image**
- **Viewport Vignette**
- **CRT Screen**
- **Bezel Image**
- **Bezel (Generated)**
- **Frame (Generated)**
- **Cabinet Glass Image**
- **Decal Image**
- **Reflection (Generated)**
- **LED Image**
- **Top Image**

-----------------------------------------------------------------------------------------------
**[ CUTOUT ]:**

Used to cut a rectangular area from the layers, for example cutting out the hole in the bezel art

- **Scale Mode**
  - Controls if this layer's scaling follows another layer 
  - **0 - Full** - Scale to the viewport 
  - **1 - Tube** - Follow the Tube Scaling
  - **2 - Bezel** - Follow the Bezel Scaling
  - **3 - Background** - Follow the Background Image Scaling
  - **4 - Bezel Image** - Follow the Bezel Image Scaling
  - **5 - Decal Image** - Follow the Decal Image Scaling
- **Scale**
  - Scales cutout in both directions
- **Scale X**
  - Scales cutout horizontally
- **Position Y**
  - Moves the cutout vertically
- **Corner Radius - Def 0**
  - Rounds the corner of the cutout

-----------------------------------------------------------------------------------------------
**[ MASK DEBUG ]:**

- **Mask** - Show the mask as a semi transparent color for the:
  
  - **-1 - Cut Out**
  - **0 - ALL** - Whole viewport
  - **1 - Screen** - Illuminated area of the tube
  - **2 - Tube**
  - **2 - Bezel and Inward** - Bezel and inward
  - **3 - Bezel**
  - **4 - Bezel +** - Bezel and outward
  - **5 - Frame**
  - **6 - Frame +** - Frame and outward
  - **7 - Background**  - Outside the frame
  
  

## ***Common Layer Parameters***

	*In the multiple image layers there are many parameters which are repeated from layer to layer, their description is shown here*

- ***Opacity***

  - *Opacity multiplier of the layer being applied. 0 means we will not see the layer because it is fully transparent*

- ***Blend Mode*** *- How the reflection is applied to the layer underneath Default is Additive*

  - ***0 - Off*** *- The layer is not shown*
  - ***1 - Normal Blending***
  - ***2 - Additive Blending*** *- Applied additively to brighten what's underneath*

- ***Source Matte Color***

  - *Controls how the image's is interpreted based on the matte color used when the image was stored (what color the image is blended with in the transparent area).* 
  - *Used to remove white fringing on the edges around transparent areas.*
  - ***0 - Black***
    - *The color in the transparent area was black*. Technically this is called Premultiplied alpha. 
  - ***1 - White***
    - *The color in the transparent area was white, Use this if you see white fringing on the edges of the transparency where there should be none.*
  - ***2 - None***
    - *The image was not blended with any matte color, the only transparency info is in the alpha channel*

- **Mask** - Mask the layer with the area inside the:

  - ***0 - ALL*** *- Whole viewport*
  - ***1 - Screen*** *- Illuminated area of the tube*
  - ***2 - Tube***
  - ***2 - Bezel and Inward*** *- Bezel and inward*
  - ***3 - Bezel***
  - ***4 - Bezel +*** *- Bezel and outward*
  - ***5 - Frame***
  - ***6 - Frame +*** *- Frame and outward*
  - ***7 - Background***  *- Outside the frame*

- ***Cutout Mask***

  - ***0 - OFF*** *- Don't cut out any area of the layer*
  - ***1 - ON*** *- Make the area of the layer INSIDE the cutout mask transparent*
  - ***2 - Invert*** *- Make the area of the layer OUTSIDE the cutout mask transparent*

- ***Brightness***

  - *Adjust Brightness of the Layer, 100 is no change*

- ***Texture Aspect***

  - *Aspect Ratio of the texture file used for this layer*
  - ***0 - Full*** *- Use Viewport Aspect*
  - ***Explicit*** *- Use Explicit Aspect*
  - ***4:3*** *- Landscape*
  - ***3:4*** *- Portrait*
  - ***16:9*** *- Landscape*
  - ***9:16*** *- Portrait*

- **Explicit Texture Aspect**

  - *The Aspect ratio used when Texture Aspect is set to Explicit*

- ***Scale Mode***

  - *Controls if this layer follows another layer's scaling* 
  - ***Full / Fullscreen*** *- Scale to the viewport*
  - ***Tube*** *- Follow the Tube Scaling*
  - ***Bezel*** *- Follow the Bezel Scaling*
  - ***Background*** *- Follow the Background Image Scaling*
  - ***Bezel Image*** *- Follow the Bezel Image Scaling*
  - ***Decal Image*** *- Follow the Decal Image Scaling*
  - ***Top Extra Image*** *- Follow the Top Extra Image Scaling*

- **Keep Aspect**

  - *The Layer image should keep its original aspect ratio*
  - *E.G. If the scale mode was **Tube** and **Keep Aspect** was on regardless of the aspect of the tube the layer's image would stay the aspect ratio of the image. If Keep Aspect is off the texture would react to the tube's changes in horizontal aspect.* 
  - **0 - OFF**
  - **1 - ON**

- **Scale** --- *Scales image layer equally in both directions*
- **Scale X** --- *Scales image layer horizontally*
- **Position Y** --- *Moves the image layer vertically*

    

-----------------------------------------------------------------------------------------------
**[ BACKGROUND LAYER ]:**

- **Opacity**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Brightness**
- **Apply NightLight & Static Gamma (0 for Backdrop)**
  - Allows Night Lighting and Static gamma to be turned off
  - Can be used to simulate a backdrop which is self illuminated
- **Scale Mode**
  - **0 - FullScreen**
  - **1 - Tube**
  - **2 - Bezel**
- **Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**
- **Mirror Wrap**
  - When ON the image is wrapped when we draw out of the texture bounds
  - **0 - OFF**
  - **1 - ON**

-----------------------------------------------------------------------------------------------
**[ VIEWPORT VIGNETTE LAYER ]:**

- **Opacity**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - Full**
  - **1 - Background**
  - **2 - Tube**
  - **3 - Bezel**
  - **4 - Decal**
  - **5 - Top Image**
- **Scale**
- **Scale X**
- **Position Y**

-----------------------------------------------------------------------------------------------
**[ CRT SCREEN LAYER ]:**

- **Blend Mode**

-----------------------------------------------------------------------------------------------
**[ BEZEL IMAGE LAYER ]:**

- **Opacity**
- **Hue Offset**
- **Saturation**
- **Brightness**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - FullScreen**
  - **1 - Tube**
  - **2 - Bezel**
  - **3 - Background**
- **Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**
- **Use Curvature**
  - If Use Curvature is ON then the layer image will follow the screen curvature

-----------------------------------------------------------------------------------------------
**[ CABINET OR CABINET GLASS LAYER ]:**

- **Opacity**
- **Brightness**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - FullScreen**
  - **1 - Tube**
  - **2 - Bezel**
  - **3 - Background**
  - **4 - Top Image**
- **Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**

-----------------------------------------------------------------------------------------------
**[ DECAL LAYER ]:**

- **Opacity**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - Full Screen**
  - **1 - Tube**
  - **2 - Bezel**
  - **3 - Background**
  - **4 - Bezel Image**
- **Scale Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**

-----------------------------------------------------------------------------------------------
**[ LED LAYER ]:**

- **Opacity**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - Full Screen**
  - **1 - Tube**
  - **2 - Bezel**
  - **3 - Background**
  - **4 - Bezel Image**
  - **5 - Decal Image**
- **Scale Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**

-----------------------------------------------------------------------------------------------
**[ TOP EXTRA LAYER ]:**

- **Opacity**
- **Blend Mode**
- **Source Matte Color**
- **Mask**
- **Cutout Mask**
- **Scale Mode**
  - **0 - Full Screen**
  - **1 - Tube**
  - **2 - Bezel**
  - **3 - Background**
  - **4 - Bezel Image**
  - **5 - Decal Image**
- **Scale Keep Aspect**
- **Scale**
- **Scale X**
- **Position Y**
- **Mirror Wrap**
  - When drawing past the edges of the texture use mirror wrapping

-----------------------------------------------------------------------------------------------
**[ INTRO SEQUENCE ]:**
	Animation sequence which plays when the content starts up, animation times are in frames. The frame rate for most games 60 fps

- **Show Intro**
  - 0 - OFF
  - 1 - When Content Loads
  - 2 - Repeat
- **Speed**
  - Overall speed of the entire intro. 1 is full speed

-----------------------------------------------------------------------------------------------
**[ INTRO LOGO ]:**

- **Logo Blend Mode**
  - 0 - Off
  - 1 - Normal Blending
  - 2 - Additive Blending - Added on as added with a projector
  - 3 - Multiply Blending - Image is applied by darkening the under layer
- **Logo Over Solid Color**
  - 0 - Off - The Logo is layered under the solid color
  - 1 - ON - The Logo is layered over the solid color
- **Logo Height (0 for exact resolution)**
- **Logo Res X**
  - X Resolution of the logo image
- **Logo Res Y**
  - Y Resolution of the logo image
- **Logo Placement **
  - 0 - Middle
  - 1 - Top Left
  - 2 - Top Right
  - 3 - Bottom Left
  - 4 - Bottom Right
- **Logo Wait Before Start Frames**
  - How many frames before it starts to fade in
- **Logo Fade In Frames**
  - How many frames to fade in
- **Logo Hold Frames**
  - How many frames to hold the image at full opacity before the fade out starts
- **Logo Fade Out Frames**
  - How many frames to fade out

-----------------------------------------------------------------------------------------------
**[ INTRO SOLID COLOR ]:**

- **Solid Color Blend Mode**
  - 0 - Off
  - 1 - Normal Blending
  - 2 - Additive Blending - Added on as added with a projector
  - 3 - Multiply Blending - Image is applied by darkening the under layer
- **Solid Color Hue**
- **Solid Color Saturation**
- **Solid Color Value**
- **Solid Color Hold Frames**
  - How many frames to hold the solid color
- **Solid Color Fade Out Frames**
  - How many frames to fade out

  **[ INTRO STATIC NOISE ]:**

- **Static Noise Blend Mode**
  - **0 - Off**
  - **1 - Normal Blending**
  - **2 - Additive Blending** - Added on as added with a projector
  - **3 - Multiply Blending** - Image is applied by darkening the under layer
- **Static Noise Hold Frames**
  - How many frames to hold the static noise
- **Static Noise Fade Out Frames**
  - How many frames to fade out

-----------------------------------------------------------------------------------------------
**[ INTRO SOLID BLACK ]:**

- **Solid Black Hold Frames**
  - How many frames to hold the solid black before the fade out starts
- **Solid Black Fade Out Frames**
  - How many frames to fade out

