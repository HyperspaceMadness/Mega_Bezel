 * The Wildcard Replacement feature replaces text wildcards found in paths 
 * in the presets with live values coming from retroarch or the preset context
 * 
 * Possible wildcards/tokens to be replaced:
 * 
 *   $CONTENT-DIR$ -> Content Directory of the game rom
 * 
 *   $CORE$       -> Core name
 * 
 *   $GAME$       -> Game ROM's name
 * 
 *   $VID-DRV$   -> Video Driver: Currently active driver, possible replacement values:
 *       glcore
 *       gl
 *       vulkan
 *       d3d11
 *       d3d9_hlsl
 *       N/A
 * 
 *   $VID-DRV-SHADER-EXT$   -> Video Driver Shader File Extension: The extension of shaders type supported by the current video driver:
 *       cg
 *       glsl
 *       slang
 * 
 *   $VID-DRV-PRESET-EXT$   -> Video Driver Preset File Extension: The extension of shaders type supported by the current video driver:
 *       cgp
 *       glslp
 *       slangp
 * 
 *   $CORE-REQ-ROT$   -> Core Requested Rotation: Rotation the core is requesting, possible replacement values:
 *       CORE-REQ-ROT-0
 *       CORE-REQ-ROT-90
 *       CORE-REQ-ROT-180
 *       CORE-REQ-ROT-270
 * 
 *   $VID-ALLOW-CORE-ROT$   -> Video Allow Core Rotation: Reflect's Retroarch's setting allowing the core requested rotation to affect the final rotation:
 *       VID-ALLOW-CORE-ROT-OFF
 *       VID-ALLOW-CORE-ROT-ON
 * 
 *   $VID-USER-ROT$   -> Video User Rotation: Rotation the core is requesting, possible replacement values, does not affect the UI:
 *       VID-USER-ROT-0
 *       VID-USER-ROT-90
 *       VID-USER-ROT-180
 *       VID-USER-ROT-270
 * 
 *   $VID-FINAL-ROT$   -> Video Final Rotation: Rotation which is the sum of the user rotation and the core rotation if it has been allowed, does not affect the UI:
 *       VID-FINAL-ROT-0
 *       VID-FINAL-ROT-90
 *       VID-FINAL-ROT-180
 *       VID-FINAL-ROT-270
 * 
 *   $SCREEN-ORIENT$   -> Screen Orientation: User adjusted screen orientation, will change windows from landscape to portrait, including the Retroarch UI:
 *       SCREEN-ORIENT-0
 *       SCREEN-ORIENT-90
 *       SCREEN-ORIENT-180
 *       SCREEN-ORIENT-270
 * 
 *   $VIEW-ASPECT-ORIENT$   -> Viewport Aspect Orientation: Orientation of the aspect ratio of the retroarch viewport
 *       VIEW-ASPECT-ORIENT-HORZ
 *       VIEW-ASPECT-ORIENT-VERT
 * 
 *   $CORE-ASPECT-ORIENT$   -> Core Aspect Orientation: Orientation of the aspect ratio requested by the core
 *       CORE-ASPECT-ORIENT-HORZ
 *       CORE-ASPECT-ORIENT-VERT
 * 
 *   $PRESET_DIR$  -> Preset directory's name
 * 
 *   $PRESET$     -> Preset's name
 * 
 * If no wildcards are found within the path, or the path 
 * after replacing the wildcards does not exist on disk,
 * the path will be left unaffected, and feedback will be logged 
 * showing what the wildcard was attempted to be replaced with

 * The presets in this folder each test one wildcard
 * To Use
   * Add the Image Viewer
   * Add the 