#include <stdio.h>
#include <stdlib.h>

#define FREQ_IN 27000000
#define FREQ_125MHZ 125000000
#define FREQ_150MHZ 150000000
#define FREQ_175MHZ 175000000
#define FREQ_MAX 230000000
#define FREQ_MIN 80000000

struct pll_conf {
	unsigned vco_range: 2;
	unsigned p: 3;
	unsigned q: 6;
	unsigned r: 9;
	unsigned n: 12;
};

void print_usage(char *prg_name)
{
	fprintf(stderr, "Usage: %s <Fvco>\n"
					"Where\n"
					"\tFvco - VCO frequency in Hz;\n"
					"\t%d<=Fvco<=%d;\n", prg_name, FREQ_MIN, FREQ_MAX);
	exit(1);
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
	if((pc.q >= 16) && ((pc.q <= 63)))
		return 1;
		
	return 0;
}

struct pll_conf calc_coeffs(unsigned int n, unsigned int m, unsigned int *real_fvco)
{
	unsigned long long fvco;
	unsigned int tip2;
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
	*real_fvco = (unsigned int)fvco;
	if(fvco < FREQ_125MHZ)
		pc.vco_range = 0;
	else if((fvco >= FREQ_125MHZ) && (fvco < FREQ_150MHZ))
		pc.vco_range = 1;
	else if((fvco >= FREQ_150MHZ) && (fvco < FREQ_175MHZ))
		pc.vco_range = 2;
	else
		pc.vco_range = 3;
			
	return pc;
}

struct pll_conf find_coeffs(unsigned int fvco)
{
	unsigned long long br;
	unsigned int real_fvco, br_q, br_r, n, m, min_n, min_m;
	unsigned int min_err=0xFFFFFFFF;
	int err;
	struct pll_conf pc, min_pc;
	
	br_q = fvco / FREQ_IN;
	br_r = fvco - br_q * FREQ_IN;
	for(m = 511; m > 0; m--)
	{
		br = (unsigned long long)m * br_r * 10000 / FREQ_IN / 1000;
		if((br - ((br / 10) * 10)) >= 5)
			br = br / 10 + 1;
		else
			br = br / 10;
		n = m * br_q + (unsigned int)br;
		if(n > 4095)
			continue;
		pc = calc_coeffs(n, m, &real_fvco);
		if(coeffs_are_valid(pc)) {
			err = fvco - real_fvco;
			if(err < 0)
				err = real_fvco - fvco;
			if(err < min_err) {
				min_err = err;
				min_pc = pc;
				min_n = n;
				min_m = m;
			}
		}
	}
	
	printf("[%d;%d] Err=%u Hz; ", min_n, min_m, min_err);
	
	return min_pc;
}

int main(int argc, char **argv)
{
	unsigned int fvco;
	struct pll_conf pc;
	
	if(argc != 2)
		print_usage(argv[0]);
		
	if(sscanf(argv[1], "%u", &fvco) != 1)
		print_usage(argv[0]);
	else if((fvco < FREQ_MIN) || (fvco > FREQ_MAX))
		print_usage(argv[0]);
	
	pc = find_coeffs(fvco);
	printf("Fvco=%d Hz; p=%d; q=%d; r=%d; VCO Range: %d; Valid: %s;\n", fvco ,\
		pc.p, pc.q, pc.r, pc.vco_range, coeffs_are_valid(pc) ? "yes" : "no");
		
	return 0;
}
