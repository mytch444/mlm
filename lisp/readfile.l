(include "funcs.l")
(include "iofuncs.l")

(define print-file-h
  (\ (f l)
   (cond ((nil? l) ())
	 (else (progn (print "%s" l)
		      (print-file-h f (readln f)))))))

(define print-file
  (\ (f)
   (print-file-h f (readln f))))

(define main
  (\ ()
   (progn (print-file (open "test.l" "r"))
	  (exit))))

(main)
