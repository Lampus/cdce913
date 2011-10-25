#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define FREQ_IN 27000000
#define FREQ_125MHZ 125000000
#define FREQ_150MHZ 150000000
#define FREQ_175MHZ 175000000
#define FREQ_MAX 230000000
#define FREQ_MIN 80000000

char verbose_flag = 0;
char debug_flag = 0;

union pll_conf {
	struct {
		unsigned vco_range: 2;
		unsigned p: 3;
		unsigned q: 6;
		unsigned r: 9;
		unsigned n: 12;
	};
	unsigned int data;
};

void print_usage(char *prg_name, FILE *stream)
{
	fprintf(stream, "Usage: %s [options] vco_frequency\n"
					"Arguments:\n"
					"\tThe VCO frequency must be set in Hz, where\n"
					"\t%d<=Fvco<=%d;\n\n"
					"Options:\n"
					"\t-h, --help\t\tPrint this help\n"
					"\t-v, --verbose\t\tVerbose output\n"
					"\t-d, --debug\t\tPrint debug information\n"
					"\t-f, --file <file>\tSet output file\n"
					, prg_name, FREQ_MIN, FREQ_MAX);
	if(stream == stdout)
		exit(0);
	else
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

int coeffs_are_valid(union pll_conf pc)
{
	if((pc.q >= 16) && ((pc.q <= 63)))
		return 1;
		
	return 0;
}

union pll_conf calc_coeffs(unsigned int n, unsigned int m, unsigned int *real_fvco)
{
	unsigned long long fvco;
	unsigned int tip2;
	int sp;
	union pll_conf pc;
	
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

union pll_conf find_coeffs(unsigned int fvco)
{
	unsigned long long br;
	unsigned int real_fvco, br_q, br_r, n, m, min_n, min_m;
	unsigned int min_err=0xFFFFFFFF;
	int err;
	union pll_conf pc, min_pc;
	
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
		if(debug_flag) {
			err = fvco - real_fvco;
			fprintf(stderr, "[%d;%d] Err=%d Hz; Fvco=%d Hz; p=%d; q=%d; r=%d; "
					"VCO Range: %d; Valid: %s;\n", n, m, err, fvco ,pc.p, pc.q, pc.r,\
							pc.vco_range, coeffs_are_valid(pc) ? "yes" : "no");
		}
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
	
	if(verbose_flag)
		fprintf(stderr, "Result: [%d;%d] Err=%u Hz; ", min_n, min_m, min_err);
	
	return min_pc;
}

void write_coeffs_to_file(char *filename, union pll_conf *pc)
{
	FILE *fd;
	
	fd = fopen(filename, "w");
	if(fd == NULL) {
		fprintf(stderr, "Can't open file\n");
		exit(1);
	}
	fprintf(fd, "%08X", pc->data);
	fclose(fd);
}

int main(int argc, char **argv)
{
	unsigned int fvco;
	char *filename = NULL;
	int opt;
	union pll_conf pc;

	struct option longopts[] = {
		{"verbose", 0, NULL, 'v'},
		{"debug", 0, NULL, 'd'},
		{"file", 1, NULL, 'f'},
		{"help", 0, NULL, 'h'},
		{0,0,0,0}
	};
	
	while((opt = getopt_long(argc, argv, "vdhf:", longopts, NULL)) != -1) {
		switch(opt) {
			case 'h':
				print_usage(argv[0], stdout);
				break;
			case 'f':
				filename = (char *)malloc(sizeof(char)*(strlen(optarg) + 1));
				strcpy(filename, optarg);
				break;
			case 'v':
				verbose_flag = 1;
				break;
			case 'd':
				debug_flag = 1;
				break;
			case '?':
				print_usage(argv[0], stderr);
				break;
		}
	}
	
	while (optind < argc) {
		if(sscanf(argv[optind++], "%u", &fvco) != 1)
			print_usage(argv[0], stderr);
		else if((fvco < FREQ_MIN) || (fvco > FREQ_MAX))
			print_usage(argv[0], stderr);
	}
            
	pc = find_coeffs(fvco);
	if(verbose_flag)
		fprintf(stderr, "Fvco=%d Hz; p=%d; q=%d; r=%d; VCO Range: %d; Valid: %s;\n",\
		fvco , pc.p, pc.q, pc.r, pc.vco_range, coeffs_are_valid(pc) ? "yes" : "no");
		
	if(filename != NULL) {
		write_coeffs_to_file(filename, &pc);
	}
		
	return 0;
}
