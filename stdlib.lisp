(def else 1)
(def t 1)
(def f ())

(def nil? (\ a (is () a)))

(def abs (\ a (cond
		((> 0 a) (- 0 a))
		(else a))))

(def < (\ a b (> b a)))
(def >= (\ a b (cond
		((> a b) t)
		((= a b) t)
		(else f))))

(def <= (\ a b (cond
		((= a b) t)
		((< a b) t)
		(else f))))

(def twice
	(\ a (* 2 a)))

(def pow (\ a b
		(cond 	((= b 0) 1)
			((> b 0) (* a (pow a (- b 1)))))))

(def map (\ f l
	(cond	((nil? l) l)
		(else (cons (f (car l)) (map f (cdr l))))
	)))

(def nth (\ i l
	(cond 	((= i 0) (car l))
		(else (nth (- i 1) (cdr l))))))

(def concat (\ c l
	(cond 	((nil? c) l)
		(else (cons (car c) (concat (cdr c) l))))))

(def not (\ a (cond 	(a f)
			(else t))))

(def != (\ a b
	(not (= a b))))

(def and (\ a b
	(cond
		(a (cond 	(b t)
				(else f)))
		(else f))))

(def or (\ a b
	(cond	(a t)
		(b t)
		(else f))))

(def last (\ l
	(cond 	((nil? (cdr l)) (car l))
		(else (last (cdr l))))))

(def stick-on-end (\ a l
	(cond	((nil? l) (cons a ()))
		(else (cons (car l) (stick-on-end a (cdr l)))))))

(def cut-last (\ l
	(cond	((nil? (cdr l)) ())
		(else (cons (car l) (cut-last (cdr l)))))))

(def reverse (\ o
	(cond 	((nil? o) ())
		(else (cons
			(last o)
			(reverse (cut-last o)))))))
