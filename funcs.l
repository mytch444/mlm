(define else
  1)

(define t
  1)

(define f
  ())

(define >=
  (\ (a b)
   (cond
    ((= a b) t)
    ((> a b) t)
    (else f))))

(define <=
  (\ (a b)
   (cond
    ((= a b) t)
    ((< a b) t)
    (else f))))

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
  (\ (func l)
   (cond ((nil? l) ())
	 (else (cons
		(func (car l))
		(map func (cdr l)))))))

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
   (cond ((nil? a) t)
	 (else f))))

(define !=
  (\ (a b)
   (not (= a b))))

(define and
  (\ (a b)
   (cond
    (a (cond
	(b t)
	(else f)))
    (else f)))

(define or
  (\ (a b)
   (cond
    (a t)
    (b t)
    (else f))))

(define char-to-upper
  (\ (c)
   (cond ((< c 'a') c)
	 ((> c 'z') c)
	 (else (- c 32)))))

(define string-to-upper
  (\ (string)
   (map char-to-upper string)))
