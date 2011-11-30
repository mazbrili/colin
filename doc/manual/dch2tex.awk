function texquote(s) {
    gsub("_","\\_",s);
    gsub("{","\\{",s);
    gsub("}","\\}",s);
    gsub("\\","\\\\",s);
    return s;
}

BEGIN { inchange=0; }

/^colin \([0-9]+\.[0-9]+\.[0-9]+-[0-9]+\) .*/ {
   printf "\\paragraph{Version %s}\n",substr($2,2,length($2)-2);
   print "\\begin{compactitem}";
   inchange = 1;
}

/^ +\*/ {
    
    if(inchange) {
        sub(/^ +\*/,"\\item",$0);
        print texquote($0);
    }
}

/^ +[^*-]/ {
    
    if(inchange) {
        print texquote($0);
    }
}
/^ -- .*/ {

    print "\\end{compactitem}";
    printf "\\texttt{%s}\n",texquote(substr($0,5));
    inchange = 1;
}
