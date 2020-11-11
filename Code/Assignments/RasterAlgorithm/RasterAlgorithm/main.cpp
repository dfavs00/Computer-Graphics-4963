#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

/*Struct of a 2D vertex*/
struct Vertex2D {
	int x;
	int y;
};

int color_0[3] = { 0,15,7 };
int color_1[3] = { 15,15,15 };

const int width = 64;			   // width in pixels of the file
const int height = 64;			   // height in pixels of the file
int colorBuffer[width][height][3]; // pixel color buffer for the ppm file (3 comes from rgb, the rest is the number of pixels in the file)


int* triangleStrip;		   // triangle strip vertex2D array
int triangleStripCoords[8] = { 5, 5, 5, 10, 10, 5,
								 10, 10 };

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


/*check bresenham when changing y to stay near slope line*/
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


/*Draw a pixel at ppm file location (x y) with specified color*/
void drawPixel(int x, int y, int* color) {
	colorBuffer[y][x][0] = color[0];
	colorBuffer[y][x][1] = color[1];
	colorBuffer[y][x][2] = color[2];
}


/*check bresenham when changing x to stay near slope line*/
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


/* This is a recursive driver function */
/* fill the inside of the rectangle using recursive "splotching" given the bounds of the rectangle*/
void fillRectangle(int* color, int startX, int startY, int endX, int endY) {
	// simply iterate over the entire rectangle setting the color at each point
	for (int x = startX; x <= endX; x++) {
		for (int y = startY; y <= endY; y++) {
			for (int c = 0; c < 3; c++) {
				colorBuffer[x][y][c] = color[c];
			}
		}
	}
}


/* make filled rectangle with top left and bottom right point*/
/* any other input results in undefined behavior */
void makeFilledRectangle(int* color, int x1, int y1, int x2, int y2) {
	fillRectangle(color, x1,y1,x2,y2);
}


/*draw line with specified color and points*/
void drawLine(int* color, int x1, int y1, int x2, int y2) {
	brisenham(color, x1, y1, x2, y2);
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

/*draw a triangle with a color given 3 points*/
void drawTriangle(Vertex2D one, Vertex2D two, Vertex2D three, int* color) {
	drawLine(color, one.x, one.y, two.x, two.y);
	drawLine(color, two.x, two.y, three.x, three.y);
	drawLine(color, three.x, three.y, one.x, one.y);
}


/*draw a triangle with a color given 3 points*/
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int* color) {
	drawLine(color, x1, y1, x2, y2);
	drawLine(color, x2, y2, x3, y3);
	drawLine(color, x3, y3, x1, y1);
}


/*draw bezier curve given four points given in an x array and y array with the specified color*/
void bezierCurve(int* color, int* x_points, int* y_points) {

}


/*draw a 2D triangle strip given an array of triangle coordinates*/
void drawTriangleStrip2D(int* vertexArray, int numVertices, int* color) {

	int size = numVertices * 2;
	//cout << color[0] << endl;

	// keep track of 3 points at a time and replace the last used point every time a new point is read in
	int x1=0, y1=1, x2=2, y2=3, x3=4, y3=5;

	while (y3 < size) {

		// draw triangle
		drawTriangle(vertexArray[x1], vertexArray[y1], vertexArray[x2], vertexArray[y2], vertexArray[x3], vertexArray[y3], color);

		// inrement each stream iterator 
		x1 += 2; y1 += 2;
		x2 += 2; y2 += 2;
		x3 += 2; y3 += 2;
	}
}


/*setup the vertices for a the array of Vertex2D array*/
void setupVertices() {

	

	// set triangle strip to have the coordinates specified in triangle strip coord
	triangleStrip = triangleStripCoords;
}



void init() {
	setupVertices();
}


void test() {


	// coordinates are in rows and columns in this type of file
	//int rectangleTopLeft_x = 10;
	//int rectangleTopLeft_y = 10;
	//int rectangleBotRight_x = 40;
	//int rectangleBotRight_y = 40;

	// test brisenham
	//brisenham(color_0, 10,60,40,8);

	// test rectangle
	//makeRectangle(color_0, rectangleTopLeft_x, rectangleTopLeft_y, rectangleBotRight_x, rectangleBotRight_y);
	//makeFilledRectangle(color_1, 15, 15, 35, 35);

	// test triangle
	//Vertex2D one, two, three;
	//one.x = 5; one.y = 5;
	//two.x = 5, two.y = 10;
	//three.x = 10; three.y = 5;
	//drawTriangle(one, two, three, color_0);

	//test triangle strip
	
	drawTriangleStrip2D(triangleStrip, 4, color_1);
}


int main() {

	init();
	test();

	// write to a ppm file at the end of the program
	cout << "writing to file" << endl;
	writeToFile("test.ppm");

	return EXIT_SUCCESS;
}
