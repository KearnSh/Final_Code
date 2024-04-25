# Final_Code
# Overview
There are two programs in this repository, StrawberryBedGeneration.cpp and pgm3D.py, and a folder data.
StrawberryBedGeneration.cpp generates a 3D digital model of farm and converts this model into a set of 2D 'slices' in PGM format and saves these in data. 
pgm3D.py compiles the 2D slices into a 3D array and visualises the models in 3D using PyVista.
To run the code, download the folder named 'Final_Code' and follow the steps bellow:

# How to run StrawberryGeneration.cpp and pgm3D.py

# StrawberryGeneration.cpp:
1) Open command prompt (type "cmd" in windows search) or terminal and input your Final_Code folder directory (the location of the folder Final_Code in your files)  e.g.:
   ```
   cd C:\Users\your_name\Documents\Final_Code
   ```
   
2) Compile the code:
   ```
   g++ StrawberryBedGeneration.cpp -o program.exe
   ```
 
3) Run the code.

   In terminal:
   ```
   ./program.exe
   ```
   In command line:
   ```
   program.exe
   ```
   
5) Input how many strawberries you would like to generate and what maturity level when asked (e.g. 20, and Random) and allow to run to completion, it should look like this in the terminal or command window:
   ```
   How Many strawberries would you like to generate? (recommended between 5 and 30) 20

   What level of maturity strawberries would you like to produce? (Random, Mature, Immature, Medium) Random

   ...
   Strawberries Generated:
   1
   2
   3
   4
   5
   6
   7
   8
   9
   10
   11
   12
   13
   14
   15
   16
   17
   18
   19
   20
   21
   22
   23
   24
   25
   26
   27
   28
   29
   30
   ...
   Grid Generated!
   ...
   Converting to PGM...
   Converted!
   ...
   End of Program

   To Visualize in 3D, run python file:'python pgm3D.py'
   Check README.docx on how to run file if error
   ```

6) The PGM files generated can be found in the 'data' folder and you can now run the python file pgm3D.py or extract pgm files for other use for pgm Files.

pgm3D.py:
1) Make sure you have python installed. 
   If you don't have it you can download it from the official Python website: https://www.python.org/downloads/

2) Install required libraries. 
   You can install the required libraries using pip, the Python package manager. 
   Open your command line or terminal and run the following commands:
   ```
   pip install numpy
   pip install Pillow
   pip install pyvista
   ```

4) Open pgm3D.py using notepad and for line 31:
   "directory = 'Your_Directory/Final_Code/data' # Change this to your directory"
   Change “your_Directory” to the location in text containing folder 'Final_Code'.
   Example: 'C:/Users/Your_name/Documents/Final_Code/data'
   
5) Run pgm3D.py in your command line or termal:
   ```
   python pgm3D.py
   ```
  The volume shape will be displayed and a warning sign that you can ignore, e.g.:
   ```
   Volume shape: (240, 1980, 1590)
C:\Users\kearn\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.11_qbz5n2kfra8p0\LocalCache\local-packages\Python311\site-packages\pyvista\core\grid.py:912: PyVistaDeprecationWarning: `UniformGrid` is deprecated. Use `ImageData` instead.
  warnings.warn(
   ``` 
  
5) View and inspect the 3D models in the PyVista pop up, you can change the angle of viewing through dragging the mouse across the pop up and zoom in using the touch pad:
 
