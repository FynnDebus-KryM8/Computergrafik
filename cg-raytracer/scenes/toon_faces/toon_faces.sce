# camera: eye-point, look-at-point, up, fovy, width, height
camera  0 16 50  0 12 -1  0 1 0  30  600 300

# recursion depth
depth  1

# background color
background 0.5 0.7 1.0

# global ambient light
ambience   0.2 0.2 0.2

# light: position and color
light 5 20 0 1.0 1.0 1.0 25

# meshes: filename, shading, material (ambient, diffuse, specular, shininess)
mesh neutral.obj   PHONG  0.2 0.2 0.2  0.9 0.9 0.4  1.0 1.0 1.0  30.0    0.0
mesh sad.obj PHONG  0.2 0.2 0.2  0.9 0.5 0.1  1.0 1.0 1.0  30.0    0.0
mesh confused.obj PHONG  0.2 0.2 0.2  0.9 0.2 0.2  1.0 1.0 1.0  30.0    0.0
mesh smile.obj PHONG  0.2 0.2 0.2  0.2 0.2 0.7  1.0 1.0 1.0  30.0    0.0
mesh kiss.obj PHONG  0.2 0.2 0.2  0.7 0.2 0.7  1.0 1.0 1.0  30.0    0.0
mesh puff.obj PHONG  0.2 0.2 0.2  0.2 0.7 0.7  1.0 1.0 1.0  30.0    0.0

# planes: center, normal, material
plane  0 0 0  0 1 0  0.2 0.9 0.2  0.2 0.9 0.2  0.0 0.0 0.0  100.0  0.1
