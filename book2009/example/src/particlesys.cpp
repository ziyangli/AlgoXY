#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

const bool INFECTED = true;
const int  MAX_SPEED = 50; // walking speed: 50 [m/min]
const double dt = 1;       // 1 min
const double pi = 3.141592654;

class area{
public:
  area(){}
  area(int w, int h):_width(w), _height(h), cells(w*h, 0){ }
  virtual ~area(){}

  int width(){ return _width; }
  int height() { return _height; }

  std::vector<int>::iterator at(int x, int y){
    return cells.begin()+y*_width+x;
  }

  void reset(){
    for(std::vector<int>::iterator it=cells.begin(); it!=cells.end(); ++it)
      *it=0;                    // change to foreach
  }

private:
  int _width;
  int _height;
  std::vector<int> cells;
};

struct physics{
  physics(){}
  physics(int _x, int _y, int _s, int _d):x(_x), y(_y), speed(_s), direction(_d){}
  physics(const physics& p):x(p.x), y(p.y), speed(p.speed), direction(p.direction){}
  physics& operator=(const physics& p){
    x=p.x; y=p.y;
    speed=p.speed; direction = p.direction;
  }

  bool operator==(const physics& p){
    return x==p.x && y==p.y && speed== p.speed && direction == p.direction;
  }

  void move(double dt){
    x+=static_cast<int>(static_cast<double>(speed)*dt*cos(static_cast<double>(direction)/180.0*pi));
    y+=static_cast<int>(static_cast<double>(speed)*dt*sin(static_cast<double>(direction)/180.0*pi));
  }

  int x;
  int y;
  int speed;
  int direction;
};

class person{
public:
  person(bool x=false):_infected(x){}
  person(const person& p):_infected(p._infected), _loc(p._loc){}
  person& operator=(const person& p){
    _infected = p._infected;
    _loc = p._loc;
    return *this;
  }
  bool operator==(const person& p){ //??? differnt person?
    return _infected==p._infected && _loc == p._loc;
  }

  physics location() const{ return _loc; }
  void set_location(const physics& l){ _loc=l; }
  bool infected() const { return _infected; }
  void set_infected(bool x) { _infected = x; }

  void move_inside(area& a, double dt){
    _loc.move(dt);
    if(_loc.x<0 || _loc.x>a.width())
      _loc.direction = (180 - _loc.direction + 360) % 360;
    if(_loc.y<0 || _loc.y>a.height())
      _loc.direction = (-_loc.direction + 360) % 360;
    if(_loc.x<0)
      _loc.x=-_loc.x;
    if(_loc.x>a.width())
      _loc.x=2*a.width()-_loc.x;
    if(_loc.y<0)
      _loc.y=-_loc.y;
    if(_loc.y>a.height())
      _loc.y=2*a.height()-_loc.y;
  }

private:
  bool _infected;
  physics _loc;
};

class scheduler{
public:
  static scheduler& inst(){
    static scheduler _inst;
    return _inst;
  }

  void setup(int w, int h, int n){
    a=area(w, h);
    people=Population(n-1);
    people.push_back(person(INFECTED));
    put_people(people, a);
    n_infected = 1;
  }

  void run(){
    while(n_infected < people.size()*90/100){
      a.reset();
      move();
      infect();
      std::cout<<n_infected<<"/"<<people.size()<<" are infected\n";
    }
  }

private:
  scheduler(){}

  void move(){
    for(Population::iterator it=people.begin(); it!=people.end(); ++it){
      it->move_inside(a, dt);   // change to foreach
      if(it->infected())
        *(a.at(it->location().x, it->location().y))=1;
    }
  }

  void infect(){
    for(Population::iterator it=people.begin(); it!=people.end(); ++it)
      if(*(a.at(it->location().x, it->location().y))){
        n_infected += it->infected()?0:1;
        it->set_infected(true);   // change to foreach
      }
  }

  template<class Coll>
  void put_people(Coll& coll, area& a){
    for(typename Coll::iterator it=coll.begin(); it!=coll.end(); ++it){
      it->set_location(physics(rand()%a.width(), rand()%a.height(),
                               rand()%MAX_SPEED, rand()%360));
    }
  }

  area a;
  typedef std::vector<person> Population;
  Population people;
  int n_infected;
};

int main(int argc, char** argv){
  //Beijing has people density as 888 persons/km^2, ==> 1061 m^2 is the best fit
  scheduler::inst().setup(1061, 1061, 1000);
  scheduler::inst().run();
}