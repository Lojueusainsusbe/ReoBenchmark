set xlabel 'Size'
set ylabel 'Average Time (s)'
set term pdf dashed
set output '../report/img/out-seq.pdf'
set key left top

set style data linespoints
set dashtype 1 '-..'

set style line 1 lt 1 lw 3 pt 13 linecolor rgb "red"
set style line 2 lt 1 lw 3 pt 13 linecolor rgb "blue"

set logscale x 2
#set logscale y 2

plot    'graph.data' using 1:2 title 'C Sequencer' with linespoints ls 1, 'graph.data' using 1:3 title 'Reo Sequencer' with linespoints ls 1 dt 1, 'graph.data' using 1:6 title 'C Early Async Replicator' with linespoints ls 2, 'graph.data' using 1:7 title 'Reo Early Async Replicator' with linespoints ls 2 dt 1
