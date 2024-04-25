#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <random>

//values for materials
const int OUTSIDE = 0;
const int INNER_STRAWBERRY = 3;
const int OUTER_STRAWBERRY = 4;
const int SEED = 1;
const int STALK = 2;

std::random_device rd;
std::mt19937 gen(rd());
std::discrete_distribution<> strawberryType({50, 30, 20}); // Adjust these weights as needed
std::uniform_real_distribution<double> distX(0, 20); // range for rotation around the x axis
std::uniform_real_distribution<double> distY(160, 200); // range for rotation around the y axis
std::uniform_real_distribution<double> distZ(0, 360); //range for rotation around the z axis
std::uniform_real_distribution<> radiusDistLarge(1.0, 1.5); // range for radius for fully grown strawberry
std::uniform_real_distribution<> radiusDistMedium(1.5, 2.0); // range for radius for partially grown strawberry
std::uniform_real_distribution<> radiusDistSmall(2.0, 2.5); // range for radius for young strawberry
std::uniform_real_distribution<> bottomConeDistLarge(4.0, 16.0); // range for bottom of the cone for fully grown strawberry
std::uniform_real_distribution<> bottomConeDistMedium(16.0, 29.0); // range for bottom of the cone for partially grown strawberry
std::uniform_real_distribution<> bottomConeDistSmall(29.0, 36.0); // range for bottom of the cone for young strawberry
std::uniform_real_distribution<> randomDistance(15.0,30.0); //random distances for spatial distribution of strawberries
std::uniform_real_distribution<> randomDistanceZ(-30.0, 30.0);
std::uniform_real_distribution<> randomDistanceX(-50.0, 50.0);
std::bernoulli_distribution d(0.5); //random decision making for placement of strawberries

struct Point3D {
    double x, y, z;
};

int getStrawberryType() {
     int value = strawberryType(gen);
     return value + 1;
}

//rotate the strawberry randomly between 0 and 30 around the x axis 
double getRandomAngleX() {
    double angleDegrees = distX(gen);
    double angleRadians = angleDegrees * M_PI / 180.0;
    return angleRadians;
}

//rotate the strawberry roughly 180 degrees around the y axis with some random variation to face the right way
double getRandomAngleY() {
    double angleDegrees = distY(gen);
    double angleRadians = angleDegrees * M_PI / 180.0;
    return angleRadians;
}

//rotate the strawberry randomly between 0 and 360 degrees around the z axis 
double getRandomAngleZ() {
    double angleDegrees = distZ(gen);
    double angleRadians = angleDegrees * M_PI / 180.0;
    return angleRadians;
}

// rotation around x axis
Point3D rotateX(const Point3D& point, double cosAngle, double sinAngle) {
    return {point.x, point.y * cosAngle - point.z * sinAngle, point.y * sinAngle + point.z * cosAngle};
}

//roatation y axis
Point3D rotateY(const Point3D& point, double cosAngle, double sinAngle) {
    return {point.x * cosAngle + point.z * sinAngle, point.y, -point.x * sinAngle + point.z * cosAngle};
}

//rotation z axis
Point3D rotateZ(const Point3D& point, double cosAngle, double sinAngle) {
    return {point.x * cosAngle - point.y * sinAngle, point.x * sinAngle + point.y * cosAngle, point.z};
}

//rotate the entire 3D matrix
void rotateMatrix(int*** matrix, int nx, int ny, int nz, double angleX, double angleY, double angleZ) {
    // Temporary matrix to hold the rotated version
    int*** tempMatrix = new int**[nx];
    for (int x = 0; x < nx; ++x) {
        tempMatrix[x] = new int*[ny];
        for (int y = 0; y < ny; ++y) {
            tempMatrix[x][y] = new int[nz]();
        }
    }

    //center of rotation
    double centerX = nx / 2.0;
    double centerY = ny / 2.0;
    double centerZ = nz / 2.0;
 
    //cos and sin calculations
    double cosX = cos(angleX);
    double sinX = sin(angleX);
    double cosY = cos(angleY);
    double sinY = sin(angleY);
    double cosZ = cos(angleZ);
    double sinZ = sin(angleZ);
    
    for (int x = 0; x < nx; ++x) {
        for (int y = 0; y < ny; ++y) {
            for (int z = 0; z < nz; ++z) {
                // Calculate position relative to center
                Point3D point = {x - centerX, y - centerY, z - centerZ};

                // Rotate the point
                point = rotateX(point, cosX, sinX);
                point = rotateY(point, cosY, sinY);
                point = rotateZ(point, cosZ, sinZ);

                // Convert back to matrix coordinates
                int newX = static_cast<int>(round(point.x + centerX));
                int newY = static_cast<int>(round(point.y + centerY));
                int newZ = static_cast<int>(round(point.z + centerZ));

                // Check bounds and assign value
                if (newX >= 0 && newX < nx && newY >= 0 && newY < ny && newZ >= 0 && newZ < nz) {
                    tempMatrix[newX][newY][newZ] = matrix[x][y][z];
                }
            }
        }
    }

    //copy the temporary matrix back to the original matrix
    for (int x = 0; x < nx; ++x) {
        for (int y = 0; y < ny; ++y) {
            for (int z = 0; z < nz; ++z) {
                matrix[x][y][z] = tempMatrix[x][y][z];
            }
            delete[] tempMatrix[x][y];
        }
        delete[] tempMatrix[x];
    }
    delete[] tempMatrix;
}


void generateStrawberry(int*** mcode_d, int nx, int ny, int nz, int centreX, int centreY, int centreZ, double radius1, double radius2, double bottomCone1, double bottomCone2) {

    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            for (int z = 0; z < nz/2; ++z) { //from 0 to centreZ
                double distanceCone = 0.8*pow(x - centreX, 2) + 0.8*pow(y - centreY, 2);
                double distanceCone1 = 0.8*pow(x - centreX, 2) + 0.8*pow(y - centreY, 2);

                if (distanceCone <= (pow(radius1, 2) / (centreZ - bottomCone1)) * (z - bottomCone1) && distanceCone1 > (pow(radius2, 2) / (centreZ - bottomCone2)) * (z - bottomCone2)) {
                    mcode_d[x][y][z] = OUTER_STRAWBERRY; //conditions for outer meat of bottom strawberry
                }
                else if (distanceCone1 <= (pow(radius2, 2) / (centreZ - bottomCone2)) * (z - bottomCone2)) {
                    mcode_d[x][y][z] = INNER_STRAWBERRY; //conditions for inner meat of bottom strawberry 
                } 
                else{
                    mcode_d[x][y][z] = OUTSIDE; //otherwise air block
                }
            }

            for (int z = nz/2; z <= centreZ + radius1; ++z) { //from centeZ to centreZ + radius1
                double distanceSphere = 0.8*pow(x - centreX, 2) + 0.8*pow(y - centreY, 2) + pow(z - centreZ, 2); 
                double distanceSphere1 = 0.8*pow(x - centreX, 2) + 0.8*pow(y - centreY, 2) + 0.25*pow(z - centreZ, 2);

                if (distanceSphere <= pow(radius1,2) && distanceSphere1 > pow(radius2,2)) {
                    mcode_d[x][y][z] = OUTER_STRAWBERRY; //conditions for outer meat of top strawberry
                } else if (distanceSphere1 <= pow(radius2,2)) { 
                    mcode_d[x][y][z] = INNER_STRAWBERRY; //conditions for inner meat of top strawberry
                } else {
                    mcode_d[x][y][z] = OUTSIDE; //otherwise outside of strawberry
                }
            }
            for (int z = centreZ + radius1 + 1; z < nz; ++z){
                double distanceCylinder = sqrt(pow(x - centreX,2) + pow(y - centreY, 2));
                if (distanceCylinder <= 1) {mcode_d[x][y][z] = STALK;} //condition for Stalk of strawberry
                else {mcode_d[x][y][z] = OUTSIDE;} //otherwise outside
            }
        }
    }
}

// Function to fill gaps in the rotated matrix
void fillGaps(int*** matrix, int nx, int ny, int nz) {
    for (int x = 1; x < nx - 1; ++x) {
        for (int y = 1; y < ny - 1; ++y) {
            for (int z = 1; z < nz - 1; ++z) {
                if (matrix[x][y][z] == OUTSIDE) {
                    // Count the frequency of each type around the current cell
                    int countOuter = 0, countInner = 0, countOutside = 0, countStalk = 0;
                    for (int dx = 0; dx <= 1; ++dx) {
                        for (int dy = 0; dy <= 1; ++dy) {
                            for (int dz = 0; dz <= 1; ++dz) {
                                if (dx == 0 && dy == 0 && dz == 0) continue; // Skip the current cell
                                int neighbourValue = matrix[x + dx][y + dy][z + dz];
                                if (neighbourValue == OUTER_STRAWBERRY) countOuter++;
                                else if (neighbourValue == INNER_STRAWBERRY) countInner++;
                                else if (neighbourValue == OUTSIDE) countOutside++;
                                else if (neighbourValue == STALK) countStalk++;
                            }
                        }
                    }
                    // Decide the value based on neighbors
                    if (countOuter > countInner && countOuter > countOutside && countOuter > countStalk) matrix[x][y][z] = OUTER_STRAWBERRY;
                    else if (countInner >= countOuter && countInner > countOutside && countInner > countStalk) matrix[x][y][z] = INNER_STRAWBERRY;
                    else if (countOutside >= countOuter && countOutside > countInner && countOutside > countStalk) matrix[x][y][z] = OUTSIDE;
                    else if (countStalk >= countOuter && countStalk > countInner && countStalk > countOutside) matrix[x][y][z] = STALK;
                }
            }
        }
    }
}


void generateStrawberryGrid(int*** mcode_d1, int*** mcode_d, int nStrawb, int nx, int ny, int nz, int nx1, int ny1, int nz1, int centreX, int centreY, int centreZ) {
    double initialRadius1 = nx / 2.5;
    double initialBottomCone1 = nz/60;
    double radius1, radius2, bottomCone1, bottomCone2;

    int startX = nx1/2 - centreX;
    int startY = ny1/2 - centreY;
    int startY1 = startY + randomDistance(gen);
    int startZ = nz1/2 - centreZ;
    int spacingX, spacingY, spacingZ;
    
    //counts for spacing along the y axis.
    int countY0 = 0;
    int countY1 = 0;

    //Variables to calculate spacing along the different axis'
    int previousSpacingX = 0;
    int previousSpacingY = 0;
    int previousSpacingZ = 0;
    int Zlevel = 60;
    int totalSpacingY1 = 0;
    int totalSpacingY0 = 0;

    //User decides type of strawberry
    int type;
    std::string strawberryType;            
    printf("\nWhat level of maturity strawberries would you like to produce? (Random, Mature, Immature, Medium) ");
    std::cin >> strawberryType; // Read a string from the user
    
    printf("\n...\nStrawberries Generated:");

    for (int i=0; i < nStrawb; ++i){
            //previous strawberry variables for spacing calculations
            int previousRadius = radius1;
            int previousBottomCone = bottomCone1;
            previousSpacingX = spacingX;
            previousSpacingY = spacingY;
            previousSpacingZ = spacingZ;

            //defining type of strawberry for random variation
            if (strawberryType == "Random") {
                type = getStrawberryType();
            }
            else if (strawberryType == "Mature") {
                type = 1;
            }
            else if (strawberryType == "Immature") {
                type = 3;
            }
            else if (strawberryType == "Medium") {
                type = 2;
            }
            else {
                printf("\nIncorrect input: Generating Random by default");
                type = getStrawberryType();
            }

            //reset radius and bottomCone incase of error
            radius1 = 0;
            bottomCone1 = 0;

            if (type == 1){ //fully grown strawberry
        	radius1 = initialRadius1 / radiusDistLarge(gen);
        	radius2 = radius1 / 2.0;
        	bottomCone1 = initialBottomCone1 * bottomConeDistLarge(gen);
        	bottomCone2 = bottomCone1 * 1.1;
    	    }

    	    if (type == 2){ //partially grown strawberry
        	radius1 = initialRadius1 / radiusDistMedium(gen);
        	radius2 = radius1 / 2.0;
        	bottomCone1 = initialBottomCone1 * bottomConeDistMedium(gen);
        	bottomCone2 = bottomCone1 * 1.1;
            }
 
    	    if (type == 3){ //young strawberry
        	radius1 = initialRadius1 / radiusDistSmall(gen);
       		radius2 = radius1 / 2.0;
        	bottomCone1 = initialBottomCone1 * bottomConeDistSmall(gen);
        	bottomCone2 = bottomCone1 * 1.1;
            }
            
            //call function to generate strawberry and orientation
            generateStrawberry(mcode_d, nx, ny, nz, centreX, centreY, centreZ, radius1, radius2, bottomCone1, bottomCone2);
            rotateMatrix(mcode_d, nx, ny, nz, getRandomAngleX(), getRandomAngleY(), getRandomAngleZ());
            fillGaps(mcode_d, nx, ny, nz);
 
            printf("\n%d",i+1); //strawberry number being generated

            //random placing
            if (d(gen) == 1){spacingX = randomDistanceX(gen);}
            else {spacingX = -randomDistanceX(gen);}
            spacingY = previousRadius + randomDistance(gen);
            spacingZ = randomDistanceZ(gen);

            //layered sitribution
            if (i == nStrawb/3) {
                spacingZ = Zlevel + randomDistanceZ(gen);
                countY1 = 0;
                countY0 = 0;
                totalSpacingY0 = 0;
                totalSpacingY1 = 0;
            }
            else if (i > nStrawb/3 && i < 2 * nStrawb/3) {spacingZ = Zlevel + randomDistanceZ(gen);}
            else if (i == 2*nStrawb/3){
                spacingZ = 2 * Zlevel + randomDistanceZ(gen);
                countY1 = 0;
                countY0 = 0;
                totalSpacingY0 = 0;
                totalSpacingY1 = 0;
            }
            else if (i > 2*nStrawb/3){spacingZ = 2*Zlevel + randomDistanceZ(gen);}
            else {spacingZ = randomDistanceZ(gen);}

            int strawbX, strawbY, strawbZ; //x,y,z variables for larger matrix
            
            if (d(gen) == 1){
                countY1++; //count for right direction along y axis
                totalSpacingY1 = totalSpacingY1 + spacingY; //spacing calculations
            for (int x = 0; x < nx; ++x) {
                for (int y = 0; y < ny; ++y) {
                    for (int z = 0; z < nz; ++z) {
                        strawbX = x + (spacingX);  //x axis placement
                        strawbY = y + startY + (totalSpacingY1); //y axis placement
                        strawbZ = z + startZ + (spacingZ); //z axis placement
                	if (strawbX >= 0 && strawbX < nx1 && strawbY >= 0 && strawbY < ny1 && strawbZ >= 0 && strawbZ < nz1) { //condition for overlap
                	    if (mcode_d1[strawbX][strawbY][strawbZ] == OUTSIDE) {
                                mcode_d1[strawbX][strawbY][strawbZ] = mcode_d[x][y][z]; //place strawberry in larger grid
                            }
                        }
                    }
                }
            }
        }
            else{
            countY0++; //count for left direction along y axis
            totalSpacingY0 = totalSpacingY0 + spacingY; //spacing calculations
            for (int x = 0; x < nx; ++x) {
                for (int y = 0; y < ny; ++y) {
                    for (int z = 0; z < nz; ++z) {
                        strawbX = x + (spacingX); //x axis placement
                        strawbY = y + startY - (totalSpacingY0); //y axis placement
                        strawbZ = z + startZ + (spacingZ); //z axis placement
                	if (strawbX >= 0 && strawbX < nx1 && strawbY >= 0 && strawbY < ny1 && strawbZ >= 0 && strawbZ < nz1) {
                	    if (mcode_d1[strawbX][strawbY][strawbZ] == OUTSIDE) {
                                mcode_d1[strawbX][strawbY][strawbZ] = mcode_d[x][y][z]; //place strawberry in larger grid
                            }
                        }
                    }
                }
            }
        }
    }
}


int main() {
    //number of strawberries to be generated received from user
    printf("How Many strawberries would you like to generate? (recommended between 5 and 30) ");
    int nStrawb;
    scanf("%d", &nStrawb);

    //dimensions of array holding single strawberry
    int nx = 80;
    int ny = nx;
    int nz = nx*1.25;

    //dimensions of array holding strawberry bed
    int nx1 = 3*nx;
    int ny1 = (2*ny/3) * nStrawb;
    int nz1 = 2*nz/3 * nStrawb;

    //centre of array holding single strawberry
    int centreX = nx / 2;
    int centreY = ny / 2;
    int centreZ = nz / 2;
    
    //3D array holding single strawberry
    int*** mcode_d = new int**[nx];
    for (int x = 0; x < nx; ++x) {
        mcode_d[x] = new int*[ny];
        for (int y = 0; y < ny; ++y) {
            mcode_d[x][y] = new int[nz];
            for (int z = 0; z < nz; ++z) {
                mcode_d[x][y][z] = OUTSIDE; //initialize matrix to 0
            }
        }
    }

    //3D array holding strawberry bed
    int*** mcode_d1 = new int**[nx1];
    for (int x = 0; x < nx1; ++x) {
        mcode_d1[x] = new int*[ny1];
        for (int y = 0; y < ny1; ++y) {
            mcode_d1[x][y] = new int[nz1];
            for (int z = 0; z < nz1; ++z) {
                mcode_d1[x][y][z] = OUTSIDE; //initialize matrix to 0
            }
        }
    }
    
    //strawberry bed generation
    generateStrawberryGrid(mcode_d1, mcode_d, nStrawb, nx, ny, nz, nx1, ny1, nz1, centreX, centreY, centreZ);

    //convert to pgm format
    printf("\n...\nGrid Generated!\n...\nConverting to PGM...");
       for (int x = 0; x < nx1; ++x) { //slicing in x direction
        std::string filename = "./data/strawberry_" + std::to_string(x) + ".pgm"; //save files in data folder
        std::ofstream file(filename);

        file << "P2" << std::endl; //magic identifier header
        file << "#test" << std::endl; 
        file << ny1 << " " << nz1 << std::endl; //dimensions header
        file << "4" << std::endl; //number of materials header

        for (int z = 0; z < nz1; ++z) {
            for (int y = 0; y < ny1; ++y) {
                file << mcode_d1[x][y][z] << " "; //place array in the file
            }
        }

        file.close();
    }

    printf("\nConverted!\n..."); //completed conversion

    // Deallocate the dynamic memory
    for (int x = 0; x < nx; ++x) {
        for (int y = 0; y < ny; ++y) {
            delete[] mcode_d[x][y];
        }
        delete[] mcode_d[x];
    }
    delete[] mcode_d;

    for (int x = 0; x < nx1; ++x) {
        for (int y = 0; y < ny1; ++y) {
            delete[] mcode_d1[x][y];
        }
        delete[] mcode_d1[x];
    }
    delete[] mcode_d1;

    printf("\nEnd of Program");
    printf("\n\nTo Visualize in 3D, run python file:'python pgm3D.py'\nCheck README.docx on how to run file if error\n");

    return 0;
}
