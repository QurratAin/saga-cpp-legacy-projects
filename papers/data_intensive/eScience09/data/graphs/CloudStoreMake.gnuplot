set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStoreFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 20"
set ylabel 't_c(s)' font "Times-Italic, 20"
set mxtics 2
set mytics 5
plot[0:9][0:2000] './localCloudStore.dat' using 1:2 title 'S0: [C1, {/Times-Italic r} = 1]' with lp lw 3,\
'./remoteCloudStore.dat' using 1:2 title 'S1: [C2, {/Times-Italic r} = 1]' with lp lw 3,\
'./localCloudStore2Chunks.dat' using 1:2 title 'S2: [C3, {/Times-Italic r} = 2]' with lp lw 3,\
'./2ChunksReplication1.dat' using 1:2 title 'S3: [C4, {/Times-Italic r} = 1]' with lp lw 3,\
'./2ChunksReplication2.dat' using 1:2 title 'S4: [C4, {/Times-Italic r} = 2]' with lp lw 3 
