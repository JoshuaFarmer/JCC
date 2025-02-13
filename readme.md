<h1>Josh's C Compiler (JCC)</h1>
this is my first (bad) C compiler
it works without an ast, as such order of operations is a bit whack<br>
but thats not the point, so its fine<br>

<h2>"Features"</h2>
all logical && arithmetic operations (except for by LHS e.g. +=)<br>
extern (ignores args)<br>
functions/function arguments (NOTE: passed backwards because idfk)<br>
shorts, integers, chars and pointers<br>
return<br>
No error correction and NO warnings<br>
also no return types<br>

TARGETS:<br>
i386 100% *<br>
i8085 50% *<br>
Astro-8 0%