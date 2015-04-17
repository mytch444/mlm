(include "iofuncs.l")

(define main
  (\ (stdin)
   (progn (print "Enter something:")
	  (print "%s" (readln stdin))
	  (main stdin))))

(define enter
  (\ ()
   (main (open "/dev/stdin" "r"))))

(enter)
