; Some functions for file stuff, look at readfile.l and stdin.l

(define cut-trailing-newline
  (\ (l)
   (cond ((nil? l) ())
	 ((= (car l) '\n') "")
	 (else (cons (car l) (cut-trailing-newline (cdr l)))))))

(define readln
  (\ (f)
   (cut-trailing-newline (read f 1000 '\n'))))
