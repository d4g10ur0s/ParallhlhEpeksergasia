/* Nonlinear Optimization using the algorithm of Hooke and Jeeves  */
/*	12 February 1994	author: Mark G. Johnson 	   */


/* Find a point X where the nonlinear function f(X) has a local    */
/* minimum.  X is an n-vector and f(X) is a scalar.  In mathe-	   */
/* matical notation  f: R^n -> R^1.  The objective function f()    */
/* is not required to be continuous.  Nor does f() need to be	   */
/* differentiable.  The program does not use or require 	   */
/* derivatives of f().						   */

/* The software user supplies three things: a subroutine that	   */
/* computes f(X), an initial "starting guess" of the minimum point */
/* X, and values for the algorithm convergence parameters.  Then   */
/* the program searches for a local minimum, beginning from the    */
/* starting guess, using the Direct Search algorithm of Hooke and  */
/* Jeeves.							   */

/* This C program is adapted from the Algol pseudocode found in    */
/* "Algorithm 178: Direct Search" by Arthur F. Kaupe Jr., Commun-  */
/* ications of the ACM, Vol 6. p.313 (June 1963).  It includes the */
/* improvements suggested by Bell and Pike (CACM v.9, p. 684, Sept */
/* 1966) and those of Tomlin and Smith, "Remark on Algorithm 178"  */
/* (CACM v.12).  The original paper, which I don't recommend as    */
/* highly as the one by A. Kaupe, is:  R. Hooke and T. A. Jeeves,  */
/* "Direct Search Solution of Numerical and Statistical Problems", */
/* Journal of the ACM, Vol. 8, April 1961, pp. 212-229. 	   */

/* Calling sequence:						   */
/*  int hooke(nvars, startpt, endpt, rho, epsilon, itermax)	   */
/*								   */
/*     nvars	   {an integer}  This is the number of dimensions  */
/*		   in the domain of f().  It is the number of	   */
/*		   coordinates of the starting point (and the	   */
/*		   minimum point.)				   */
/*     startpt	   {an array of doubles}  This is the user-	   */
/*		   supplied guess at the minimum.		   */
/*     endpt	   {an array of doubles}  This is the location of  */
/*		   the local minimum, calculated by the program    */
/*     rho	   {a double}  This is a user-supplied convergence */
/*		   parameter (more detail below), which should be  */
/*		   set to a value between 0.0 and 1.0.	Larger	   */
/*		   values of rho give greater probability of	   */
/*		   convergence on highly nonlinear functions, at a */
/*		   cost of more function evaluations.  Smaller	   */
/*		   values of rho reduces the number of evaluations */
/*		   (and the program running time), but increases   */
/*		   the risk of nonconvergence.	See below.	   */
/*     epsilon	   {a double}  This is the criterion for halting   */
/*		   the search for a minimum.  When the algorithm   */
/*		   begins to make less and less progress on each   */
/*		   iteration, it checks the halting criterion: if  */
/*		   the stepsize is below epsilon, terminate the    */
/*		   iteration and return the current best estimate  */
/*		   of the minimum.  Larger values of epsilon (such */
/*		   as 1.0e-4) give quicker running time, but a	   */
/*		   less accurate estimate of the minimum.  Smaller */
/*		   values of epsilon (such as 1.0e-7) give longer  */
/*		   running time, but a more accurate estimate of   */
/*		   the minimum. 				   */
/*     itermax	   {an integer}  A second, rarely used, halting    */
/*		   criterion.  If the algorithm uses >= itermax    */
/*		   iterations, halt.				   */


/* The user-supplied objective function f(x,n) should return a C   */
/* "double".  Its  arguments are  x -- an array of doubles, and    */
/* n -- an integer.  x is the point at which f(x) should be	   */
/* evaluated, and n is the number of coordinates of x.	That is,   */
/* n is the number of coefficients being fitted.		   */

/* rho, the algorithm convergence control			   */
/*	The algorithm works by taking "steps" from one estimate of */
/*    a minimum, to another (hopefully better) estimate.  Taking   */
/*    big steps gets to the minimum more quickly, at the risk of   */
/*    "stepping right over" an excellent point.  The stepsize is   */
/*    controlled by a user supplied parameter called rho.  At each */
/*    iteration, the stepsize is multiplied by rho  (0 < rho < 1), */
/*    so the stepsize is successively reduced.			   */
/*	Small values of rho correspond to big stepsize changes,    */
/*    which make the algorithm run more quickly.  However, there   */
/*    is a chance (especially with highly nonlinear functions)	   */
/*    that these big changes will accidentally overlook a	   */
/*    promising search vector, leading to nonconvergence.	   */
/*	Large values of rho correspond to small stepsize changes,  */
/*    which force the algorithm to carefully examine nearby points */
/*    instead of optimistically forging ahead.	This improves the  */
/*    probability of convergence.				   */
/*	The stepsize is reduced until it is equal to (or smaller   */
/*    than) epsilon.  So the number of iterations performed by	   */
/*    Hooke-Jeeves is determined by rho and epsilon:		   */
/*	    rho**(number_of_iterations) = epsilon		   */
/*	In general it is a good idea to set rho to an aggressively */
/*    small value like 0.5 (hoping for fast convergence).  Then,   */
/*    if the user suspects that the reported minimum is incorrect  */
/*    (or perhaps not accurate enough), the program can be run	   */
/*    again with a larger value of rho such as 0.85, using the	   */
/*    result of the first minimization as the starting guess to    */
/*    begin the second minimization.				   */

/* Normal use: (1) Code your function f() in the C language	   */
/*	       (2) Install your starting guess {or read it in}	   */
/*	       (3) Run the program				   */
/*	       (4) {for the skeptical}: Use the computed minimum   */
/*		      as the starting point for another run	   */

/* Data Fitting:						   */
/*	Code your function f() to be the sum of the squares of the */
/*	errors (differences) between the computed values and the   */
/*	measured values.  Then minimize f() using Hooke-Jeeves.    */
/*	EXAMPLE: you have 20 datapoints (ti, yi) and you want to   */
/*	find A,B,C such that  (A*t*t) + (B*exp(t)) + (C*tan(t))    */
/*	fits the data as closely as possible.  Then f() is just    */
/*	f(x) = SUM (measured_y[i] - ((A*t[i]*t[i]) + (B*exp(t[i])) */
/*				  + (C*tan(t[i]))))^2		   */
/*	where x[] is a 3-vector consisting of {A, B, C}.	   */

/*								   */
/*  The author of this software is M.G. Johnson.		   */
/*  Permission to use, copy, modify, and distribute this software  */
/*  for any purpose without fee is hereby granted, provided that   */
/*  this entire notice is included in all copies of any software   */
/*  which is or includes a copy or modification of this software   */
/*  and in all copies of the supporting documentation for such	   */
/*  software.  THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT    */
/*  ANY EXPRESS OR IMPLIED WARRANTY.  IN PARTICULAR, NEITHER THE   */
/*  AUTHOR NOR AT&T MAKE ANY REPRESENTATION OR WARRANTY OF ANY	   */
/*  KIND CONCERNING THE MERCHANTABILITY OF THIS SOFTWARE OR ITS    */
/*  FITNESS FOR ANY PARTICULAR PURPOSE. 			   */
/*								   */


//ayto einai omp tasks

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <omp.h>

#define MAXVARS		(250)	/* max # of variables	     */
#define RHO_BEGIN	(0.5)	/* stepsize geometric shrink */
#define EPSMIN		(1E-6)	/* ending value of stepsize  */
#define IMAX		(5000)	/* max # of iterations	     */

/* global variables */
unsigned long funevals = 0;


/* Rosenbrocks classic parabolic valley ("banana") function */
double f(double *x, int n)
{
    double fv;
    int i;

	  funevals++;
    fv = 0.0;
      for (i=0; i<n-1; i++){
        fv = fv + 100.0*pow((x[i+1]-x[i]*x[i]),2) + pow((x[i]-1.0),2);
        //printf("%i\n",omp_get_thread_num() );
    }   /* rosenbrock */


    return fv;
}

/* given a point, look for a better one nearby, one coord at a time */
double best_nearby(double delta[MAXVARS], double point[MAXVARS], double prevbest, int nvars)
{
	double z[MAXVARS];
	double minf, ftmp;
	int i;
	minf = prevbest;
	for (i = 0; i < nvars; i++)
		z[i] = point[i];

	for (i = 0; i < nvars; i++) {
		z[i] = point[i] + delta[i];
		ftmp = f(z, nvars);
		if (ftmp < minf)
			minf = ftmp;
		else {
			delta[i] = 0.0 - delta[i];
			z[i] = point[i] + delta[i];
			ftmp = f(z, nvars);
			if (ftmp < minf)
				minf = ftmp;
			else
				z[i] = point[i];
		}
	}
	for (i = 0; i < nvars; i++)
		point[i] = z[i];

  //printf("%lf\n", minf);
	return (minf);
}


int hooke(int nvars, double startpt[MAXVARS], double endpt[MAXVARS], double rho, double epsilon, int itermax)
{
  //ena thread
	double delta[MAXVARS];
	double newf, fbefore, steplength;//h tmp dhlwnetai parakatw
	double xbefore[MAXVARS], newx[MAXVARS];
	int i, j, keep;
	int iters, iadj;
  //ena thread
  #pragma omp parallel
  {
  #pragma omp single
  #pragma omp taskloop default(shared)
	for (int dag = 0; dag < nvars; dag++) {
    //ola ta threads
		newx[dag] = xbefore[dag] = startpt[dag];
		delta[dag] = fabs(startpt[dag] * rho);
		if (delta[dag] == 0.0)
			delta[dag] = rho;
	}
  //ena thread
	iadj = 0;
	steplength = rho;
	iters = 0;
	fbefore = f(newx, nvars);
	newf = fbefore;
	while ((iters < itermax) && (steplength > epsilon)) {
      iters++;
      iadj++;
#if DEBUG
		printf("\nAfter %5d funevals, f(x) =  %.4le at\n", funevals, fbefore);
		for (j = 0; j < nvars; j++)
			printf("   x[%2d] = %.4le\n", j, xbefore[j]);
#endif
		/* find best new point, one coord at a time */
		for (i = 0; i < nvars; i++) {
      newx[i] = xbefore[i];
		}
		newf = best_nearby(delta, newx, fbefore, nvars);
		/* if we made some improvements, pursue that direction */
		keep = 1;
    //ena thread

    while ((newf < fbefore) && (keep == 1)) {
      /*
      metavlhtes provlhma --->  newf,fbefore,keep (kammia den einai pinakas)
      dhmiourgia topikwn metavlhtwn kai sto telos ths while 8etw
      */
      //ola ta threads
      #pragma omp task
      keep = 1;
      int mkeep = 1;//arxikopoihsh dikou mou keep
      double ffbefore,newff;
      double temp_delta[MAXVARS];//diko m delta
      double xxbefore[MAXVARS], newxx[MAXVARS];//dika m xbefore kai newx
      #pragma omp critical
      {
        ffbefore = fbefore;
        newff = newf;
        memcpy(&temp_delta,&delta,sizeof(delta));
        memcpy(&xxbefore,&xbefore,sizeof(xbefore));
        memcpy(&newxx,&newx,sizeof(newx));
      }


    	iadj = 0;
			for (int k = 0; k < nvars; k++) {
				/* firstly, arrange the sign of delta[] */
        //ena vlepei etsi kai alliws
				if (newxx[k] <= xxbefore[k])
					temp_delta[k] = 0.0 - fabs(temp_delta[k]);
				else
					temp_delta[k] = fabs(temp_delta[k]);
        //ena vlepei etsi kai alliws
				/* now, move further in this direction */
				double mtmp = xxbefore[k];
				xxbefore[k] = newxx[k];
				newxx[k] = newxx[k] + newxx[k] - mtmp;
			}

      ffbefore = newff;//edw ti paizei?
      newff = best_nearby(temp_delta, newxx, ffbefore, nvars);// edw?

      /* if the further (optimistic) move was bad.... */
			if (newff >= ffbefore){
        #pragma omp critical
        {
          newf = newff;
          fbefore = ffbefore;
          keep = mkeep;
          memcpy(&delta,&temp_delta,sizeof(temp_delta));
          memcpy(&xbefore,&xxbefore,sizeof(xxbefore));
          memcpy(&newx,&newxx,sizeof(newxx));
        }
        break;
      }

			/* make sure that the differences between the new */
			/* and the old points are due to actual */
			/* displacements; beware of roundoff errors that */
			/* might cause newf < fbefore */
			mkeep = 0;
			for (int k = 0; k < nvars; k++) {
				mkeep = 1;
				if (fabs(newxx[k] - xxbefore[k]) > (0.5 * fabs(temp_delta[k])))
					break;
				else
					mkeep = 0;
			}
      #pragma omp critical
      {
        keep = mkeep;
        newf = newff;
        fbefore = ffbefore;
        memcpy(&delta,&temp_delta,sizeof(temp_delta));
        memcpy(&xbefore,&xxbefore,sizeof(xxbefore));
        memcpy(&newx,&newxx,sizeof(newxx));
      }//sthn arxh 8a dei to gnhsio keep kai delta
		}
    //end while
    //ena thread
		if ((steplength >= epsilon) && (newf >= fbefore)) {
			steplength = steplength * rho;
			for (int k = 0; k < nvars; k++) {
				delta[k] *= rho;
			}
		}
	}
	for (int k = 0; k < nvars; k++)
		endpt[k] = xbefore[k];

  }
	return (iters);
}


double get_wtime(void)
{
    struct timeval t;

    gettimeofday(&t, NULL);

    return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}

int main(int argc, char *argv[])
{
	double startpt[MAXVARS], endpt[MAXVARS];
	int itermax = IMAX;
	double rho = RHO_BEGIN;
	double epsilon = EPSMIN;
	int nvars;
	int trial, ntrials;
	double fx;
	int i, jj;
	double t0, t1;

	double best_fx = 1e10;
	double best_pt[MAXVARS];
	int best_trial = -1;
	int best_jj = -1;

	for (i = 0; i < MAXVARS; i++) best_pt[i] = 0.0;

	ntrials = 128*1024;	/* number of trials */
	nvars = 16;		/* number of variables (problem dimension) */
	srand48(time(0));

  omp_set_num_threads(atoi(argv[1]));//posa threads

	t0 = get_wtime();
  //#pragma omp parallel
  
  //exw mono ena thread na trexei, ayto leei sta ypoloipa pou perimenoun pote na drasoun
	for (trial = 0; trial < ntrials; trial++) {
		/* starting guess for rosenbrock test function, search space in [-4, 4) */
		for (i = 0; i < nvars; i++) {
			startpt[i] = 4.0*drand48()-4.0;
		}

		jj = hooke(nvars, startpt, endpt, rho, epsilon, itermax);
#if DEBUG
		printf("\n\n\nHOOKE %d USED %d ITERATIONS, AND RETURNED\n", trial, jj);
		for (i = 0; i < nvars; i++)
			printf("x[%3d] = %15.7le \n", i, endpt[i]);
#endif

		fx = f(endpt, nvars);
#if DEBUG
		printf("f(x) = %15.7le\n", fx);
#endif
		if (fx < best_fx) {
			best_trial = trial;
			best_jj = jj;
			best_fx = fx;
			for (i = 0; i < nvars; i++)
				best_pt[i] = endpt[i];
		}
	}
	t1 = get_wtime();

	printf("\n\nFINAL RESULTS:\n");
	printf("Elapsed time = %.3lf s\n", t1-t0);
	printf("Total number of trials = %d\n", ntrials);
	printf("Total number of function evaluations = %ld\n", funevals);
	printf("Best result at trial %d used %d iterations, and returned\n", best_trial, best_jj);
	for (i = 0; i < nvars; i++) {
		printf("x[%3d] = %15.7le \n", i, best_pt[i]);
	}
	printf("f(x) = %15.7le\n", best_fx);

	return 0;
}
