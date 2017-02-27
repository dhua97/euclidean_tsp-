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

bool operator< (CITY a, CITY b) { return a.num < b.num; }
/* needed to use structs as keys in map? OHHH its because maps are implemented as BSTs,
so if you don't specify comparison function for the struct it'll break */

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

// returns  the powerset of a given set of cities 
vector<vector<CITY>> gen_subsets(vector<CITY> all_cities) {
	vector<vector<CITY>> subsets;
	vector<CITY> Empty;
	subsets.push_back(Empty);

	for (int i = 0; i < all_cities.size(); i++) {
		/* for each iteration, create a clone of the current subset and add the next
		element to every subset. then we combine the subsets with the element and the
		subsets without */ 

		vector<vector<CITY>> clone = subsets;
		for (int j = 0; j < clone.size(); j++) {
			clone[j].push_back(all_cities[i]);
		}
		for (int k = 0; k < clone.size(); k++) {
			subsets.push_back(clone[k]);
		}
	}
	return subsets;
}

// returns all subsets that have the given size k 
vector<vector<CITY>> subsets_k(vector<vector<CITY>> allcities, int k) {
	vector<vector<CITY>> subsets;
	for (int i = 0; i < allcities.size(); i++) {
		if (allcities[i].size() == k) {
			subsets.push_back(allcities[i]);
		}
	}
	return subsets;
}

typedef std::pair<vector<CITY>, CITY> city_key; // key is (vector of middle cities, endcity)
typedef std::pair<vector<CITY>, double> city_value; 
/* value is a pair which returns the mininum distance from the start city to the end city (traversing 
through each of the middle cities) as well as the path */ 

pair<vector<CITY>, double> get_TSP(CITY start, vector<CITY> allcities) {
	map<city_key, city_value> stored_paths;
	vector<vector<CITY>> all_subsets = gen_subsets(allcities);

	/* Our plan is to tackle subproblems and store those results into our map.
	We will divide them based on subset size so we always have the answers to smaller subproblems
	in our map.
	*/

	for (int i = 1; i <= allcities.size(); i++) {
		vector<vector<CITY>> subsets = subsets_k(all_subsets, i);
		if (i == 1) {
			/* initialize our map by considering subsets of size 1. The min-distance from the start city
			to any given city is just the distance between cities (no middle cities) */
			for (int j = 0; j < subsets.size(); j++) {
				vector<CITY> Empty;
				vector<CITY> path;
				path.push_back(start);
				city_key key = make_pair(Empty, subsets[j][0]);
				city_value value = make_pair(path, city_dist(start, subsets[j][0]));
				stored_paths.insert(make_pair(key, value)); // yay fixed!!! :) 
			}
		}
		else {
			/* For subsets greater than 1, we first choose an end city. We then figure out the min path
			from start to the end city through the rest of the cities in the subset, by iterating through
			every possible second-to-last city, and taking the min distance of all possible choices. We
			then update our map with these values for future use
			*/
			for (int j = 0; j < subsets.size(); j++) {
				for (int k = 0; k < subsets[j].size(); k++) {
					CITY endcity = subsets[j][k]; //choose end city
					CITY best_secondlast; // keep track of best second-last city and best distance
					double best_dist = 100000000000; //bad way to do this, could initialize to first seen
					vector<CITY> best_path;
					vector<CITY> one_less = subsets[j]; 
					one_less.erase(one_less.begin() + k); // take out end city, left with middle cities

					for (int l = 0; l < one_less.size(); l++) {
						vector<CITY> rest_of_cities = one_less;
						CITY secondlast = rest_of_cities[l]; //iterate through all possible choices
						rest_of_cities.erase(rest_of_cities.begin() + l);
						city_key check_key = make_pair(rest_of_cities, secondlast); //checking subproblem
						city_value v = stored_paths[check_key];
						// v.first holds the path, v.second holds the distance
						double dist_tolast = city_dist(secondlast, endcity);
						double total_dist = v.second + dist_tolast;

						// we update the best_path and best_dist if it is smaller 
						if (total_dist < best_dist) {
							best_dist = total_dist;
							best_secondlast = secondlast;
							best_path = v.first;
						}
					}
					// store the result of this problem for future use 
					city_key key = make_pair(one_less, endcity);
					best_path.push_back(best_secondlast);
					city_value value = make_pair(best_path, best_dist);
					stored_paths.insert(make_pair(key, value));
				}
			}

		}
	}

	double overall_best = 1000000000; // again bad way to do this 
	vector<CITY> best_path;

	//* We now have solved all subproblems, and just need to iterate through all possible last cities.
	for (int i = 0; i < allcities.size(); i++) {
		vector<CITY> rest = allcities;
		CITY lastcity = allcities[i];
		rest.erase(rest.begin() + i);
		city_value v = stored_paths[make_pair(rest, lastcity)];
		vector<CITY> curr_path = v.first;
		curr_path.push_back(lastcity);
		double end_to_start = city_dist(lastcity, start);
		double total_dist = v.second + end_to_start;

		//update if this distance is smaller 
		if (total_dist < overall_best) {
			overall_best = total_dist;
			best_path = curr_path;
		}
	}
	best_path.push_back(start);
	return make_pair(best_path, overall_best);
}

int main()
{
	make_cities(7); //takes a while on larger inputs 

	//create a random starting location with city num 0
	CITY s = new_city(0, rand() % surface_size + 1, rand() % surface_size + 1);


	pair<vector<CITY>, double> result = get_TSP(s, cities);

	cout << "The total distance is: " << result.second << "\n";
	
	vector<CITY> path = result.first;
	cout << "The order of cities is ";
	for (int i = 0; i < path.size(); i++) {
		cout << path[i].num << " ";
	}
	cout << "\n";
	/*for (int i = 0; i < cities.size(); i++) {
		cout << cities[i].num << " " << cities[i].x << " " << cities[i].y << "\n";
	} */
	return 0; 
}