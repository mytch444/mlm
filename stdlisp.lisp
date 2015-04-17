(def else 1)
(def t 1)
(def f ())

(def abs (\ a (cond
		((> 0 a) (- 0 a))
		(else a))))

(def < (\ a b (> b a)))
(def >= (\ a b (cond
		((> a b) t)
		((= a b) t)
		(else f))))

