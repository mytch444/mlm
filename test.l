; run as:
; ./mlm funcs.l test.l
; so that it has the functions declared in funcs.l.

(define print-out-powers
  (\ (a)
   (cond ((= a 0) (exit))
	 (else (progn
		 (print "2 to the %i is %i" a (pow 2 a))
		 (print-out-powers (- a 1)))))))
(define main
  (\ ()
   (progn (print "Please enter a number:")
	  (print-out-powers (string-to-int (read))))))

(main)
