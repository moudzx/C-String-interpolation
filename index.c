#include "rprintf.h"

int main(void) {
	let(age, 26);
	let(pi, 3.14159);
	let(grade, 'A');
	let(name, "Moss");

	int scores[] = { 10, 20, 30, 0 };
	double readings[] = { 1.1, 2.2, 3.3, 0.0 };
	watch(scores);
	watch(readings);

	int n;

	n = rprintf("hello {name}, you are {age} years old\n");
	printf("resolved %d variable(s)\n\n", n);

	n = rprintf("pi is {pi}, grade is {grade}\n");
	printf("resolved %d variable(s)\n\n", n);

	n = rprintf("scores: {scores}\n");
	printf("resolved %d variable(s)\n\n", n);

	n = rprintf("repeat: {name} is still {name}\n");
	printf("resolved %d variable(s)\n\n", n);

	n = rprintf("escape \\{name} and undefined {ghost}\n");
	printf("resolved %d variable(s)\n", n);
  return 69;
}
