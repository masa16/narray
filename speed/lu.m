# mesuring performance of Octave

n = 1000;
m = linspace(0,n*n-1,n*n);
m = rem(m, n+1) + 1;
m = reshape(m,n,n);

printf ("executing %ix%i LU...\n",n,n);
[t1, u1, s1] = cputime ();
[l,u,p] = lu(m);
[t2, u2, s2] = cputime ();
printf (" Time: %5.2f sec\n", u2 - u1);
# Time: 20.43 sec, GNU Octave, version 2.0.16 (sparc-sun-solaris2.7).
exit
