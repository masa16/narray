# mesuring performance of Octave

n = 500;

a = linspace(0,n*n-1,n*n);
a = rem(a, n+1) + 1;
a = reshape(a,n,n);

b = linspace(0,n*n-1,n*n);
b = rem(b, n-1) + 1;
b = reshape(b,n,n);

printf ("executing product of %ix%i Matrix...\n",n,n);
printf ("c=a*b\n");

[t1, u1, s1] = cputime ();
c = a*b;
[t2, u2, s2] = cputime ();

printf (" Time: %f\n", u2 - u1);
# Time: 7.280000
# GNU Octave, version 2.0.16 (sparc-sun-solaris2.7).
exit
