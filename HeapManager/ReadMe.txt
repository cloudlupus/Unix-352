David Shagam
Heap manager Unix 352
Algorithms implemented, quick using worst heap, worst heap, best heap.

How to run
	run ./RunMe.sh
	this will make all 
	and then run best fit, worst fit, and quick fit
	Best fit outputs to bestOutput.txt
	Worst fit outputs to worstOutput.txt
	and quick fit outputs to quickOutput.txt

There is no toggling off the prints for how long things take in CPU time this always outputs.
Set #DEFINE DEBUG to 0 to turn off the free list printing and block info printing. set it to 1 to enable it.
this must be manually done for bestheap352.c worstheap352.c and quickheap352.c


Time comparisons
	Quick tends to run much faster than best and worst fits however it doesn't all recoalesce at the end ALL data is present and accounted for. But it ends with a lot of fragmentation
		in my runnings with print off (because print is what takes the most time and quick prints more data than best and worst so it's unfair comparison) it takes about 0.01 clock time to finish. Where as best and worst take between 0.02 to 0.04.

	Worst tends to be about equal to best in run time with printing off or on but it does lean towards being a little bit faster.
		it runs somewhere between 0.02 to 0.04 wall clock time to complete.
		Benefits of this are there are a lot less tiny unusable fragments

	Best tends to be slightly slower than worst fit but sometimes equal
		it tends to run on the 0.03 to 0.04 range but sometimes as low as 0.02
		downside lots of tiny unusable fragments

SIDE NOTES:
	I expect my best and worst fit cases run about the same amount of time because of a very poorly implemented fragmentation handling code. I don't handle fragmentation nicely. Both best and worst probably go through looking to coalesce a lot more than they need to in an intelligent way.