#include <iostream>

class Test {

public:
    int value;
    Test (){
	      printf ("Default construction %p \n" , this);
    }
    Test (const Test& t){
	      printf ("Copy construction.Copy %p in  %p \n" ,&t, this); 
    }
    Test (Test&& t){
	      printf ("Move construction. Move %p in  %p \n" ,&t, this);
    }
    ~Test (){
	      printf ("Destruction %p \n" , this);
    }
};

Test factory (int i){

  if (i % 10){
	    Test t;
	    t.value = i /23;
	    return t;
  } else {
	    Test t;
	    t.value = 45;
	    return t;
    }
}

int main (){
    Test t2 {factory (100)}; 
}
