#!/bin/sed -nf
#Drops wikipedia redirect pages from wiki markup xml
#Written by: Michael Agun
#Last Updated: 2/21/2014
/<page>/!d;
/<page>/{
h;
:a;
n;
/<\/page>/b b;
/<redirect[^>]*\/>/d;
/{{[ \t]*[Rr]edirect[ \t]*|[^}]*}}/d;
H;b a;
:b;
x;G;p;
}
