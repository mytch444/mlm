(def stdin 0)
(def stdout 1)

(def readln-h (\ a (cond
		((= a '\n') ())
		(else (cons a (readln-h (read)))))))

(def readln (\ (readln-h (read))))

