# mesuring performance of Octave
n = 500;
m = linspace(0,n*n-1,n*n);
m = rem(m, n+1) + 1;
m = reshape(m,n,n);
x = reshape(linspace(1,n*n,n*n),n,n);

printf ("solving %ix%i matrix...\n",n,n);
[t1, u1, s1] = cputime ();

y = m \ x;

[t2, u2, s2] = cputime ();
printf (" Time: %5.2f sec\n", u2 - u1);

#  Time:  6.87 sec, GNU Octave, version 2.0.16 (sparc-sun-solaris2.7).
#y
exit
