# camera: eye, center, up, fovy, width, height
camera -15 0 0  0 0 0  0 1 0  45  600 400

# recursion depth
depth  5

# background color
background 0 0 0

# global ambient light
ambience   0.2 0.2 0.2

# light: position and color
light  0  0 0   0.55 0.55 0.55

# spheres: center, radius, material
sphere  0.0 0.0 -1.0  0.5  1.0 0.0 0.0  1.0 0.0 0.0  1.0 1.0 1.0  100.0  0.0 
sphere  0.0 0.0 4.1  3.0  0.0 1.0 0.0  0.0 1.0 0.0  1.0 1.0 1.0  200.0  0.2
sphere  0.0 0.0 -20.0  17.0  0.0 0.0 1.0  0.0 0.0 1.0  1.0 1.0 1.0   50.0  0.0

