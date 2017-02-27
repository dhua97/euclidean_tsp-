#include <iostream> 
#include <vector>
#include <cmath>
#include <set>
#include <stdlib.h>
#include <algorithm>
#include <map>

using namespace std;

static int surface_size = 420;

// struct for a city, consists of a city number and coordinates
typedef struct CITY {
	int num;
	int x;
	int y;
} CITY;

// creates new city 
CITY new_city(int n, int xcoord, int ycoord) {
	CITY c;
	c.num = n;
	c.x = xcoord;
	c.y = ycoord;
	return c;
}

// returns if two cities are equal 
bool city_eq(CITY a, CITY b) {
	return (a.x == b.x && a.y == b.y);
}

// vector that will hold all the cities 
vector<CITY> cities;

// makes cities (ensuring no repeats) and stores them into cities vector 
void make_cities(int num) {
	set<pair<int, int>> chosen; // set consists of pairs we've already selected
	for (int i = 1; i <= num; i++) {
		int x = rand() % surface_size + 1;
		int y = rand() % surface_size + 1;
		pair<int, int> c = make_pair(x, y);
		while (chosen.find(c) != chosen.end()) { // checks if pair is in the set 
			x = rand() % surface_size + 1;
			y = rand() % surface_size + 1;
			c = make_pair(x, y);
		}
		chosen.insert(c);
		cities.push_back(new_city(i, x, y));
	}
}

// returns distance between two cities 
double city_dist(CITY a, CITY b) {
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

int main()
{
	make_cities(42);
	CITY s = new_city(0, rand() % surface_size + 1, rand() % surface_size + 1);

	for (int i = 0; i < cities.size(); i++) {
		cout << cities[i].num << " " << cities[i].x << " " << cities[i].y << "\n";
	}
	return 0;
}