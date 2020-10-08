#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

const int width = 64;			   // width in pixels of the file
const int height = 64;			   // height in pixels of the file
int colorBuffer[width][height][3]; // pixel color buffer for the ppm file (3 comes from rgb, the rest is the number of pixels in the file)

/*
write on existing file data with new file data
only replacing the new pixels
*/
void writeToFile(const char* filename) {
	string content = "";
	ofstream file(filename);

	// setup file
	file << "P3\n" << "# " << string(filename) << endl << height << string(" ") << width << endl << "15 " << endl;

	// loop through the color buffer and add each pixel to the file (in this case it is just printing one color per line)
	for (int i=0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < 3; k++) {
				file << colorBuffer[i][j][k] << " ";
			}
			file << "\n";
		}
	}
	file.close();
}


/*check bresenham when changing y*/
void bresenhamLow(int* color, int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int y_increment;
	if (dy < 0) {
		y_increment = -1;
		dy *= -1;
	}
	else {
		y_increment = 1;
	}
	int D = (2 * dy) - dx;

	int y = y1;
	for (int x = x1; x <= x2; x++) {
		// Set the color if the current point
		colorBuffer[y][x][0] = color[0];
		colorBuffer[y][x][1] = color[1];
		colorBuffer[y][x][2] = color[2];
		//------------------------------------

		// increment y if the Difference between points is positive
		if (D > 0) {
			y += y_increment;
			D = D + (2 * (dy - dx));
		}
		else {
			D = D + 2 * dy;
		}
	}
}


/*check bresenham when changing x*/
void bresenhamHigh(int* color, int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int x_increment;
	if (dx < 0) {
		x_increment = -1;
		dx *= -1;
	}
	else {
		x_increment = 1;
	}
	int D = (2 * dx) - dy;

	int x = x1;
	for (int y = y1; y <= y2; y++) {
		

		// Set the color if the current point
		colorBuffer[y][x][0] = color[0];
		colorBuffer[y][x][1] = color[1];
		colorBuffer[y][x][2] = color[2];
		//------------------------------------

		// increment y if the Difference between points is positive
		cout << D << endl;
		if (D > 0) {
			x += x_increment;
			D = D + (2 * (dx - dy));
		}
		else {
			D = D + 2 * dx;
		}
	}
}


/*Input coordinates of the line to be drawn and output results into the file string*/
void brisenham(int* color, int x1, int y1, int x2, int y2) {

	// check which has the greater difference
	if (abs(y2 - y1) < abs(x2 - x1)) {
		// check the line direction
		if (x1 > x2) {
			bresenhamLow(color, x2, y2, x1, y1);
		}
		else {
			bresenhamLow(color, x1, y1, x2, y2);
		}
	}
	else {
		// check the line diretion
		if (y1 > y2) {
			bresenhamHigh(color, x2, y2, x1, y1);
		}
		else {
			bresenhamHigh(color, x1, y1, x2, y2);
		}
	}
}


/*Use brisenham to draw four lines given two corner points of a rectangle (top left and bottom right)*/
void makeRectangle(int* color, int x1, int y1, int x2, int y2) {
	// locations of top right and bottom left corners
	int trX, trY;
	int blX, blY;
	// top right
	trX = x2;
	trY = y1;
	// bottom left
	blX = x1;
	blY = y2;
	
	// draw four lines of rectangle using brisenham
	brisenham(color, x1,y1,trX,trY);
	brisenham(color, trX,trY,x2,y2);
	brisenham(color, x1,y1,blX,blY);
	brisenham(color, blX,blY,x2,y2);
}


/* fill the inside of the rectangle */
void fill(int* color) {

}

/*Initialize color buffer to be all an all black color*/
void initializeColorBuffer() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < 3; k++) {
				colorBuffer[i][j][k] = 0;
			}
		}
	}
}


int main() {
	//initialize the color Buffer array to be all black

	// coordinates are in rows and columns in this type of file
	int rectangleTopLeft_x = 10;
	int rectangleTopLeft_y = 10;
	int rectangleBotRight_x = 40;
	int rectangleBotRight_y = 40;

	// rgb color could have made it a struct but eh
	int color[3] = {0,15,7};

	// test brisenham
	//brisenham(color, 10,60,40,8);

	// make the rectangle
    makeRectangle(color, rectangleTopLeft_x, rectangleTopLeft_y, rectangleBotRight_x, rectangleBotRight_y);

	// write to a ppm file at the end of the program
	writeToFile("test.ppm");

	return EXIT_SUCCESS;
}
