#include <stdio.h>

unsigned int ulog2(unsigned int x)
{
	unsigned int n=0;
	if(x&0xFFFF0000) { n += 16; x >>= 16; }
	if(x&0xFFFFFF00) { n += 8; x >>= 8; }
	if(x&0xFFFFFFF0) { n += 4; x >>= 4; }
	if(x&0xFFFFFFFC) { n += 2; x >>= 2; }
	if(x&0xFFFFFFFE) { n += 1; x >>= 1; }
	return n;
}

int main(int argc, char **argv)
{
	unsigned int fin, fvco, n, m, p, q, r, i, tip2;
	int sp;
	fin = 27000000;

	printf("Enter n> ");
	scanf("%d", &n);
	printf("Enter m> ");
	scanf("%d", &m);
	sp = 4 - ulog2(n/m);
	if(sp<0)
		p = 0;
	else
		p = (unsigned int)sp;
	tip2 = 1<<p;
	q = (n * tip2) / m;
	r = n * tip2 - m * q;
	printf("p=%d; q=%d; r=%d;\n", p, q, r);
	return 0;
}
