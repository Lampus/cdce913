#include <stdio.h>
#include <math.h>

double fin=27.0;

double m_log2(int x)
{
    return (log(x)*M_LOG2E);
}

int calc_nstroke(int n, int p)
{
    return n*(int)pow(2.0, p);
}

int calc_p(int n, int m)
{
    int result;
    result=4 - (int)floor(m_log2((double)n/(double)m));
    if(result<0)
	result=0;
    return result;
}

int calc_q(int nstroke, int m)
{
    return (int)floor((double)nstroke/(double)m);
}

int calc_r(int nstroke, int m, int q)
{
    return nstroke-m*q;
}

double calc_fvco(double fin, int m, int n)
{
    return fin*((double)n/(double)m);
}

char is_valid(double fvco, int q, int p, int r)
{
    if(!(fvco>=80.0&&fvco<=230.0))
    	return 0;
    if(!(q>=16&&q<=63))
	return 0;
    if(!(p>=0&&p<=7))
	return 0;
    if(!(r>=0&&r<=511))
    	return 0;
    return 1;
}

double calc_br(int n, int m)
{
    return (double)n/(double)m;
}

double calc_target_br(double fvco, double fin)
{
    return fvco/fin;
}

void find_best_coeffs(double fvco)
{
    double br, fvco_err;
    double min_fvco_err=100.0;
    int min_n, min_m, min_p, min_q, min_r;
    int n, m, nstroke, p, q , r;

    br=calc_target_br(fvco, fin);
    //printf("Fvco=%f; target BR: %f\n", fvco, br);
    for(m=511; m>0; m--)
    {
	n=(int)((double)m*br);
	if(n>4095)
	    continue;
	fvco_err=fabs(calc_fvco(fin, m, n)-fvco);
	p=calc_p(n, m);
	nstroke=calc_nstroke(n, p);
	q=calc_q(nstroke, m);
	r=calc_r(nstroke, m, q);
	if(is_valid(fvco, q, p, r))
	{
	    if(fvco_err<min_fvco_err)
	    {
		min_fvco_err=fvco_err;
		min_n=n;
		min_m=m;
		min_p=p;
		min_q=q;
		min_r=r;
	    }
	}
    }
    //printf("[N=%d|M=%d] Fvco=%0.3f, P=%d, Q=%d, R=%d\n", min_n, min_m, fvco, min_p, min_q, min_r);
    //printf("Error: %f (%.2f %%)\n", min_fvco_err, (min_fvco_err/fvco)*100.0);
    printf("%0.6f;%0.3f;%d;%d;%d;[%d;%d]\n", fvco, min_fvco_err*1000.0, min_p, min_q, min_r, min_n, min_m);
}

int main(int argc, char **argv)
{
    double fvco;
    //printf("Enter Fvco> ");
    //scanf("%lf", &fvco);
    for(fvco=80.0; fvco<=230.0; fvco += 0.213471)
        find_best_coeffs(fvco);
    return 0;
}
