#include <stdio.h>
#include <stdlib.h>

#define FREQ_IN 27000000
#define FREQ_125MHZ 125000000
#define FREQ_150MHZ 150000000
#define FREQ_175MHZ 175000000
#define MAX_N ((1<<12) - 1)
#define MAX_M ((1<<9) - 1)

struct pll_conf {
	unsigned vco_range: 2;
	unsigned p: 3;
	unsigned q: 6;
	unsigned r: 9;
	unsigned n: 12;
};

void print_usage(char *prg_name)
{
	fprintf(stderr, "Usage: %s <N> <M>\n"
					"Where\n"
					"\t1<=N<=%d;\n"
					"\t1<=M<=%d;\n", prg_name, MAX_N, MAX_M);
	exit(1);
}

inline int inputs_are_valid(unsigned int n, unsigned int m)
{
	if((n >= 1) && (n <= MAX_N))
		if((m >= 1) && (m <= MAX_M))
			if(n >= m)
				return 1;
	
	return 0;
}

inline unsigned int ulog2(unsigned int x)
{
	unsigned int n=0;
	
	if(x&0xFFFF0000) { n += 16; x >>= 16; }
	if(x&0xFFFFFF00) { n += 8; x >>= 8; }
	if(x&0xFFFFFFF0) { n += 4; x >>= 4; }
	if(x&0xFFFFFFFC) { n += 2; x >>= 2; }
	if(x&0xFFFFFFFE) { n += 1; x >>= 1; }
	
	return n;
}

int coeffs_are_valid(struct pll_conf pc)
{
	int ret;
	
	if((pc.q >= 16) && ((pc.q <= 63)))
		if((pc.r >= 0) && ((pc.r <= 51)))
			return 1;
		
	return 0;
}

inline void calc_coeffs(unsigned int n, unsigned int m)
{
	unsigned long long fvco;
	unsigned int i, tip2;
	int sp;
	struct pll_conf pc;
	
	sp = 4 - ulog2(n / m);
	if(sp < 0)
		pc.p = 0;
	else
		pc.p = (unsigned int)sp;
	tip2 = 1<<pc.p;
	pc.q = (n * tip2) / m;
	pc.r = n * tip2 - m * pc.q;
	pc.n = n;
	fvco = (unsigned long long)FREQ_IN * n / m;	
	if(fvco < FREQ_125MHZ)
		pc.vco_range = 0;
	else if((fvco >= FREQ_125MHZ) && (fvco < FREQ_150MHZ))
		pc.vco_range = 1;
	else if((fvco >= FREQ_150MHZ) && (fvco < FREQ_175MHZ))
		pc.vco_range = 2;
	else
		pc.vco_range = 3;
		
	printf("Fvco=%lld Hz; p=%d; q=%d; r=%d; VCO Range: %d; Valid: %s\n", fvco , \
			pc.p, pc.q, pc.r, pc.vco_range, coeffs_are_valid(pc) ? "yes" : "no");
}

void find_coeffs(unsigned long long fvco, unsigned int *n, unsigned int *m)
{
	*m = 1;
	*n = fvco / FREQ_IN;
	printf("pN=%u; pM=%u\n", *n, *m);
}

int main(int argc, char **argv)
{
	unsigned int n, m;
	
	if(argc != 3)
		print_usage(argv[0]);
		
	if((sscanf(argv[1], "%d", &n) != 1) ||
		sscanf(argv[2], "%d", &m) != 1) {
		fprintf(stderr, "Invalid coefficient format\n");
		print_usage(argv[0]);
	}
	
	if(inputs_are_valid(n, m))
		calc_coeffs(n, m);
	else
		print_usage(argv[0]);
		
	find_coeffs(199800000, &n, &m);
	printf("dN=%u; dM=%u\n", n, m);	
	return 0;
}
