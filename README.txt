WASD to move camera. Shift and Space to move up and down respectively.

The scene contains the following:

An island terrain generated using a heightmap and indexed rendering.
Different textures and bump maps are applied to the terrain depending on height. These textures are mixed together at the height boundaries.
A scene graph that manages the various objects present in the scene. Frustum culling is applied before drawing the nodes.
A skybox rendered using a cube map.