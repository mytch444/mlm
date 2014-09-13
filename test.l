(define else
  (\ () 1))

(define 1+
  (\ (a)
   (+ a 1)))

(define 1-
  (\ (a)
   (- a 1)))

(define twice
  (\ (a)
   (* 2 a)))

(define pow
  (\ (a b)
   (cond ((= b 0) 1)
	 ((else) (* a
		  (pow a (- b 1)))))))

(define map
  (\ (f l)
   (cond ((nil? l) ())
	 ((else) (cons
		(f (car l))
		(map f (cdr l)))))))

(define n
  (\ (i l)
   (cond ((= i 0) (car l))
	 ((else) (n (- i 1) (cdr l))))))

(define concat
  (\ (c l)
   (cond ((nil? l) c)
	 ((else) (cons (car l) (concat c (cdr l)))))))
