#include <iostream>
#include <ctime>

double monte_carlo_method(long long int toss, long long int period, unsigned int seed){
    long long int circle = 0;
    long long int square = 0;
    long long int start = 0;
    std::srand(seed);
    while(++start <= toss){
        long double x = 2*(long double)std::rand()/RAND_MAX - 1.0;//[-1,1]
        long double y = 2*(long double)std::rand()/RAND_MAX - 1.0;//[-1,1]
        if(x*x + y*y < 1.0){
            ++circle;
        }
        ++square;
        
        if( start % period == 0 ){
            std::cout<< "At iteration " << start<<", the value of pi = "<<4*(long double)circle/square <<std::endl;
        }
    }
    return 4*(long double)circle/square;
}

int main(){
    monte_carlo_method(1000000000, 10000000, 7);
    return 0;
}
