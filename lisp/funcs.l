; Just some functions for doing stuff. Run
; ./mlmi lisp/funcs.l
; To load them into the interperater then you can use these.
; Also checkout test.l

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
	 ((> b 0) (* a
		     (pow a (- b 1))))
	 (else (/ 1.0 (pow a (- 0 b)))))))

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

(define char-to-int
  (\ (a)
   (to-int (- a '0'))))

(define int-to-char
  (\ (a)
   (to-char (+ a '0'))))

(define last
  (\ (l)
   (cond ((nil? (cdr l)) (car l))
	 (else (last (cdr l))))))

(define stick-on-end
  (\ (a l)
   (cond ((nil? l) (cons a ()))
	 (else (cons (car l) (stick-on-end a (cdr l)))))))

(define cut-last
  (\ (l)
   (cond ((nil? (cdr l)) ())
	 (else (cons (car l) (cut-last (cdr l)))))))

(define reverse
  (\ (o)
   (cond ((nil? o) ())
	 (else (cons
		(last o)
		(reverse (cut-last o)))))))

(define string-to-int-h
  (\ (s p)
   (cond ((nil? s) 0)
	 (else (+
		(* (char-to-int (car s)) (pow 10 p))
		(string-to-int-h (cdr s) (+ p 1)))))))

(define string-to-int
  (\ (s)
   (string-to-int-h (reverse s) 0)))
