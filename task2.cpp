#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

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

class Shape {
protected:
	int x, y;
	char s;
	screen *scr;
public:
	Shape() :
		x{0}, y{0}, s{0}, scr{nullptr} {}
	Shape(int x_, int y_, char s_, screen *scr_) :
		x{x_}, y{y_}, s{s_}, scr{scr_} {}
	Shape(Shape const &other) {
		x = other.x;
		y = other.y;
		s = other.s;
		scr = other.scr;
	}
	Shape(Shape &&other) {
		x = other.x;
		y = other.y;
		s = other.s;
		scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.s = 0;
		other.scr = nullptr;
	}
	Shape &operator=(Shape const &other) {
		x = other.x;
		y = other.y;
		s = other.s;
		scr = other.scr;
		return *this;
	}
	Shape &operator=(Shape &&other) {
		x = other.x;
		y = other.y;
		s = other.s;
		scr = other.scr;
		other.x = 0;
		other.y = 0;
		other.s = 0;
		other.scr = nullptr;
		return *this;
	}
	virtual ~Shape() = 0;
	int get_x() {
		return x;
	}
	void set_x(int x_) {
		x = x_;
	}
	int get_y() {
		return y;
	}
	void set_y(int y_) {
		y = y_;
	}
	char get_s() {
		return s;
	}
	void set_s(char s_) {
		s = s_;
	}
	screen *get_scr() {
		return scr;
	}
	void set_scr(screen *scr_) {
		scr = scr_;
	}
	virtual void draw() = 0;
	virtual string info() = 0;
};

Shape::~Shape() {}

class Point : public Shape {
public:
	Point() :
		Shape(0, 0, 0, nullptr) {}
	Point(int x_, int y_, char s_, screen *scr_) :
		Shape(x_, y_, s_, scr_) {}
	Point(Point const &other) :
		Shape(other) {}
	Point(Point &&other) :
		Shape(other) {}
	Point &operator=(Point const &other) {
		Shape::operator=(other);
		return *this;
	}
	Point &operator=(Point &&other) {
		Shape::operator=(other);
		return *this;
	}
	~Point() {}
	int get_x() {
		return x;
	}
	void set_x(int x_) {
		x = x_;
	}
	void draw() {
		if (scr != nullptr)
			scr->point(x, y, s);
	}
	string info() {
		string str = "point";
		str += " " + to_string(x);
		str += " " + to_string(y);
		str += " ";
		str.push_back(s);
		return str;
	}
};

class Ellipse : public Shape {
	int a, b;
public:
	Ellipse() :
		Shape(0, 0, 0, nullptr), a{0}, b{0} {}
	Ellipse(int x_, int y_, int a_, int b_, char s_, screen *scr_) :
		Shape(x_, y_, s_, scr_), a{a_}, b{b_} {}
	Ellipse(Ellipse const &other) : Shape(other) {
		this->a = other.a;
		this->b = other.b;
	}
	Ellipse(Ellipse &&other) : Shape(other) {
		this->a = other.a;
		this->b = other.b;
		other.a = 0;
		other.b = 0;
	}
	Ellipse &operator=(Ellipse const &other) {
		Shape::operator=(other);
		this->a = other.a;
		this->b = other.b;
		return *this;
	}
	Ellipse &operator=(Ellipse &&other) {
		Shape::operator=(other);
		this->a = other.a;
		this->b = other.b;
		other.a = 0;
		other.b = 0;
		return *this;
	}
	~Ellipse() {}
	int get_a() {
		return a;
	}
	void set_a(int a_) {
		a = a_;
	}
	int get_b() {
		return b;
	}
	void set_b(int b_) {
		b = b_;
	}
	void draw() {
		if (scr != nullptr)
			scr->ellipse(x, y, a, b, s);
	}
	string info() {
		string str = "ellipse";
		str += " " + to_string(x);
		str += " " + to_string(y);
		str += " " + to_string(a);
		str += " " + to_string(b);
		str += " ";
		str.push_back(s);
		return str;
	}
};

class Polygon : public Shape {
	int n, l;
public:
	Polygon() :
		Shape(0, 0, 0, nullptr), n{0}, l{0} {}
	Polygon(int x_, int y_, int n_, int l_, char s_, screen *scr_) :
		Shape(x_, y_, s_, scr_), n{n_}, l{l_} {}
	Polygon(Polygon const &other) : Shape(other) {
		this->n = other.n;
		this->l = other.l;
	}
	Polygon(Polygon &&other) : Shape(other) {
		this->n = other.n;
		this->l = other.l;
		other.n = 0;
		other.l = 0;
	}
	Polygon &operator=(Polygon const &other) {
		Shape::operator=(other);
		this->n = other.n;
		this->l = other.l;
		return *this;
	}
	Polygon &operator=(Polygon &&other) {
		Shape::operator=(other);
		this->n = other.n;
		this->l = other.l;
		other.n = 0;
		other.l = 0;
		return *this;
	}
	~Polygon() {}
	int get_n() {
		return n;
	}
	void set_n(int n_) {
		n = n_;
	}
	int get_l() {
		return l;
	}
	void set_l(int l_) {
		l = l_;
	}
	void draw() {
		if (scr != nullptr)
			scr->polygon(x, y, n, l, s);
	}
	string info() {
		string str = "polygon";
		str += " " + to_string(x);
		str += " " + to_string(y);
		str += " " + to_string(n);
		str += " " + to_string(l);
		str += " ";
		str.push_back(s);
		return str;
	}
};

class Line : public Shape {
	int xp, yp;
public:
	Line() :
		Shape(0, 0, 0, nullptr), xp{0}, yp{0} {}
	Line(int x_, int y_, int xp_, int yp_, char s_, screen *scr_) :
		Shape(x_, y_, s_, scr_), xp{xp_}, yp{yp_} {}
	Line(Line const &other) : Shape(other) {
		this->xp = other.xp;
		this->yp = other.yp;
	}
	Line(Line &&other) : Shape(other) {
		this->xp = other.xp;
		this->yp = other.yp;
		other.xp = 0;
		other.yp = 0;
	}
	Line &operator=(Line const &other) {
		Shape::operator=(other);
		this->xp = other.xp;
		this->yp = other.yp;
		return *this;
	}
	Line &operator=(Line &&other) {
		Shape::operator=(other);
		this->xp = other.xp;
		this->yp = other.yp;
		other.xp = 0;
		other.yp = 0;
		return *this;
	}
	~Line() {}
	int get_xp() {
		return xp;
	}
	void set_xp(int xp_) {
		xp = xp_;
	}
	int get_yp() {
		return yp;
	}
	void set_yp(int yp_) {
		yp = yp_;
	}
	void draw() {
		if (scr != nullptr)
			scr->line(x, y, xp, yp, s);
	}
	string info() {
		string str = "line";
		str += " " + to_string(x);
		str += " " + to_string(y);
		str += " " + to_string(xp);
		str += " " + to_string(yp);
		str += " ";
		str.push_back(s);
		return str;
	}
};

template <class S>
class Bunch {
	static int const max_length = 32;
	S *items[max_length];
	size_t length;
public:
	Bunch() :
		length{0} {}
	~Bunch() {
		while (length > 0)
			remove();
	}
	size_t get_length() {
		return length;
	}
	S *operator[](size_t index) {
		return items[index];
	}
	void clear() {
		length = 0;
	}
	int add(S *item) {
		if (length < max_length) {
			items[length++] = item;
			return 0;
		}
		return -1;
	}
	int remove() {
		if (length > 0) {
			delete items[--length];
			return 0;
		}
		return -1;
	}
	void write(ostream &file) {
		for (size_t i = 0; i < length; ++i) {
			file << items[i]->info() << endl;
		}
	}
	void list() {
		write(cout);
	}
	void paint(size_t index) {
		if (index == length) {
			for (size_t i = 0; i < length; ++i) {
				items[i]->draw();
			}
		} else if (index < length) {
			items[index]->draw();
		}
	}
};

void draw_bunch(Bunch<Shape> &bunch, size_t index, screen &scr)
{
	if (index <= bunch.get_length()) {
		bunch.paint(index);
		scr.display();
	} else {
		cerr << "index out of range" << endl;
	}
}

void bad_input()
{
	cerr << "bad input" << endl;
}

int add(Bunch<Shape> &bunch, screen &scr, char const *command)
{
	int x, y, a, b;
	char s;
	if (sscanf(command, "point %i %i %c", &x, &y, &s) == 3) {
		Shape *point = new Point(x, y, s, &scr);
		point->draw();
		if (bunch.add(point))
			cerr << "adding point failed" << endl;
		else
			return 0;
	} else if (sscanf(command, "ellipse %i %i %i %i %c", &x, &y, &a, &b, &s) == 5) {
		Shape *ellipse = new Ellipse(x, y, a, b, s, &scr);
		ellipse->draw();
		if (bunch.add(ellipse))
			cerr << "adding ellipse failed" << endl;
		else
			return 0;
	} else if (sscanf(command, "polygon %i %i %i %i %c", &x, &y, &a, &b, &s) == 5) {
		Shape *polygon = new Polygon(x, y, a, b, s, &scr);
		polygon->draw();
		if (bunch.add(polygon))
			cerr << "adding polygon failed" << endl;
		else
			return 0;
	} else if (sscanf(command, "line %i %i %i %i %c", &x, &y, &a, &b, &s) == 5) {
		Shape *line = new Line(x, y, a, b, s, &scr);
		line->draw();
		if (bunch.add(line))
			cerr << "adding line failed" << endl;
		else
			return 0;
	}
	return -1;
}

int read(Bunch<Shape> &bunch, screen &scr, char const *filename)
{
	ifstream file;
	file.open(filename);
	if (file.fail()) {
		cerr << "could not open file" << endl;
		return -1;
	}
	string line;
	bunch.clear();
	while (getline(file, line)) {
		if (add(bunch, scr, line.c_str())) {
			cerr << "bad file input" << endl;
			return -1;
		}
	}
	return 0;
}

int write(Bunch<Shape> &bunch, char const *filename)
{
	ofstream file;
	file.open(filename);
	bunch.write(file);
	return 0;
}

int remove(Bunch<Shape> &bunch)
{
	if (bunch.remove()) {
		cerr << "failed to remove shape" << endl;
		return -1;
	} else {
		return 0;
	}
}

int
main()
{
	string input;
	char const *cstr;
	char filename[256];
	int index;
	Bunch<Shape> bunch;
	screen scr;
	while (true) {
		cout << "> ";
		getline(cin, input);
		cstr = input.c_str();
		if (strncmp(cstr, "draw ", 5) == 0) {
			if (add(bunch, scr, cstr+5)) {
				bad_input();
			} else {
				scr.display();
			}
		} else if (strcmp(cstr, "remove") == 0) {
			if (remove(bunch)) {
			} else {
				scr.clear();
				bunch.paint(bunch.get_length());
				scr.display();
			}
		} else if (strcmp(cstr, "clear") == 0) {
			bunch.clear();
			scr.clear();
			scr.display();
		} else if (strcmp(cstr, "clear screen") == 0) {
			scr.clear();
			scr.display();
		} else if (strcmp(cstr, "list") == 0) {
			bunch.list();
		} else if (sscanf(cstr, "paint %i", &index) == 1) {
			scr.clear();
			draw_bunch(bunch, index, scr);
		} else if (sscanf(cstr, "read %s", filename) == 1) {
			scr.clear();
			if (read(bunch, scr, filename)) {
			} else {
				scr.display();
			}
		} else if (sscanf(cstr, "write %s", filename) == 1) {
			write(bunch, filename);
		} else if (strcmp(cstr, "exit") == 0) {
			break;
		} else {
			bad_input();
		}
	}
	return 0;
}
