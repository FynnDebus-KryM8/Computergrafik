# camera: eye-point, look-at-point, up, fovy, width, height
camera  0 16 50  0 12 -1  0 1 0  30  600 300

# recursion depth
depth  1

# background color
background 0.0 0.0 0.0

# global ambient light
ambience   0.2 0.2 0.2

# light: position and color
light 10 20 0 0.4 0.4 0.4 25
light -10 20 0 0.4 0.4 0.4 25
light -10 40 -10 0.4 0.4 0.4 25

# models and textures are from http://roestudios.co.uk/project/3d-pokemon-models
# meshes: filename, shading, material (ambient, diffuse, specular, shininess, mirror)

mesh umbreon.obj PHONG 0.2 0.2 0.2  0.3 0.3 0.3  0.0 0.0 0.0  30.0    0.0
mesh pichu.obj PHONG  0.2 0.2 0.2  0.9 0.9 0.1  0.2 0.2 0.2  10.0    0.0
mesh magnemite.obj PHONG 0.2 0.2 0.2  0.2 0.2 0.2  1.0 1.0 1.0  50.0    0.2
mesh absol.obj PHONG  0.2 0.2 0.2  0.8 0.8 0.8  0.1 0.1 0.1  10.0    0.0
mesh ground.obj FLAT 0.2 0.2 0.2  0.5 0.35 0.2  0.2 0.2 0.2  10.0    0.0
mesh sky.obj FLAT 0.3 0.3 0.3  0.0 0.0 0.0  0.0 0.0 0.0  10.0    0.0
