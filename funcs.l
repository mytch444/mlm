(define else
  1)

(define >=
  (\ (a b)
   (cond
    ((= a b) 1)
    ((> a b) 1)
    (else ()))))

(define <=
  (\ (a b)
   (cond
    ((= a b) 1)
    ((< a b) 1)
    (else ()))))

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
	 (else (* a
		  (pow a (- b 1)))))))

(define map
  (\ (f l)
   (cond ((nil? l) ())
	 (else (cons
		(f (car l))
		(map f (cdr l)))))))

(define n
  (\ (i l)
   (cond ((= i 0) (car l))
	 (else (n (- i 1) (cdr l))))))

(define concat
  (\ (c l)
   (cond ((nil? c) l)
	 (else (cons (car c) (concat (cdr c) l))))))

(define not
  (\ (a)
   (cond ((nil? a) 1)
	 (else ()))))

(define !=
  (\ (a b)
   (not (= a b))))

(define and
  (\ (a b)
   (cond
    (a (cond
	(b 1)
	(else ())))
    (else ()))))

(define or
  (\ (a b)
   (cond
    (a 1)
    (b 1)
    (else ()))))

(define char-to-upper
  (\ (c)
   (cond ((< c 'a') c)
	 ((> c 'z') c)
	 (else (- c 32)))))

(define string-to-upper
  (\ (string)
   (map char-to-upper string)))
