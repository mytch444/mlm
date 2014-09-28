; Some functions for file stuff, look at readfile.l and stdin.l

(define cut-trailing-newline
  (\ (l)
   (cond ((nil? l) ())
	 ((and (= (car l) '\n') (= (car (cdr l)) '\0')) "")
	 (else (cons (car l) (cut-trailing-newline (cdr l)))))))

(define readln
  (\ (f)
   (cut-trailing-newline (read f 100 '\n'))))

(define read-file-h
  (\ (f l)
   (cond ((nil? l) ())
	 (else (concat l
		      (read-file-h f (readln f)))))))

(define read-file
  (\ (f)
   (read-file-h f (readln f))))

(define include-h
  (\ (f line)
   (cond ((nil? line) ())
	 (else (progn (print "got line: %s" line)
		      (eval line)
		      (include-h f (readln f)))))))

(define include-step-h
  (\ (f)
   (include-h f (readln f))))

(define include
  (\ (name)
   (include-step-h (open name "r"))))
