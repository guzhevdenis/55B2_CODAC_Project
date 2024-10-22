#include <iostream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
void show(std::vector<int>& a);
void create_range(std::vector<int>& a, int min, int max, int range);
void count_numbers(std::vector<int>&a, std::vector<int>&b, std::vector<int>& c, int range);
void create_histogram (std::vector<int>&a , std::vector<int>& b, std::vector<int>& c, int range);
int main ()
{
    std::vector<int> a{10,0,10,0,0,0,0,1,1,1,2,0,0,2};
    show(a);
    std::vector<int> b; 
    std::vector<int> c;
    create_histogram (a , b,  c,  3);

    show(b);
    show(c);
    return 0;
}

void create_histogram (std::vector<int>&a , std::vector<int>& b, std::vector<int>& c, int range)
{
    std::vector<int>::iterator min = std::min_element(a.begin(), a.end());
    std::vector<int>::iterator max = std::max_element(a.begin(), a.end());
    create_range(b,*min, *max, range);
    count_numbers(a,b,c, range);
}
void show(std::vector<int>& a)
{
    for (int i = 0; i < a.size(); ++i )
    {
        std::cout<<a[i]<<" ";
    }
    std::cout<<std::endl;
}

void create_range(std::vector<int>& a, int min, int max, int range)
{
    for (int i = min; i <= max; i = i + range)
    {
        a.push_back(i);
    }
}

void count_numbers(std::vector<int>&a, std::vector<int>&b, std::vector<int>& c, int range)
{
    //int range = b[1]-b[0];
    for (int i = 0; i < b.size(); ++i)
    {
        int counter = 0;
        for (int j = 0; j < range; ++j)
        {
            counter = counter + std::count(a.begin(), a.end(),b[i]+j); //optimize it
        }
        c.push_back(counter);
    }
}