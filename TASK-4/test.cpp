#include<iostream>
#include"tmpl.hpp"
#include"virt.hpp"


using namespace std;
//现在这个test还没有传入帧
int main(){
   tmpl::preprocess();
   virt::preprocess();
   return 0;
}