import numpy as np
from PIL import Image
import os
import pyvista as pv
import numpy as np

def read_pgm_to_numpy(file_path):
    """Read a single PGM file into a NumPy array."""
    return np.array(Image.open(file_path))

def stack_slices_to_volume(directory, file_pattern, num_slices):
    """Stack multiple PGM slices to form a 3D volume.
    
    Args:
        directory (str): Directory where the PGM files are stored.
        file_pattern (str): A pattern to generate file names, e.g., 'slice{}.pgm'.
        num_slices (int): Number of slices to read and stack.
    
    Returns:
        np.ndarray: A 3D numpy array representing the stacked volume.
    """
    slices = []
    for i in range(0, num_slices):
        file_path = os.path.join(directory, file_pattern.format(i))
        slice_array = read_pgm_to_numpy(file_path)
        slices.append(slice_array)
    
    return np.stack(slices, axis=0)

# Usage:
directory = 'C:/Users/kearn/OneDrive/Documents/EEE/IndividualProject/Final_Code/data' # Change this to your directory
file_pattern = 'strawberry_{}.pgm'  # File pattern
num_slices = 240  # Amount of slices

volume = stack_slices_to_volume(directory, file_pattern, num_slices)
print(f"Volume shape: {volume.shape}")

# 'Volume' is 3D NumPy array
grid = pv.UniformGrid()

# Set the grid dimensions to the shape of the volume
grid.dimensions = np.array(volume.shape) + 1

# Set the cell data to the volume
grid.cell_data["values"] = volume.flatten(order="F")  # Flatten in Fortran order

# Visualize the volume using PyVista's plotting capabilities
plotter = pv.Plotter()
plotter.add_volume(grid, cmap="Reds")  # Customize the colormap as needed e.g. "Greys"
plotter.show()
