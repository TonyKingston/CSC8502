WASD to move camera. Shift and Space to move up and down respectively.
The camera starts in auto mode and goes to a few waypoints, press L to toggle between this mode.

The scene contains the following:

An island terrain generated using a heightmap and indexed rendering.
Different textures and bump maps are applied to the terrain depending on height. These textures are mixed together at the height boundaries.
A scene graph that manages the various objects present in the scene. Frustum culling is applied before drawing the nodes.
A skybox rendered using a cube map.
Meshes for rocks and trees (which are transparent) rendered multiple times. The trees use instanced rendering and their transforms are sent to the shader via a uniform buffer object.
An animated Golem mesh.
Three lights, one direction (white), one point (red) and one spot (blue). The lights use a more complex constant-linear-quadratic falloff attenuation
The terrain and the meshes correctly interact with these lights, with diffuse, specular and ambient calculations (there appears to be something wrong with the tree's normal map but I haven't had time to get a different mesh)
The scene has water which is created using projective texturing. Two framebuffers are used to render the scene above and below the water. A dudv map is sampled to create a distortion effect and the appearance of waves.
Fresnel, the water gets less reflective depending on the angle you are looking at it.

A video should be available at: https://github.com/TonyKingston/8502Video