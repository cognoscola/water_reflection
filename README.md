# Water

This is my first attempt at making water using OpenGL. I learned by following YouTube user ThinMatrix's tutorial on water (https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh). 

See what it looks like here: 
https://www.instagram.com/p/_AviB3NgeN/

Install:
- copy the entire project to any directory
- Go to install directory in a terminal
- type without quotes: "cmake ." to generate a makefile
- type without quotes: "make" to generate an executable
- run executable.

Controls
- a,s,d,w    - move camera
- mouse      - orient camera
- spacebar   - take a snapshop (will freeze for a few seconds)
- p          - record 10 seconds worth of frames at 25fps

In Summary, here are the steps taken to create the Basic water effect:

1. Create a quad on the scene with the normal (0, 1, 0) and keep track of the height.
2. Create two FrameBuffer objects each with color and depth attachments. One will hold the reflection texture of the scene and the other will hold the refraction texture. 
3. Render to the reflection frame buffer making sure to clip out anything that is below the water. Here we must move the camera below the water and have it point upwards to towards the sky.  
4. Render to the refraction frame buffer making sure to clip out anything that is above the water.
5. Render the scene one more time to the default frame buffer, this time drawing the water quad. Make sure to give the quad the two refraction/reflection textures.
6. In the vertex shader convert the quad coordinates to clip space coordinates and pass that to fragment shader. 
8. In the fragment shader, use projective texture mappings by converting the water quads clip space coordinate system to normalized device space coordinate system (convert using perspective division on the clip space coodinates ) 
7. convert the result mappings to UV coordinate system ( x/2 + 0.5). This will ensure that the reflection and the refraction textures are properly rotated and aligned with the scene 

Adding Light Effects(distortions, specular highlights, depth effects and Fresnel effects):

Distortions of light made from the waves:

1. create 1 texture object from a distortion map file and pass that to the water fragment shader
2. create 1 uniform float in the fragment shader and updated every frame. Use this value to calculate distortions values in the water. 
3. Add the distortion value to the reflection and the refraction texture coordinates. 

Specular Hightlights

1. create 1 texture object from a normal map file and pass that to the water fragment shader
2. Also pass the coordinates of the camera and the light source to the vertex shader. We can use these to create 2 vectors, one which points from the light source to the water and one that points from the water to the camera's position.
3. Using the first vector (from light to water) and using the normal map, calculate the reflected light vector and its strength. 
4. Use the reflected light vector and the vector from the water to the camera to calculate the specular highlight strength. 
5. add this resulting strenth to a colour vector and then add the colour vector to the result colour output. 

Fresnel Effect: 

1. Calcualte the dot product between the water's normal vector and the vector between the water and the camera. This will be the refractive factor. 
2. Using the mix function in the water shader, interpolate between the refractive texture and the reflective texture with the refractive factor as the weight between them. 

Depth effects: 

1. Create a depth texture attachment on the refractive frame buffer and get the texture. Pass this texture to the water shader. 
2. calculate the water depth ( distance between the water's height and the floor below the water) 
3. change the fragment output's alpha value based on this value. The deeper the water, the smaller the value of alpha. 








