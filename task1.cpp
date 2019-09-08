#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

/* A class to display 2d graphics in text mode */
/* The screen is a square (-dmin,-dmin) to (dmax,dmax) */
/* The typical screen size is (-20, -20) to (20,20) */
class screen {
public:
	screen();               // default constructor
	screen(screen const &that);     // copy constructor
	screen(screen && that) noexcept;      // move constructor
	screen & operator =(screen const &that);      // copy assignment
	screen & operator =(screen && that) noexcept; // move assignment
	~screen();             // destructor

	int dmin() const;       // getter for dmin
	int dmax() const;       // getter for dmax

	void display() const;   // display screen

	void clear();           // clear the screen
	void axes(char s);      // show both axes using symbol s

	void point(int x, int y, char s);       // plot a point using coordinate (x,y) and symbol s
	void ellipse(int x, int y, int a, int b, char s);       // plot an ellipse using center (x,y), radii a and b, and symbol s
	void polygon(int x, int y, int n, int l, char s);       // plot a polygon using center (x,y), n sides, length l, and symbol s

	void line(int x, int y, int xp, int yp, char s);        // plot a line from (x,y) to (xp, yp) using symbol s
private:

	// low lovel function to plot a single point  since y : x ratio is not 1, x is multiplied if r is true
	void plot(double x, double y, char s);  // plot a point 

	int const dim = 20;     // default dimension
	double const ratio = 2.23;      // aspect ratio
	double const pi = acos(-1);     // pi value

	int mWidth, mHeight;    // width and height
	char **mBoard;          // 2d screen board      
};

// default constructor
screen::screen():mWidth(ratio * dim * 2 + 1), mHeight(dim * 2 + 1)
{
	mBoard = new char *[mHeight];
	for (int j = 0; j < mHeight; ++j)
		mBoard[j] = new char[mWidth] ();
}

// destructor
screen::~screen()
{
	for (int j = 0; j < mHeight; ++j)
		delete[]mBoard[j];
	delete mBoard;
}

// copy constructor
screen::screen(screen const &that):
mWidth(that.mWidth),
mHeight(that.mHeight)
{
	mBoard = new char *[mHeight];
	for (int j = 0; j < mHeight; ++j) {
		mBoard[j] = new char[mWidth];
		for (int i = 0; i < mWidth; ++i)
			mBoard[j][i] = that.mBoard[j][i];
	}
}

// move constructor
screen::screen(screen && that) noexcept:
mWidth(move(that.mWidth)), mHeight(move(that.mHeight)),
mBoard(move(that.mBoard))
{
	that.mBoard = nullptr;
	that.mWidth = 0;
	that.mHeight = 0;
}

// copy assignment
screen & screen::operator =(screen const &that)
{
	if (this == &that)
		return *this;

	if (mHeight != that.mHeight || mWidth != that.mWidth) {
		for (int j = 0; j < mHeight; ++j)
			delete[]mBoard[j];
		delete
		    mBoard;
		mWidth = that.mWidth;
		mHeight = that.mHeight;
		mBoard = new char *[mHeight];
		for (int j = 0; j < mHeight; ++j)
			mBoard[j] = new char[mWidth] ();
	}

	for (int j = 0; j < mHeight; ++j)
		for (int i = 0; i < mWidth; ++i)
			mBoard[j][i] = that.mBoard[j][i];

	return *this;
}

// move constructor
screen & screen::operator =(screen && that) noexcept
{
	mWidth = move(that.mWidth);
	mHeight = move(that.mHeight);
	mBoard = move(that.mBoard);

	that.mBoard = nullptr;
	that.mWidth = 0;
	that.mHeight = 0;

	return *this;
}

// plot a point
void
screen::plot(double x, double y, char s)
{
	int yy = static_cast < int >(round(-y + mHeight / 2));
	int xx =
	    static_cast < int >(round(ratio * x + mWidth / 2 + ratio / 2));
	mBoard[yy][xx] = s;
}

// getter for dmax
int
screen::dmax() const
{
	return dim;
}

// getter for dmin
int
screen::dmin() const
{
	return -dim;
}

// display screen
void
screen::display() const
{
	for (int j = 0; j < mHeight; ++j) {
		for (int i = 0; i < mWidth; ++i)
			if (mBoard[j][i])
				cout << mBoard[j][i];
			else
				cout << ' ';
		cout << endl;
	}
}

// clear the screen
void
screen::clear()
{
	for (int j = 0; j < mHeight; ++j)
		for (int i = 0; i < mWidth; ++i)
			mBoard[j][i] = 0;
}

// show the axes using symbol s
void
screen::axes(char s)
{
	// plot x axis
	for (int x = dmin(); x <= dmax(); ++x)
		plot(x, 0, s);
	// plot y axis
	for (int y = dmin(); y <= dmax(); ++y)
		plot(0, y, s);
}

// plot a point at (x,y) with symbol s
void
screen::point(int x, int y, char s)
{
	plot(static_cast < double >(x), static_cast < double >(y), s);
}

// plot an ellipse using center (xc, yc), radii rx and ry, and symbol s
void
screen::ellipse(int xc, int yc, int rx, int ry, char s)
{
	double dx, dy, d1, d2, x, y;
	x = 0;
	y = ry;

	// initial decision parameter of region 1 
	d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
	dx = 2 * ry * ry * x;
	dy = 2 * rx * rx * y;

	// for region 1 
	while (dx < dy) {
		// print points based on 4-way symmetry 
		plot(x + xc, y + yc, s);
		plot(-x + xc, y + yc, s);
		plot(x + xc, -y + yc, s);
		plot(-x + xc, -y + yc, s);

		// checking and updating value of 
		// decision parameter based on algorithm 
		if (d1 < 0) {
			x++;
			dx = dx + (2 * ry * ry);
			d1 = d1 + dx + (ry * ry);
		} else {
			x++;
			y--;
			dx = dx + (2 * ry * ry);
			dy = dy - (2 * rx * rx);
			d1 = d1 + dx - dy + (ry * ry);
		}
	}

	// decision parameter of region 2 
	d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
	    ((rx * rx) * ((y - 1) * (y - 1))) - (rx * rx * ry * ry);

	// plotting points of region 2 
	while (y >= 0) {
		// print points based on 4-way symmetry 
		plot(x + xc, y + yc, s);
		plot(-x + xc, y + yc, s);
		plot(x + xc, -y + yc, s);
		plot(-x + xc, -y + yc, s);

		// checking and updating parameter 
		// value based on algorithm 
		if (d2 > 0) {
			y--;
			dy = dy - (2 * rx * rx);
			d2 = d2 + (rx * rx) - dy;
		} else {
			y--;
			x++;
			dx = dx + (2 * ry * ry);
			dy = dy - (2 * rx * rx);
			d2 = d2 + dx - dy + (rx * rx);
		}
	}
}

// plot a regular polygon with center (x,y), length l, n sides, using symbol s
void
screen::polygon(int x, int y, int n, int l, char s)
{
	double *xx = new double[n];
	double *yy = new double[n];
	for (int k = 0; k < n; ++k) {
		xx[k] = l * cos(2 * pi * k / n);
		yy[k] = l * sin(2 * pi * k / n);
	}
	for (int k = 0; k < n; ++k)
		line(x + xx[k], y + yy[k], x + xx[(k + 1) % n],
	             y + yy[(k + 1) % n], s);
}

// plot a line from (x0,y0) to (x1, y1) using symbol s
void
screen::line(int x0, int y0, int x1, int y1, char s)
{
	if (x1 == x0) {
		if (y1 > y0)
			for (double y = y0; y <= y1; y++)
				plot(x0, y, s);
		else
			for (double y = y1; y <= y0; y++)
				plot(x0, y, s);
		return;
	}

	if (y1 == y0) {
		if (x1 > x0)
			for (double x = x0; x <= x1; x++)
				plot(x, y0, s);
		else
			for (double x = x1; x <= x0; x++)
				plot(x, y0, s);
		return;
	}

	double m = 1.0 * (y1 - y0) / (x1 - x0);
	double c = 1.0 * (y0 * x1 - y1 * x0) / (x1 - x0);

	if (abs(x1 - x0) > abs(y1 - y0)) {
		if (x1 > x0)
			for (double x = x0; x <= x1; x += 1)
				plot(x, m * x + c, s);
		else
			for (double x = x1; x <= x0; x += 1)
				plot(x, m * x + c, s);
	} else {
		if (y1 > y0)
			for (double y = y0; y <= y1; y += 1)
				plot((y - c) / m, y, s);
		else
			for (double y = y1; y <= y0; y += 1)
				plot((y - c) / m, y, s);
	}
}

class Point {
	int x, y;
	char s;
	screen *scr;
public:
	Point(int x_, int y_, char s_, screen *scr_) :
		x{x_}, y{y_}, s{s_}, scr{scr_} {}
	Point(Point const &other) {
		this->x = other.x;
		this->y = other.y;
		this->s = other.s;
		this->scr = other.scr;
	}
	Point(Point &&other) {
		this->x = other.x;
		this->y = other.y;
		this->x = other.s;
		this->scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.s = 0;
		other.scr = nullptr;
	}
	Point &operator=(Point const &other) {
		this->x = other.x;
		this->y = other.y;
		this->s = other.s;
		this->scr = other.scr;
		return *this;
	}
	Point &operator=(Point &&other) {
		this->x = other.x;
		this->y = other.y;
		this->s = other.s;
		this->scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.s = 0;
		other.scr = nullptr;
		return *this;
	}
	~Point() {}
	void draw() {
		if (scr != nullptr)
			scr->point(x, y, s);
	}
	void info() {
		cout << "point";
		cout << " " << x;
		cout << " " << y;
		cout << " " << s;
		cout << endl;
	}
};

class Ellipse {
	int x, y, a, b;
	char s;
	screen *scr;
public:
	Ellipse(int x_, int y_, int a_, int b_, char s_, screen *scr_) :
		x{x_}, y{y_}, a{a_}, b{b_}, s{s_},scr{scr_} {}
	Ellipse(Ellipse const &other) {
		this->x = other.x;
		this->y = other.y;
		this->a = other.a;
		this->b = other.b;
		this->s = other.s;
		this->scr = other.scr;
	}
	Ellipse(Ellipse &&other) {
		this->x = other.x;
		this->y = other.y;
		this->a = other.a;
		this->b = other.b;
		this->s = other.s;
		this->scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.a = 0;
		other.b = 0;
		other.s = 0;
		other.scr = nullptr;
	}
	Ellipse &operator=(Ellipse const &other) {
		this->x = other.x;
		this->y = other.y;
		this->a = other.a;
		this->b = other.b;
		this->s = other.s;
		this->scr = other.scr;
		return *this;
	}
	Ellipse &operator=(Ellipse &&other) {
		this->x = other.x;
		this->y = other.y;
		this->a = other.a;
		this->b = other.b;
		this->s = other.s;
		this->scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.a = 0;
		other.b = 0;
		other.s = 0;
		other.scr = nullptr;
		return *this;
	}
	~Ellipse() {}
	void draw() {
		if (scr != nullptr)
			scr->ellipse(x, y, a, b, s);
	}
	void info() {
		cout << "ellipse";
		cout << " " << x;
		cout << " " << y;
		cout << " " << a;
		cout << " " << b;
		cout << " " << s;
		cout << endl;
	}
};

class Polygon {
	int x, y, n, l;
	char s;
	screen *scr;
public:
	Polygon(int x_, int y_, int n_, int l_, char s_, screen *scr_) :
		x{x_}, y{x_}, n{n_}, l{l_}, s{s_}, scr{scr_} {}
	void draw();
	void info();
};

class Line {
	int x, y, xp, yp;
	char s;
	screen *scr;
public:
	Line(int x_, int y_, int xp_, int yp_, char s_, screen *scr_) :
		x{x_}, y{y_}, xp{xp_}, yp{yp_}, s{s_}, scr{scr_} {}
	void draw();
	void info();
};

template <class S>
class Bunch {
public:
};
// main function
int
main()
{
	screen s;
	//      s.clear();                              // clear the screen
//      s.axes('+');                            // show axis using +
	s.point(-14, -16, '$'); // show point using $
	s.ellipse(6, 6, 5, 5, '*');     // circle since 5 = 5
	s.ellipse(-10, 8, 9, 5, '*');   // ellipse since 9 != 5
	s.polygon(-9, -9, 6, 8, '#');   // show 6-gon with length 8
	s.line(9, -12, 19, 7, '-');     // line from (9,12) to (19,7)
	s.display();            // display whatever drawn
	return 0;
}
